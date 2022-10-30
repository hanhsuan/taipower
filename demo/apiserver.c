#define CISSON_IMPLEMENTATION
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

  if (json_string[0] == '\0') {
    rjson("{\"ERROR\": \"Empty payload\"}", &json_tree);
  } else {
    if (rjson(json_string, &json_tree) == JSON_ERROR_NO_ERRORS) {
      char *type =
          to_string_pointer_fail(&json_tree, query(&json_tree, "/type"), "");
      if (!memcmp("\"residential\"", type, sizeof("\"residential\"") - 1) ||
          !memcmp("\"commercial\"", type, sizeof("\"commercial\"") - 1)) {
        meter_ntou_charge(&json_tree);
      } else if (!memcmp("\"simple_tou\"", type,
                         sizeof("\"simple_tou\"") - 1)) {
        meter_simple_tou(&json_tree);
      } else if (!memcmp("\"power_tou\"", type, sizeof("\"power_tou\"") - 1)) {
        power_tou(&json_tree);
      } else if (!memcmp("\"high_voltage_tou\"", type,
                         sizeof("\"high_voltage_tou\"") - 1) ||
                 !memcmp("\"extra_high_voltage_tou\"", type,
                         sizeof("\"extra_high_voltage_tou\"") - 1)) {
        voltage(&json_tree);
      } else {
        start_state(&json_tree, NULL, NULL);
        rjson("{\"ERROR\": \"type is not supported\"}", &json_tree);
      }
    }
  }

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
