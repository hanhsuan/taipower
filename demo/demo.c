#define CISSON_IMPLEMENTATION
#include "cisson.h"
#include "taipower.h"
#include <stdio.h>
#include <string.h>

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

/**
 * @brief meter_ntou_rate 依照 level 成員進行 qsort 排序用之比較程式
 */
int level_cmp(const void *p1, const void *p2) {
  return (((const meter_ntou_rate *)p1)->level >
          ((const meter_ntou_rate *)p2)->level);
}

/**
 * @brief 設定表燈(住商)功率因素物件
 * @param max_kw 最大需量
 * @param json_tree json 設定內容
 * @param pf 用於儲存設定之 power_factor_info 物件
 * @return TAIPOWER_ERROR 或 TAIPOWER_SUCC
 */
int meter_power_factor_init(long max_kw, struct json_tree *json_tree,
                            power_factor_info *pf) {
  char *endptr = NULL;
  char *tmp = NULL;

  MARCO_STRTOL(long power_factor, tmp, endptr, json_tree, "/power_factor");

  pf->threshold_for_counted =
      ((max_kw < 20) ? TAIPOWER_DONT_COUNT_POWER_FACTOR : 20);
  pf->fine_per_percentage = 0.1;
  pf->max_percentage_for_reward = 15;
  pf->reward_base_line = 80;
  pf->power_factor = power_factor;
  pf->reward_per_percentage = 0.1;

  return TAIPOWER_SUCC;
}

/**
 * @brief 表燈(住商)非時間電價
 * @param json_tree json 設定內容
 * @return TAIPOWER_ERROR 或 TAIPOWER_SUCC
 */
int meter_ntou_charge(struct json_tree *json_tree) {
  char *endptr = NULL;
  char *tmp = NULL;

  MARCO_STRTOL(long max_kw, tmp, endptr, json_tree, "/max_kw");

  power_factor_info pf;
  if (meter_power_factor_init(max_kw, json_tree, &pf) == TAIPOWER_ERROR) {
    fprintf(stdout, "init power factor failed\n");
    return TAIPOWER_ERROR;
  }

  MARCO_STRTOD(double energy_consumption, tmp, endptr, json_tree,
               "/energy_consumption");

  struct token *root = NULL;
  root = query(json_tree, "/level");
  if (root == NULL) {
    fprintf(stdout, "there is no level informations\n");
    return TAIPOWER_ERROR;
  }

  struct token *cur = next_child(&(json_tree->tokens), root, NULL);
  int array_size = 0;
  while (cur != NULL) {
    cur = next_child(&(json_tree->tokens), root, cur);
    array_size++;
  }
  if (array_size == 0) {
    fprintf(stdout, "there is no level informations\n");
    return TAIPOWER_ERROR;
  }
  cur = next_child(&(json_tree->tokens), root, NULL);
  meter_ntou_rate levels[array_size];
  while (cur != NULL) {
    array_size--;
    struct json_tree level_json = {0};
    rjson(to_string_pointer(json_tree, cur), &level_json);
    tmp = to_string_pointer(&level_json,
                            query(&level_json, "/consumption_level"));
    levels[array_size].level = strtol(tmp, &endptr, 10);
    if (endptr == tmp) {
      if (!memcmp("\"above\"", tmp, sizeof("\"above\"") - 1)) {
        levels[array_size].level = TAIPOWER_EXCEED_KWH;
      } else {
        return TAIPOWER_ERROR;
      }
    }

    MARCO_STRTOD(levels[array_size].rate, tmp, endptr, json_tree,
                 "/consumption_rate");

    cur = next_child(&(json_tree->tokens), root, cur);
  }
  array_size = sizeof(levels) / sizeof(meter_ntou_rate);

  qsort(levels, array_size, sizeof(meter_ntou_rate), level_cmp);

  meter_ntou_basic_info info = {
      .level_count = array_size, .levels = levels, .power_factor = pf};

  fprintf(stdout, "charge:[%lf]\n",
          meter_ntou_charge_calc(energy_consumption, info));

  return TAIPOWER_SUCC;
}

/**
 * @brief 表燈(住商)簡易時間電價
 * @param json_tree json 設定內容
 * @return TAIPOWER_ERROR 或 TAIPOWER_SUCC
 */
