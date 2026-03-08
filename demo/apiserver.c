#define JSON_IMPL
#include "comm.h"
#include "sagui.h"
#include <event.h>
#include <signal.h>

void sighandler(int signal, short events, void *base) {
  event_base_loopbreak(base);
}

void req_cb(__SG_UNUSED void *cls, struct sg_httpreq *req,
            struct sg_httpres *res) {
  struct json_tree json_tree = {0};
  struct sg_str *payload = sg_httpreq_payload(req);
  const char *json_string = sg_str_content(payload);
  char *json_string_copy = strdup(json_string);

  if (json_string_copy[0] == '\0') {
    rjson("{\"ERROR\": \"Empty payload\"}", &json_tree);
  } else {
    rjson(json_string_copy, &json_tree);
    if (json_tree.cur_state != JSS_ERROR_STATE) {
      char *type = to_string_pointer(&json_tree, query(&json_tree, "/type"));
      int index =
          (sizeof(taipower_functions) / sizeof(struct supported_function));
      while (index--) {
        if (!memcmp(taipower_functions[index].name, type,
                    taipower_functions[index].cmp_length)) {
          taipower_functions[index].function(&json_tree);
          break;
        }
      }
      if (index < 0) {
        json_tree = (struct json_tree){0};
        rjson("{\"ERROR\": \"type is not supported\"}", &json_tree);
      }
    } else {
      rjson("{\"ERROR\": \"json format error\"}", &json_tree);
    }
  }

  free(json_string_copy);
  sg_httpres_send(res, to_string(&json_tree), "application/json", 200);
}

/**
 * @brief 使用 taipower.c 之範例程式
 */
int main(int argc, char **argv) {
  struct sg_httpsrv *srv;
  struct event ev_sigterm;
  struct event_base *ev_base;
  uint16_t port;

  if (argc != 2) {
    printf("%s <PORT>\n", argv[0]);
    return EXIT_FAILURE;
  }

  port = strtol(argv[1], NULL, 10);
  srv = sg_httpsrv_new(req_cb, NULL);
  if (!sg_httpsrv_listen(srv, port, false)) {
    sg_httpsrv_free(srv);
    return EXIT_FAILURE;
  }

  ev_base = event_init();

  evsignal_set(&ev_sigterm, SIGTERM, sighandler, ev_base);
  evsignal_add(&ev_sigterm, NULL);
  event_dispatch();
  sg_httpsrv_shutdown(srv);
  sg_httpsrv_free(srv);
  return EXIT_SUCCESS;
}
