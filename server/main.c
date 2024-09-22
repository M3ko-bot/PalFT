#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "csocket-server.h"
#include "cthreads.h"

#include "httpserver.h"
#include "types.h"

static struct httpserver server;

void callback(struct csocket_server_client *client, int socket_index, struct httpparser_request *request) {
  (void) socket_index; (void) request;

  printf("[main]: Requisição recebida\n");

  char resp[] = "Base para o projeto da UCP :)";

  char resp_len[10];
  snprintf(resp_len, sizeof(resp_len), "%ld", strlen(resp));

  struct httpserver_response response = {
    .client = client,
    .status = 200,
    .headers = (struct httpserver_header *) &(struct httpserver_header []) {
      {
        .key = "Content-Type",
        .value = "application/json"
      },
      {
        .key = "Content-Length",
        .value = resp_len
      }
    },
    .headers_length = 2,
    .body = resp,
    .body_length = strlen(resp)
  };

  httpserver_send_response(&response);

  printf("[main]: Resposta enviada\n");
}

void disconnect_callback(struct csocket_server_client *client, int socket_index) {
  (void) client; (void) socket_index;

  printf("[main]: Cliente desconectado\n");
}


void handle_sig_int(int signal) {
  (void) signal;

  printf("\n[main]: Parando o servidor...\n");

  httpserver_stop_server(&server);

  printf("[main]: Pronto! Tchau.\n");
}

int main(void) {
  signal(SIGINT, handle_sig_int);

  httpserver_start_server(&server);

  printf("[main]: Servidor iniciado na porta %d\n", 8888);

  httpserver_handle_request(&server, callback, disconnect_callback);
}

#ifdef _WIN32
  int APIENTRY WinMain(HINSTANCE h_instance, HINSTANCE h_prev_instance, LPSTR lp_cmd_line, int n_cmd_show) {
    (void) h_instance; (void) h_prev_instance; (void) lp_cmd_line; (void) n_cmd_show;

    setbuf(stdout, NULL);

    return main();
  }
#endif