int meter_simple_tou(struct json_tree *json_tree) {
  char *endptr = NULL;
  char *tmp = NULL;

  MARCO_STRTOL(long max_kw, tmp, endptr, json_tree, "/max_kw");

  meter_tou_a_o1_engery_consumption ec;

  MARCO_STRTOD(ec.peak, tmp, endptr, json_tree, "/energy_consumption/peak");

  MARCO_STRTOD(ec.off_peak, tmp, endptr, json_tree,
               "/energy_consumption/off_peak");

  MARCO_STRTOD(ec.sun_sat_off_peak, tmp, endptr, json_tree,
               "/energy_consumption/sun_sat_off_peak");

  meter_tou_a_o1_basic_info info;

  MARCO_STRTOD(info.customer_charge, tmp, endptr, json_tree,
               "/info/customer_charge");

  MARCO_STRTOD(info.portion_of_usage_limit, tmp, endptr, json_tree,
               "/info/portion_of_usage_limit");

  MARCO_STRTOD(info.peak_rate, tmp, endptr, json_tree, "/info/peak_rate");

  MARCO_STRTOD(info.off_peak_rate, tmp, endptr, json_tree,
               "/info/off_peak_rate");

  MARCO_STRTOD(info.sun_sat_off_peak, tmp, endptr, json_tree,
               "/info/sun_sat_off_peak");

  MARCO_STRTOD(info.exceed_limit_rate, tmp, endptr, json_tree,
               "/info/exceed_limit_rate");

  if (meter_power_factor_init(max_kw, json_tree, &info.power_factor) ==
      TAIPOWER_ERROR) {
    fprintf(stdout, "init power factor failed\n");
    return TAIPOWER_ERROR;
  }

  if (*to_string_pointer(json_tree, query(json_tree, "/option")) == '2') {
    meter_tou_a_o2_engery_consumption ec2;
    meter_tou_a_o2_basic_info info2;

    MARCO_STRTOD(ec2.partial_peak, tmp, endptr, json_tree,
                 "/energy_consumption/partial_peak");

    ec2.peak = ec.peak;
    ec2.off_peak = ec.off_peak;
    ec2.sun_sat_off_peak = ec.sun_sat_off_peak;

    MARCO_STRTOD(info2.partial_peak_rate, tmp, endptr, json_tree,
                 "/info/partial_peak_rate");

    info2.customer_charge = info.customer_charge;
    info2.portion_of_usage_limit = info.portion_of_usage_limit;
    info2.peak_rate = info.peak_rate;
    info2.off_peak_rate = info.off_peak_rate;
    info2.sun_sat_off_peak = info.sun_sat_off_peak;
    info2.exceed_limit_rate = info.exceed_limit_rate;
    info2.power_factor = info.power_factor;

    fprintf(stdout, "charge:[%lf]\n", meter_tou_a_o2_charge_calc(ec2, info2));
  } else {
    fprintf(stdout, "charge:[%lf]\n", meter_tou_a_o1_charge_calc(ec, info));
  }

  return TAIPOWER_SUCC;
}

/**
 * @brief 低壓時間電價
 * @param json_tree json 設定內容
 * @return TAIPOWER_ERROR 或 TAIPOWER_SUCC
 */
