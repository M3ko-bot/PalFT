#include <stdio.h>
#include <string.h>
#include <signal.h>

#include <libpq-fe.h>

#include "csocket-server.h"
#include "cthreads.h"
#include "queryparser.h"
#include "pjsonb.h"
#include "httpserver.h"
#include "types.h"
#include "tablec.h"
#include "fq_utils.h"

static struct httpserver server;
static char *conn_login = "user=postgres dbname=ramzen port=5432 host=localhost password=postgres";
static struct tablec_ht sessions;

#define SENHA "senha"

void normalize_string(char *str) {
  /* INFO: Evita brechas de segurança em relação ao SQL Injection */
  for (int i = 0; str[i] != '\0'; i++) {
    if (str[i] == '\'') {
      str[i] = '"';
    }
  }
}

void normalize_query_value(char *str) {
  /* INFO: Converte os + pra espaços */
  for (int i = 0; str[i] != '\0'; i++) {
    if (str[i] == '+') {
      str[i] = ' ';
    }
  }
}

void callback(struct csocket_server_client *client, int socket_index, struct httpparser_request *request) {
  (void) socket_index; (void) request;

  printf("[main]: Requisição recebida\n");

  struct qparser_info parse_info;
  struct qparser_query queries[10];
  qparser_init(&parse_info, queries, 10);

  qparser_parse(&parse_info, request->path);

  printf("[main]: Path: %s\n", request->path);

  /* INFO: /api/produtos/lista */
  /* INFO: (resposta): [{ "nome": "produto", "imagem": "/...", "preco": 50, "avaliacao": 5 }, ...] */
  if (strncmp(request->path, "/api/produtos/lista", sizeof("/api/produtos/lista") - 1) == 0) {
    printf("[main]: Listando produtos\n");

    char sql_command[1024];
    snprintf(sql_command, sizeof(sql_command), "SELECT * FROM produtos;");

    PGconn *conn = PQconnectdb(conn_login);
    if (PQstatus(conn) != CONNECTION_OK) {
      fprintf(stderr, "[main]: Erro ao conectar com o banco de dados: %s\n", PQerrorMessage(conn));
      PQfinish(conn);

      struct httpserver_response response = {
        .client = client,
        .status = 500,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    PGresult *res = PQexec(conn, sql_command);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
      fprintf(stderr, "[main]: Erro ao listar produtos: %s\n", PQerrorMessage(conn));
      PQclear(res);
      PQfinish(conn);

      struct httpserver_response response = {
        .client = client,
        .status = 400,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    struct pjsonb array;
    pjsonb_init(&array, PJSONB_ARRAY);

    for (int i = 0; i < PQntuples(res); i++) {
      pjsonb_enter_object(&array, NULL);

      char *nome = PQgetvalue(res, i, 0);
      char *imagem = PQgetvalue(res, i, 1);

      pjsonb_set_string(&array, "nome", nome, strlen(nome));
      pjsonb_set_string(&array, "imagem", imagem, strlen(imagem));
      pjsonb_set_int(&array, "preco", atoi(PQgetvalue(res, i, 2)));
      pjsonb_set_int(&array, "avaliacao", 5);

      pjsonb_leave_object(&array);
    }

    PQclear(res);
    PQfinish(conn);

    pjsonb_end(&array);

    char content_length_str[10];
    snprintf(content_length_str, sizeof(content_length_str), "%d", array.position);
    
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
          .value = content_length_str
        },
        {
          .key = "Access-Control-Allow-Origin",
          .value = "*"
        }
      },
      .headers_length = 3,
      .body = array.string,
      .body_length = array.position
    };

    httpserver_send_response(&response);

    pjsonb_free(&array);

    return;


  /* INFO: /api/register?name=UwU&email=exemplo@example.com&password=HASHED_SENHA */
  /* INFO: (resposta): xxxxxxxxxxxxxxxxxxxxxxxxxxxx (sessão) */
  } 
  
  else if (strncmp(request->path, "/api/register", sizeof("/api/register") - 1) == 0) {
    printf("[main]: Registrando\n");

    if (qparser_get_query(&parse_info, "name") == NULL || qparser_get_query(&parse_info, "email") == NULL || qparser_get_query(&parse_info, "password") == NULL) {
      struct httpserver_response response = {
        .client = client,
        .status = 400,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    char *name = qparser_get_query(&parse_info, "name")->value;
    char *email = qparser_get_query(&parse_info, "email")->value;
    char *password = qparser_get_query(&parse_info, "password")->value;

    normalize_string(name);
    normalize_string(email);
    normalize_string(password);

    char sql_command[4096];
    snprintf(sql_command, sizeof(sql_command), "INSERT INTO usuarios (nome, email, senha) VALUES ('%s', '%s', '%s');", name, email, password);

    PGconn *conn = PQconnectdb(conn_login);
    if (PQstatus(conn) != CONNECTION_OK) {
      fprintf(stderr, "[main]: Erro ao conectar com o banco de dados: %s\n", PQerrorMessage(conn));
      PQfinish(conn);

      struct httpserver_response response = {
        .client = client,
        .status = 500,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    PGresult *res = PQexec(conn, sql_command);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
      fprintf(stderr, "[main]: Erro ao registrar: %s\n", PQerrorMessage(conn));
      PQclear(res);
      PQfinish(conn);

      struct httpserver_response response = {
        .client = client,
        .status = 400,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    PQclear(res);
    PQfinish(conn);

    struct httpserver_response response = {
      .client = client,
      .status = 204,
      .headers = (struct httpserver_header *) &(struct httpserver_header []) {
        {
          .key = "Access-Control-Allow-Origin",
          .value = "*"
        }
      },
      .headers_length = 1,
      .body = NULL,
      .body_length = 0
    };

    httpserver_send_response(&response);

    return;

  /* INFO: /api/login?email=exemplo@example.com&password=HASHED */
  /* INFO: (resposta): xxxxxxxxxxxxxxxxxxxxxxxxxxxx (sessão) */
  }

  else if (strncmp(request->path, "/api/login", sizeof("/api/login") - 1) == 0) {
    printf("[main]: Logando\n");

    if (qparser_get_query(&parse_info, "email") == NULL || qparser_get_query(&parse_info, "password") == NULL) {
      struct httpserver_response response = {
        .client = client,
        .status = 400,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    char *email = qparser_get_query(&parse_info, "email")->value;
    char *password = qparser_get_query(&parse_info, "password")->value;

    normalize_string(email);
    normalize_string(password);

    char sql_command[(1024 * 2) + 68 + 1];
    snprintf(sql_command, sizeof(sql_command), "SELECT * FROM usuarios WHERE email='%s' AND senha='%s';", email, password);

    PGconn *conn = PQconnectdb(conn_login);
    if (PQstatus(conn) != CONNECTION_OK) {
      fprintf(stderr, "[main]: Erro ao conectar com o banco de dados: %s\n", PQerrorMessage(conn));
      PQfinish(conn);

      struct httpserver_response response = {
        .client = client,
        .status = 500,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    PGresult *res = PQexec(conn, sql_command);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
      fprintf(stderr, "[main]: Erro ao logar: %s\n", PQerrorMessage(conn));
      PQclear(res);
      PQfinish(conn);

      struct httpserver_response response = {
        .client = client,
        .status = 400,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    if (PQntuples(res) == 0) {
      PQclear(res);
      PQfinish(conn);

      struct httpserver_response response = {
        .client = client,
        .status = 403,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    char *session = malloc((16 + 1) * sizeof(char));
    frequenc_generate_session_id(session);

    char time_str[32];
    snprintf(time_str, sizeof(time_str), "%ld", time(NULL));

    tablec_set(&sessions, session, time_str);

    PQclear(res);
    PQfinish(conn);

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
          .value = "32"
        },
        {
          .key = "Access-Control-Allow-Origin",
          .value = "*"
        }
      },
      .headers_length = 3,
      .body = session,
      .body_length = 32
    };

    httpserver_send_response(&response);

    return;
  /* INFO: /api/session/check?session=xxxxxxxxxxxxxxxxxxxxxxxx */
  /* INFO: (resposta): true ou false */
  } else if (strncmp(request->path, "/api/session/check", sizeof("/api/session/check") - 1) == 0) {
    printf("[main]: Checando sessão\n");

  /* INFO: /api/produtos/add?nome=...&imagem=...&preco=...&senha=... */
  /* INFO: (resposta): 204 ou 400 ou 403 (status code) */
  } else if (strncmp(request->path, "/api/produtos/add", sizeof("/api/produtos/add") - 1) == 0) {
    printf("[main]: Adicionando produto\n");
  
    if (qparser_get_query(&parse_info, "nome") == NULL || qparser_get_query(&parse_info, "imagem") == NULL || qparser_get_query(&parse_info, "preco") == NULL) {
      struct httpserver_response response = {
        .client = client,
        .status = 400,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    if (qparser_get_query(&parse_info, "senha") == NULL || strcmp(qparser_get_query(&parse_info, "senha")->value, SENHA) != 0) {
      struct httpserver_response response = {
        .client = client,
        .status = 403,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    char *nome = qparser_get_query(&parse_info, "nome")->value;
    char *imagem = qparser_get_query(&parse_info, "imagem")->value;
    int preco = atoi(qparser_get_query(&parse_info, "preco")->value);
    if (preco == 0) {
      struct httpserver_response response = {
        .client = client,
        .status = 400,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    normalize_string(nome);
    normalize_query_value(nome);
    normalize_string(imagem);

    char sql_command[(1024 * 2) + 68 + (sizeof(int) * 3) + 1];
    snprintf(sql_command, sizeof(sql_command), "INSERT INTO produtos (nome, imagem, preco) VALUES ('%s', '%s', %d);", nome, imagem, preco);

    PGconn *conn = PQconnectdb(conn_login);
    if (PQstatus(conn) != CONNECTION_OK) {
      fprintf(stderr, "[main]: Erro ao conectar com o banco de dados: %s\n", PQerrorMessage(conn));
      PQfinish(conn);

      struct httpserver_response response = {
        .client = client,
        .status = 500,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    PGresult *res = PQexec(conn, sql_command);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
      fprintf(stderr, "[main]: Erro ao adicionar produto: %s\n", PQerrorMessage(conn));
      PQclear(res);
      PQfinish(conn);

      struct httpserver_response response = {
        .client = client,
        .status = 400,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    PQclear(res);
    PQfinish(conn);

    struct httpserver_response response = {
      .client = client,
      .status = 204,
      .headers = (struct httpserver_header *) &(struct httpserver_header []) {
        {
          .key = "Access-Control-Allow-Origin",
          .value = "*"
        }
      },
      .headers_length = 1,
      .body = NULL,
      .body_length = 0
    };

    httpserver_send_response(&response);    

    return;
  /* INFO: /api/produtos/del?nome=...&senha=... */
  /* INFO: (resposta): 204 ou 400 ou 403 (status code) */
  } else if (strncmp(request->path, "/api/produtos/del", sizeof("/api/produtos/del") - 1) == 0) {
    printf("[main]: Deletando produto\n");

    if (qparser_get_query(&parse_info, "nome") == NULL) {
      struct httpserver_response response = {
        .client = client,
        .status = 400,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    if (qparser_get_query(&parse_info, "senha") == NULL || strcmp(qparser_get_query(&parse_info, "senha")->value, SENHA) != 0) {
      struct httpserver_response response = {
        .client = client,
        .status = 403,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    char *nome = qparser_get_query(&parse_info, "nome")->value;

    normalize_string(nome);
    normalize_query_value(nome);

    char sql_command[1024 + 33 + 1];
    snprintf(sql_command, sizeof(sql_command), "DELETE from produtos WHERE='%s';", nome);

    PGconn *conn = PQconnectdb(conn_login);
    if (PQstatus(conn) != CONNECTION_OK) {
      fprintf(stderr, "[main]: Erro ao conectar com o banco de dados: %s\n", PQerrorMessage(conn));
      PQfinish(conn);

      struct httpserver_response response = {
        .client = client,
        .status = 500,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    PGresult *res = PQexec(conn, sql_command);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
      fprintf(stderr, "[main]: Erro ao deletar produto: %s\n", PQerrorMessage(conn));
      PQclear(res);
      PQfinish(conn);

      struct httpserver_response response = {
        .client = client,
        .status = 400,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    PQclear(res);
    PQfinish(conn);

    struct httpserver_response response = {
      .client = client,
      .status = 204,
      .headers = (struct httpserver_header *) &(struct httpserver_header []) {
        {
          .key = "Access-Control-Allow-Origin",
          .value = "*"
        }
      },
      .headers_length = 0,
      .body = NULL,
      .body_length = 0
    };

    httpserver_send_response(&response);    

    return;
  /* INFO: /api/carrinho/add?produto=ID */
  /* INFO: (resposta): 204 ou 400 ou 403 (status code) */
  }

  else if (strncmp(request->path, "/api/carrinho/add", sizeof("/api/carrinho/add") - 1) == 0) {
    printf("[main]: Adicionando produto ao carrinho\n");

    if (qparser_get_query(&parse_info, "produto") == NULL) {
      struct httpserver_response response = {
        .client = client,
        .status = 400,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    char *produto = qparser_get_query(&parse_info, "produto")->value;

    normalize_string(produto);
    normalize_query_value(produto);

    char *session = qparser_get_query(&parse_info, "session")->value;

    struct tablec_bucket *session_info = tablec_get(&sessions, session);
    if (session_info == NULL) {
      struct httpserver_response response = {
        .client = client,
        .status = 403,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    char sql_command[2048];
    snprintf(sql_command, sizeof(sql_command), "INSERT INTO carrinho (usuario, produto) VALUES ('%s', '%s');", session_info->key, produto);

    PGconn *conn = PQconnectdb(conn_login);
    if (PQstatus(conn) != CONNECTION_OK) {
      fprintf(stderr, "[main]: Erro ao conectar com o banco de dados: %s\n", PQerrorMessage(conn));
      PQfinish(conn);

      struct httpserver_response response = {
        .client = client,
        .status = 500,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    PGresult *res = PQexec(conn, sql_command);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
      fprintf(stderr, "[main]: Erro ao adicionar produto ao carrinho: %s\n", PQerrorMessage(conn));
      PQclear(res);
      PQfinish(conn);

      struct httpserver_response response = {
        .client = client,
        .status = 400,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    PQclear(res);
    PQfinish(conn);

    struct httpserver_response response = {
      .client = client,
      .status = 204,
      .headers = (struct httpserver_header *) &(struct httpserver_header []) {
        {
          .key = "Access-Control-Allow-Origin",
          .value = "*"
        }
      },
      .headers_length = 1,
      .body = NULL,
      .body_length = 0
    };

    httpserver_send_response(&response);

    return;
  /* INFO: /api/carrinho/del?produto=ID */
  /* INFO: (resposta): 204 ou 400 ou 403 (status code) */
  }

  else if (strncmp(request->path, "/api/carrinho/del", sizeof("/api/carrinho/del") - 1) == 0) {
    printf("[main]: Deletando produto do carrinho\n");

    if (qparser_get_query(&parse_info, "produto") == NULL) {
      struct httpserver_response response = {
        .client = client,
        .status = 400,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    char *produto = qparser_get_query(&parse_info, "produto")->value;

    normalize_string(produto);
    normalize_query_value(produto);

    char *session = qparser_get_query(&parse_info, "session")->value;

    struct tablec_bucket *session_info = tablec_get(&sessions, session);
    if (session_info == NULL) {
      struct httpserver_response response = {
        .client = client,
        .status = 403,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    char sql_command[2048];
    snprintf(sql_command, sizeof(sql_command), "DELETE from carrinho WHERE usuario='%s' AND produto='%s';", session_info->key, produto);

    PGconn *conn = PQconnectdb(conn_login);
    if (PQstatus(conn) != CONNECTION_OK) {
      fprintf(stderr, "[main]: Erro ao conectar com o banco de dados: %s\n", PQerrorMessage(conn));
      PQfinish(conn);

      struct httpserver_response response = {
        .client = client,
        .status = 500,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    PGresult *res = PQexec(conn, sql_command);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
      fprintf(stderr, "[main]: Erro ao deletar produto do carrinho: %s\n", PQerrorMessage(conn));
      PQclear(res);
      PQfinish(conn);

      struct httpserver_response response = {
        .client = client,
        .status = 400,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    PQclear(res);
    PQfinish(conn);

    struct httpserver_response response = {
      .client = client,
      .status = 204,
      .headers = (struct httpserver_header *) &(struct httpserver_header []) {
        {
          .key = "Access-Control-Allow-Origin",
          .value = "*"
        }
      },
      .headers_length = 1,
      .body = NULL,
      .body_length = 0
    };

    httpserver_send_response(&response);

    return;
  /* INFO: /api/carrinho/lista */
  /* INFO: (resposta): [{ "produto": "ID" }, ...] */
  }

  else if (strncmp(request->path, "/api/carrinho/lista", sizeof("/api/carrinho/lista") - 1) == 0) {
    printf("[main]: Listando carrinho\n");

    char *session = qparser_get_query(&parse_info, "session")->value;

    struct tablec_bucket *session_info = tablec_get(&sessions, session);
    if (session_info == NULL) {
      struct httpserver_response response = {
        .client = client,
        .status = 403,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    char sql_command[1024 + 33 + 1];
    snprintf(sql_command, sizeof(sql_command), "SELECT * FROM carrinho WHERE usuario='%s';", session_info->key);

    PGconn *conn = PQconnectdb(conn_login);
    if (PQstatus(conn) != CONNECTION_OK) {
      fprintf(stderr, "[main]: Erro ao conectar com o banco de dados: %s\n", PQerrorMessage(conn));
      PQfinish(conn);

      struct httpserver_response response = {
        .client = client,
        .status = 500,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    PGresult *res = PQexec(conn, sql_command);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
      fprintf(stderr, "[main]: Erro ao listar carrinho: %s\n", PQerrorMessage(conn));
      PQclear(res);
      PQfinish(conn);

      struct httpserver_response response = {
        .client = client,
        .status = 400,
        .headers = (struct httpserver_header *) &(struct httpserver_header []) {
          {
            .key = "Access-Control-Allow-Origin",
            .value = "*"
          }
        },
        .headers_length = 1,
        .body = NULL,
        .body_length = 0
      };

      httpserver_send_response(&response);

      return;
    }

    struct pjsonb array;
    pjsonb_init(&array, PJSONB_ARRAY);

    for (int i = 0; i < PQntuples(res); i++) {
      pjsonb_enter_object(&array, NULL);

      char *produto = PQgetvalue(res, i, 1);

      pjsonb_set_string(&array, "produto", produto, strlen(produto));

      pjsonb_leave_object(&array);
    }

    PQclear(res);
    PQfinish(conn);

    pjsonb_end(&array);

    char content_length_str[10];
    snprintf(content_length_str, sizeof(content_length_str), "%d", array.position);
    
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
          .value = content_length_str
        },
        {
          .key = "Access-Control-Allow-Origin",
          .value = "*"
        }
      },
      .headers_length = 3,
      .body = array.string,
      .body_length = array.position
    };

    httpserver_send_response(&response);

    pjsonb_free(&array);

    return;
  }

  char resp[] = "Base para o projeto da UCP :)";

  char resp_len[10];
  snprintf(resp_len, sizeof(resp_len), "%ld", strlen(resp));

  struct httpserver_response response = {
    .client = client,
    .status = 404,
    .headers = (struct httpserver_header *) &(struct httpserver_header []) {
      {
        .key = "Content-Type",
        .value = "application/json"
      },
      {
        .key = "Content-Length",
        .value = resp_len
      },
      {
        .key = "Access-Control-Allow-Origin",
        .value = "*"
      }
    },
    .headers_length = 3,
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

void libpq_notice_processor(void *arg, const char *message) {
  (void) arg; (void) message;

  if (strstr(message, "\"produtos\" already exists") == NULL)
    fprintf(stderr, "[main]: %s", message);
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

  /* INFO: A hashtable vai expandir quando precisar, não precisa de muito espaço de início */
  tablec_init(&sessions, 4);

  printf("[main]: Tabela de sessões criada\n");

  PGconn *conn = PQconnectdb("user=postgres dbname=ramzen port=5432 host=localhost password=postgres");
  if (PQstatus(conn) != CONNECTION_OK) {
    fprintf(stderr, "[main]: Erro ao conectar com o banco de dados: %s\n", PQerrorMessage(conn));
    PQfinish(conn);

    return 1;
  }

  printf("[main]: Conectado ao banco de dados\n");

  /* INFO: Evita o log do libpq (que é inútil pra o comando abaixo) */
  PQsetNoticeProcessor(conn, libpq_notice_processor, NULL);

  /* INFO: Cria a área de produtos na database */
  PGresult *res = PQexec(conn, "CREATE TABLE IF NOT EXISTS produtos (nome TEXT NOT NULL, imagem TEXT NOT NULL, preco INT NOT NULL);");
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    fprintf(stderr, "[main]: Erro ao criar a tabela produtos: %s\n", PQerrorMessage(conn));
    PQclear(res);
    PQfinish(conn);

    return 1;
  }
  PQclear(res);

  /* INFO: Cria a área de usuários na database */
  res = PQexec(conn, "CREATE TABLE IF NOT EXISTS usuarios (nome TEXT NOT NULL, email TEXT NOT NULL, senha TEXT NOT NULL);");
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    fprintf(stderr, "[main]: Erro ao criar a tabela usuarios: %s\n", PQerrorMessage(conn));
    PQclear(res);
    PQfinish(conn);

    return 1;
  }
  PQclear(res);

  printf("[main]: Tabela produtos criada\n");

  /* INFO: Cria a área de carrinho na database */
  res = PQexec(conn, "CREATE TABLE IF NOT EXISTS carrinho (usuario TEXT NOT NULL, produto TEXT NOT NULL);");
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    fprintf(stderr, "[main]: Erro ao criar a tabela carrinho: %s\n", PQerrorMessage(conn));
    PQclear(res);
    PQfinish(conn);

    return 1;
  }
  PQclear(res);
  PQfinish(conn);

  printf("[main]: Tabela carrinho criada\n");

  httpserver_handle_request(&server, callback, disconnect_callback);
}

#ifdef _WIN32
  int APIENTRY WinMain(HINSTANCE h_instance, HINSTANCE h_prev_instance, LPSTR lp_cmd_line, int n_cmd_show) {
    (void) h_instance; (void) h_prev_instance; (void) lp_cmd_line; (void) n_cmd_show;

    setbuf(stdout, NULL);

    return main();
  }
#endif
