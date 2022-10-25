#ifndef TAIPOWER_COMM_H
#define TAIPOWER_COMM_H

#include <stdio.h>
#include <string.h>
#include "taipower.h"
#define WANT_LIBC  /** 避免 cisson 與其他標準函式庫衝突，要放置在所有標頭檔最後，並且在有 libc 的環境下定義此參數 */
#include "cisson.h"

#define MARCO_STRTOL(value, str, endptr, json_tree, key)                       \
  str = to_string_pointer(json_tree, query(json_tree, key));                   \
  value = strtol(str, &endptr, 10);                                            \
  if (endptr == str) {                                                         \
    fprintf(stdout, "convert for %.s to long failed\n", str);                  \
    return TAIPOWER_ERROR;                                                     \
  }

#define MARCO_STRTOD(value, str, endptr, json_tree, key)                       \
  str = to_string_pointer(json_tree, query(json_tree, key));                   \
  value = strtod(str, &endptr);                                                \
  if (endptr == str) {                                                         \
    fprintf(stdout, "convert for %.s to double failed\n", str);                \
    return TAIPOWER_ERROR;                                                     \
  }

#define MARCO_OUTPUT_JSON(json_tree, charge, max_length)                       \
  {                                                                            \
    char output_json[max_length];                                              \
    memset(output_json, 0, sizeof(output_json));                               \
    start_state(json_tree, NULL, NULL);                                        \
    snprintf(output_json, sizeof(output_json) - 1,                             \
             "{\"Total_charge\": %.2lf, \"Basic_charge\": %.2lf, "             \
             "\"Energy_charge\": %.2lf}",                                      \
             charge.total_charge, charge.detail.basic_charge,                  \
             charge.detail.energy_charge);                                     \
    rjson(output_json, json_tree);                                             \
  }

#define MARCO_OUTPUT_JSON_SINGLE(json_tree, charge, max_length)                \
  {                                                                            \
    char output_json[max_length];                                              \
    memset(output_json, 0, sizeof(output_json));                               \
    start_state(json_tree, NULL, NULL);                                        \
    snprintf(output_json, sizeof(output_json) - 1,                             \
             "{\"Total_charge\": %.2lf}", charge);                             \
    rjson(output_json, json_tree);                                             \
  }

/**
 * @brief meter_ntou_rate 依照 level 成員進行 qsort 排序用之比較程式
 */
int level_cmp(const void *p1, const void *p2);


/**
 * @brief 設定表燈(住商)功率因素物件
 * @param max_kw 最大需量
 * @param json_tree json 設定內容
 * @param pf 用於儲存設定之 power_factor_info 物件
 * @return TAIPOWER_ERROR 或 TAIPOWER_SUCC
 */
int meter_power_factor_init(long max_kw, struct json_tree *json_tree,
                            power_factor_info *pf);

/**
 * @brief 表燈(住商)非時間電價
 * @param json_tree json 設定內容
 * @return TAIPOWER_ERROR 或 TAIPOWER_SUCC
 */
int meter_ntou_charge(struct json_tree *json_tree);


/**
 * @brief 表燈(住商)簡易時間電價
 * @param json_tree json 設定內容
 * @return TAIPOWER_ERROR 或 TAIPOWER_SUCC
 */
int meter_simple_tou(struct json_tree *json_tree);

/**
 * @brief 低壓時間電價
 * @param json_tree json 設定內容
 * @return TAIPOWER_ERROR 或 TAIPOWER_SUCC
 */
int power_tou(struct json_tree *json_tree);

/**
 * @brief 高壓及特高壓時間電價
 * @param json_tree json 設定內容
 * @return TAIPOWER_ERROR 或 TAIPOWER_SUCC
 */
int voltage(struct json_tree *json_tree);

#endif