int power_tou(struct json_tree *json_tree) {
  char *endptr = NULL;
  char *tmp = NULL;
  char *basic_type = NULL;

  tou_o1_engery_consumption ec;

  MARCO_STRTOD(ec.peak, tmp, endptr, json_tree, "/energy_consumption/peak");

  MARCO_STRTOD(ec.off_peak, tmp, endptr, json_tree,
               "/energy_consumption/off_peak");

  MARCO_STRTOD(ec.sat_partial_peak, tmp, endptr, json_tree,
               "/energy_consumption/sat_partial_peak");

  power_tou_o1_basic_info info;

  MARCO_STRTOL(long power_factor, tmp, endptr, json_tree, "/power_factor");

  info.info.power_factor.threshold_for_counted = 20;
  info.info.power_factor.fine_per_percentage = 0.1;
  info.info.power_factor.max_percentage_for_reward = 15;
  info.info.power_factor.reward_base_line = 80;
  info.info.power_factor.power_factor = power_factor;
  info.info.power_factor.reward_per_percentage = 0.1;

  basic_type = to_string_pointer(json_tree, query(json_tree, "/basic_type"));
  if (!memcmp("\"device\"", basic_type, sizeof("\"device\"") - 1)) {

    MARCO_STRTOD(info.capacity.customer_charge, tmp, endptr, json_tree,
                 "/info/customer_charge");

    MARCO_STRTOD(info.capacity.based_on_the_contracted_installed_capacity_rate,
                 tmp, endptr, json_tree,
                 "/info/based_on_the_contracted_installed_capacity_rate");
  } else if (!memcmp("\"demand\"", basic_type, sizeof("\"demand\"") - 1)) {
    MARCO_STRTOD(info.info.customer_charge, tmp, endptr, json_tree,
                 "/info/customer_charge");
    MARCO_STRTOD(info.info.regular_contract.demand_charge_rate, tmp, endptr,
                 json_tree, "/info/regular_contract/demand_charge_rate");
    MARCO_STRTOD(info.info.sat_partial_peak_contract.demand_charge_rate, tmp,
                 endptr, json_tree,
                 "/info/sat_partial_peak_contract/demand_charge_rate");
    MARCO_STRTOD(info.info.off_peak_contract.demand_charge_rate, tmp, endptr,
                 json_tree, "/info/off_peak_contract/demand_charge_rate");
  }

  MARCO_STRTOD(info.info.regular_contract.contracted_demand, tmp, endptr,
               json_tree, "/info/regular_contract/contracted_demand");

  MARCO_STRTOD(info.info.regular_contract.energy_charge_rate, tmp, endptr,
               json_tree, "/info/regular_contract/energy_charge_rate");

  MARCO_STRTOD(info.info.sat_partial_peak_contract.contracted_demand, tmp,
               endptr, json_tree,
               "/info/sat_partial_peak_contract/contracted_demand");

  MARCO_STRTOD(info.info.sat_partial_peak_contract.energy_charge_rate, tmp,
               endptr, json_tree,
               "/info/sat_partial_peak_contract/energy_charge_rate");

  MARCO_STRTOD(info.info.off_peak_contract.contracted_demand, tmp, endptr,
               json_tree, "/info/off_peak_contract/contracted_demand");

  MARCO_STRTOD(info.info.off_peak_contract.energy_charge_rate, tmp, endptr,
               json_tree, "/info/off_peak_contract/energy_charge_rate");

  tou_o1_charge charge;
  if (TAIPOWER_SUCC != power_tou_o1_charge_calc(&charge, ec, info)) {
    fprintf(stdout, "power_tou_o1_charge_calc failed!!\n");
    return TAIPOWER_ERROR;
  }

  fprintf(stdout,
          "Total charge:[%lf]\n->basic charge:[%lf]\n->energy charge:[%lf]\n",
          charge.total_charge, charge.detail.basic_charge,
          charge.detail.energy_charge);

  return TAIPOWER_SUCC;
}

/**
 * @brief 高壓及特高壓時間電價
 * @param json_tree json 設定內容
 * @return TAIPOWER_ERROR 或 TAIPOWER_SUCC
 */
