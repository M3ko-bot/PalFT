/* 
 * Copyright (c) 2024, The PerformanC Organization
 * License available at LICENSE file (BSD 2-Clause)
*/

#ifndef URLPARSER_H
#define URLPARSER_H

struct qparser_query {
  char key[64];
  char value[1024];
};

struct qparser_info {
  int length;
  int queries_length;
  struct qparser_query *queries;
};

void qparser_init(struct qparser_info *parse_info, struct qparser_query *buffer, int length);

void qparser_parse(struct qparser_info *parse_info, char *url);

struct qparser_query *qparser_get_query(struct qparser_info *parse_info, char *key);

#endif