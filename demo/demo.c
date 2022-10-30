#define CISSON_IMPLEMENTATION
#include "comm.h"

/**
 * @brief 使用 taipower.c 之範例程式
 */
int main(int argc, char **argv) {
  FILE *fp = NULL;
  long file_size = 0;
  char *json_string = NULL;
  struct json_tree json_tree = {0};

  if (argc < 2) {
    fprintf(stdout, "you should give me one json file for calculating\n");
    return TAIPOWER_ERROR;
  }

  fp = fopen(argv[1], "ro");

  if (fp == NULL) {
    fprintf(stdout, "open file failed\n");
    return TAIPOWER_ERROR;
  }

  fseek(fp, 0L, SEEK_END);
  file_size = ftell(fp);
  json_string = calloc(sizeof(char), (file_size + 1));
  if (json_string == NULL) {
    fprintf(stdout, "get memory failed\n");
    goto FREE;
  }
  rewind(fp);

  if (file_size < 1) {
    fprintf(stdout, "Empty file\n");
  } else {
    fread(json_string, sizeof(char), file_size, fp);

    if (rjson(json_string, &json_tree) == JSON_ERROR_NO_ERRORS) {
      char *type = to_string_pointer(&json_tree, query(&json_tree, "/type"));
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
        fprintf(stdout, "type is not supported\n");
      }
    }
  }

FREE:
  if (json_string) {
    free(json_string);
  }
  if (fp) {
    fclose(fp);
  }

  return 0;
}