int voltage(struct json_tree *json_tree) {
  char *endptr = NULL;
  char *tmp = NULL;

  tou_o1_engery_consumption ec;
  MARCO_STRTOD(ec.peak, tmp, endptr, json_tree, "energy_consumption/peak");

  MARCO_STRTOD(ec.sat_partial_peak, tmp, endptr, json_tree,
               "energy_consumption/sat_partial_peak");

  MARCO_STRTOD(ec.off_peak, tmp, endptr, json_tree,
               "energy_consumption/off_peak");

  tou_o1_basic_info info;

  MARCO_STRTOD(info.regular_contract.contracted_demand, tmp, endptr, json_tree,
               "info/regular_contract/contracted_demand");

  MARCO_STRTOD(info.regular_contract.demand_charge_rate, tmp, endptr, json_tree,
               "info/regular_contract/demand_charge_rate");

  MARCO_STRTOD(info.regular_contract.energy_charge_rate, tmp, endptr, json_tree,
               "info/regular_contract/energy_charge_rate");

  MARCO_STRTOD(info.sat_partial_peak_contract.contracted_demand, tmp, endptr,
               json_tree, "info/sat_partial_peak_contract/contracted_demand");

  MARCO_STRTOD(info.sat_partial_peak_contract.demand_charge_rate, tmp, endptr,
               json_tree, "info/sat_partial_peak_contract/demand_charge_rate");

  MARCO_STRTOD(info.sat_partial_peak_contract.energy_charge_rate, tmp, endptr,
               json_tree, "info/sat_partial_peak_contract/energy_charge_rate");

  MARCO_STRTOD(info.off_peak_contract.contracted_demand, tmp, endptr, json_tree,
               "info/off_peak_contract/contracted_demand");

  MARCO_STRTOD(info.off_peak_contract.demand_charge_rate, tmp, endptr,
               json_tree, "info/off_peak_contract/demand_charge_rate");

  MARCO_STRTOD(info.off_peak_contract.energy_charge_rate, tmp, endptr,
               json_tree, "info/off_peak_contract/energy_charge_rate");

  MARCO_STRTOL(long power_factor, tmp, endptr, json_tree, "/power_factor");

  info.power_factor.threshold_for_counted = 20;
  info.power_factor.fine_per_percentage = 0.1;
  info.power_factor.max_percentage_for_reward = 15;
  info.power_factor.reward_base_line = 80;
  info.power_factor.power_factor = power_factor;
  info.power_factor.reward_per_percentage = 0.1;

  if (*to_string_pointer(json_tree, query(json_tree, "/option")) == '2') {
    tou_o2_engery_consumption ec2;
    tou_o2_basic_info info2;

    MARCO_STRTOD(ec2.partial_peak, tmp, endptr, json_tree,
                 "/energy_consumption/partial_peak");

    ec2.peak = ec.peak;
    ec2.off_peak = ec.off_peak;
    ec2.sat_partial_peak = ec.sat_partial_peak;

    MARCO_STRTOD(info2.partial_peak_contract.contracted_demand, tmp, endptr,
                 json_tree, "info/partial_peak_contract/contracted_demand");

    MARCO_STRTOD(info2.partial_peak_contract.demand_charge_rate, tmp, endptr,
                 json_tree, "info/partial_peak_contract/demand_charge_rate");

    MARCO_STRTOD(info2.partial_peak_contract.energy_charge_rate, tmp, endptr,
                 json_tree, "info/partial_peak_contract/energy_charge_rate");

    info2.customer_charge = info.customer_charge;
    info2.regular_contract.contracted_demand =
        info.regular_contract.contracted_demand;
    info2.regular_contract.demand_charge_rate =
        info.regular_contract.demand_charge_rate;
    info2.regular_contract.energy_charge_rate =
        info.regular_contract.energy_charge_rate;

    info2.sat_partial_peak_contract.contracted_demand =
        info.sat_partial_peak_contract.contracted_demand;
    info2.sat_partial_peak_contract.demand_charge_rate =
        info.sat_partial_peak_contract.demand_charge_rate;
    info2.sat_partial_peak_contract.energy_charge_rate =
        info.sat_partial_peak_contract.energy_charge_rate;

    info2.off_peak_contract.contracted_demand =
        info.off_peak_contract.contracted_demand;
    info2.off_peak_contract.demand_charge_rate =
        info.off_peak_contract.demand_charge_rate;
    info2.off_peak_contract.energy_charge_rate =
        info.off_peak_contract.energy_charge_rate;

    info2.power_factor = info.power_factor;

    tou_o2_charge charge;
    high_voltage_tou_o2_charge_calc(&charge, ec2, info2);

    fprintf(stdout,
            "Total charge:[%lf]\n->basic charge:[%lf]\n->energy "
            "charge:[%lf]\n",
            charge.total_charge, charge.detail.basic_charge,
            charge.detail.energy_charge);
  } else {
    tou_o1_charge charge;
    high_voltage_tou_o1_charge_calc(&charge, ec, info);

    fprintf(stdout,
            "Total charge:[%lf]\n->basic charge:[%lf]\n->energy "
            "charge:[%lf]\n",
            charge.total_charge, charge.detail.basic_charge,
            charge.detail.energy_charge);
  }

  return TAIPOWER_SUCC;
}

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
  fread(json_string, sizeof(char), file_size, fp);

  rjson(json_string, &json_tree);
  char *type = to_string_pointer(&json_tree, query(&json_tree, "/type"));
  if (!memcmp("\"residential\"", type, sizeof("\"residential\"") - 1) ||
      !memcmp("\"commercial\"", type, sizeof("\"commercial\"") - 1)) {
    meter_ntou_charge(&json_tree);
  } else if (!memcmp("\"simple_tou\"", type, sizeof("\"simple_tou\"") - 1)) {
    meter_simple_tou(&json_tree);
  } else if (!memcmp("\"power_tou\"", type, sizeof("\"power_tou\"") - 1)) {
    power_tou(&json_tree);
  } else if (!memcmp("\"high_voltage_tou\"", type,
                     sizeof("\"high_voltage_tou\"") - 1) ||
             !memcmp("\"extra_high_voltage_tou\"", type,
                     sizeof("\"extra_high_voltage_tou\"") - 1)) {
    voltage(&json_tree);
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
