/* 
 * Copyright (c) 2024, The PerformanC Organization
 * License available at LICENSE file (BSD 2-Clause)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "libtstr.h"

#include "httpparser.h"

void httpparser_init_request(struct httpparser_request *http_request, struct httpparser_header *buffer, int length) {
  http_request->path = NULL;
  http_request->headers_length = 0;
  http_request->headers_max_length = length;
  http_request->headers = buffer;
  http_request->body = NULL;
}

static int _httpparser_check_method(const char *method) {
  char *http_methods[] = { "GET", "HEAD", "POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE", "PATCH" };

  for (size_t i = 0; i < sizeof(http_methods) / sizeof(*http_methods); i++) {
    if (strcmp(method, http_methods[i]) == 0) return 0;
  }

  return -1;
}

static void _httpparser_to_lower_case(char *str) {
  for (int i = 0; str[i]; i++) {
    str[i] = tolower(str[i]);
  }
}

int httpparser_parse_request(struct httpparser_request *http_request, const char *request, int request_length) {
  struct tstr_string_token headers_end;
  tstr_find_between(&headers_end, request, NULL, 0, "\r\n\r\n", 0);

  if (headers_end.end == 0) return -1;

  struct tstr_string_token method_token;
  tstr_find_between(&method_token, request, NULL, 0, " ", 0);

  if (method_token.end == 0) return -1;

  memcpy(http_request->method, request, (size_t)method_token.end);
  http_request->method[method_token.end] = '\0';

  if (_httpparser_check_method(http_request->method) == -1) return -1;

  struct tstr_string_token path_token;
  tstr_find_between(&path_token, request, NULL, method_token.end + 1, " ", 0);

  if (path_token.end == 0) return -1;

  http_request->path = malloc((size_t)(path_token.end - path_token.start + 1) * sizeof(char));
  memcpy(http_request->path, request + path_token.start, (size_t)(path_token.end - path_token.start));
  http_request->path[path_token.end - path_token.start] = '\0';

  struct tstr_string_token version_token;
  tstr_find_between(&version_token, request, NULL, path_token.end + 1 + (int)sizeof("HTTP/") - 1, "\r\n", 0);

  if (version_token.end == 0) return -1;

  memcpy(http_request->version, request + version_token.start, (size_t)(version_token.end - version_token.start));
  http_request->version[version_token.end - version_token.start] = '\0';

  if (strcmp(http_request->version, "1.1")) return -1;

  int content_length = 0;
  struct tstr_string_token last_header = {
    .start = 0,
    .end = version_token.end
  };

  while (last_header.end != headers_end.end) {
    struct tstr_string_token header;
    tstr_find_between(&header, request, NULL, last_header.end + 2, "\r\n", 0);

    struct tstr_string_token header_name;
    tstr_find_between(&header_name, request, NULL, header.start, ": ", header.end);

    if (header_name.start == 0 || header_name.end == 0 || http_request->headers_length >= http_request->headers_max_length) return -1;

    int key_length = header_name.end - header_name.start;
    int value_length = header.end - header_name.end - 2;

    memcpy(http_request->headers[http_request->headers_length].key, request + header_name.start, (size_t)key_length);
    http_request->headers[http_request->headers_length].key[key_length] = '\0';
    _httpparser_to_lower_case(http_request->headers[http_request->headers_length].key);
    memcpy(http_request->headers[http_request->headers_length].value, request + header_name.end + 2, (size_t)value_length);
    http_request->headers[http_request->headers_length].value[value_length] = '\0';

    if (strcmp(http_request->headers[http_request->headers_length].key, "content-length") == 0)
      content_length = atoi(request + header.start + sizeof("content-length: ") - 1);

    last_header = header;

    http_request->headers_length++;
  }

  struct httpparser_header *transfer_encoding_header = httpparser_get_header(http_request, "transfer-encoding");

  if (content_length > 0 || transfer_encoding_header != NULL) {
    struct httpparser_header *content_type_header = httpparser_get_header(http_request, "content-type");
    if (content_type_header == NULL) return -1;

    if (transfer_encoding_header != NULL && strcmp(transfer_encoding_header->value, "chunked") == 0) {
      const char *body_and_length = request + headers_end.end + 4;

      struct tstr_string_token chunk_size;
      tstr_find_between(&chunk_size, body_and_length, NULL, 0, "\r\n", 0);

      if (chunk_size.end == 0) return -1;

      char chunk_size_str[5];
      memcpy(chunk_size_str, body_and_length, (size_t)chunk_size.end);
      chunk_size_str[chunk_size.end] = '\0';

      http_request->chunk_length = (int)strtol(chunk_size_str, NULL, 16);

      int requested_length = (request_length + headers_end.end - 4) - chunk_size.end - 2;

      if (requested_length > http_request->chunk_length) requested_length = http_request->chunk_length;

      http_request->body_length = (size_t)requested_length;
      http_request->body = malloc((size_t)requested_length * sizeof(char));
      memcpy((void *)http_request->body, (void *)(body_and_length + chunk_size.end + 2), (size_t)requested_length);

      /* TODO: Implement chunk handling */
      http_request->finished = http_request->body_length == (size_t)http_request->chunk_length;
    } else {
      if ((request_length - headers_end.end - 4) != content_length) return -1;

      http_request->body = malloc((size_t)content_length * sizeof(char));
      memcpy((void *)http_request->body, (void *)(request + headers_end.end + 4), (size_t)content_length);

      http_request->body_length = (size_t)content_length;
      http_request->finished = true;
    }
  } else {
    if (content_length > 0) return -1;
  }

  return 0;
}

void httpparser_free_request(struct httpparser_request *http_request) {
  if (http_request->path != NULL) {
    free(http_request->path);
    http_request->path = NULL;
  }
  if (http_request->body != NULL) {
    free(http_request->body);
    http_request->body = NULL;
  }
}

struct httpparser_header *httpparser_get_header(struct httpparser_request *http_request, const char *key) {
  int i = 0;

  while (i < http_request->headers_length) {
    if (strcmp(http_request->headers[i].key, key) == 0) return &http_request->headers[i];

    i++;
  }

  return NULL;
}