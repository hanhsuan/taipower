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
        fprintf(stdout, "type isn't supported:[%.*s]\n", 100, type);
      }
    } else {
      fprintf(stdout, "json format error\n");
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
