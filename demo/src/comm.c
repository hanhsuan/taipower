#include "comm.h"

int level_cmp(const void *p1, const void *p2) {
  return (((const struct meter_ntou_rate *)p1)->level >
          ((const struct meter_ntou_rate *)p2)->level);
}

int meter_power_factor_init(long max_kw, struct json_tree *json_tree,
                            struct power_factor_info *pf) {
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

int meter_ntou_charge(struct json_tree *json_tree) {
  char *endptr = NULL;
  char *tmp = NULL;

  MARCO_STRTOL(long max_kw, tmp, endptr, json_tree, "/max_kw");

  struct power_factor_info pf = {0};
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
  struct meter_ntou_rate levels[array_size];
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
  array_size = sizeof(levels) / sizeof(struct meter_ntou_rate);

  qsort(levels, array_size, sizeof(struct meter_ntou_rate), level_cmp);

  struct meter_ntou_basic_info info = {
      .level_count = array_size, .levels = levels, .power_factor = pf};

  double charge = meter_ntou_charge_calc(energy_consumption, info);
  fprintf(stdout, "charge:[%lf]\n", charge);
  MARCO_OUTPUT_JSON_SINGLE(json_tree, charge, 100);
  return TAIPOWER_SUCC;
}

int meter_simple_tou(struct json_tree *json_tree) {
  char *endptr = NULL;
  char *tmp = NULL;

  MARCO_STRTOL(long max_kw, tmp, endptr, json_tree, "/max_kw");

  struct meter_tou_a_o1_engery_consumption ec = {0};

  MARCO_STRTOD(ec.peak, tmp, endptr, json_tree, "/energy_consumption/peak");

  MARCO_STRTOD(ec.off_peak, tmp, endptr, json_tree,
               "/energy_consumption/off_peak");

  MARCO_STRTOD(ec.sun_sat_off_peak, tmp, endptr, json_tree,
               "/energy_consumption/sun_sat_off_peak");

  struct meter_tou_a_o1_basic_info info = {0};

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
    struct meter_tou_a_o2_engery_consumption ec2 = {.peak = ec.peak,
                                                    .off_peak = ec.off_peak,
                                                    .sun_sat_off_peak =
                                                        ec.sun_sat_off_peak};
    struct meter_tou_a_o2_basic_info info2 = {
        .customer_charge = info.customer_charge,
        .portion_of_usage_limit = info.portion_of_usage_limit,
        .peak_rate = info.peak_rate,
        .off_peak_rate = info.off_peak_rate,
        .sun_sat_off_peak = info.sun_sat_off_peak,
        .exceed_limit_rate = info.exceed_limit_rate,
        .power_factor = info.power_factor};

    MARCO_STRTOD(ec2.partial_peak, tmp, endptr, json_tree,
                 "/energy_consumption/partial_peak");

    MARCO_STRTOD(info2.partial_peak_rate, tmp, endptr, json_tree,
                 "/info/partial_peak_rate");

    double charge = meter_tou_a_o2_charge_calc(ec2, info2);
    fprintf(stdout, "charge:[%lf]\n", charge);
    MARCO_OUTPUT_JSON_SINGLE(json_tree, charge, 100);
  } else {
    double charge = meter_tou_a_o1_charge_calc(ec, info);
    fprintf(stdout, "charge:[%lf]\n", charge);
    MARCO_OUTPUT_JSON_SINGLE(json_tree, charge, 100);
  }

  return TAIPOWER_SUCC;
}

int power_tou(struct json_tree *json_tree) {
  char *endptr = NULL;
  char *tmp = NULL;
  char *basic_type = NULL;

  struct tou_o1_engery_consumption ec = {0};

  MARCO_STRTOD(ec.peak, tmp, endptr, json_tree, "/energy_consumption/peak");

  MARCO_STRTOD(ec.off_peak, tmp, endptr, json_tree,
               "/energy_consumption/off_peak");

  MARCO_STRTOD(ec.sat_partial_peak, tmp, endptr, json_tree,
               "/energy_consumption/sat_partial_peak");

  struct power_tou_o1_basic_info info = {0};

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

  struct tou_o1_charge charge = {0};
  if (TAIPOWER_SUCC != power_tou_o1_charge_calc(&charge, ec, info)) {
    fprintf(stdout, "power_tou_o1_charge_calc failed!!\n");
    return TAIPOWER_ERROR;
  }

  fprintf(stdout,
          "Total charge:[%lf]\n->basic charge:[%lf]\n->energy charge:[%lf]\n",
          charge.total_charge, charge.detail.basic_charge,
          charge.detail.energy_charge);

  MARCO_OUTPUT_JSON(json_tree, charge, 100);

  return TAIPOWER_SUCC;
}

int voltage(struct json_tree *json_tree) {
  char *endptr = NULL;
  char *tmp = NULL;

  struct tou_o1_engery_consumption ec = {0};
  MARCO_STRTOD(ec.peak, tmp, endptr, json_tree, "energy_consumption/peak");

  MARCO_STRTOD(ec.sat_partial_peak, tmp, endptr, json_tree,
               "energy_consumption/sat_partial_peak");

  MARCO_STRTOD(ec.off_peak, tmp, endptr, json_tree,
               "energy_consumption/off_peak");

  struct tou_o1_basic_info info = {0};

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
    struct tou_o2_engery_consumption ec2 = {.peak = ec.peak,
                                            .off_peak = ec.off_peak,
                                            .sat_partial_peak =
                                                ec.sat_partial_peak};
    struct tou_o2_basic_info info2 = {
        .customer_charge = info.customer_charge,
        .regular_contract.contracted_demand =
            info.regular_contract.contracted_demand,
        .regular_contract.demand_charge_rate =
            info.regular_contract.demand_charge_rate,
        .regular_contract.energy_charge_rate =
            info.regular_contract.energy_charge_rate,
        .sat_partial_peak_contract.contracted_demand =
            info.sat_partial_peak_contract.contracted_demand,
        .sat_partial_peak_contract.demand_charge_rate =
            info.sat_partial_peak_contract.demand_charge_rate,
        .sat_partial_peak_contract.energy_charge_rate =
            info.sat_partial_peak_contract.energy_charge_rate,
        .off_peak_contract.contracted_demand =
            info.off_peak_contract.contracted_demand,
        .off_peak_contract.demand_charge_rate =
            info.off_peak_contract.demand_charge_rate,
        .off_peak_contract.energy_charge_rate =
            info.off_peak_contract.energy_charge_rate,
        .power_factor = info.power_factor};

    MARCO_STRTOD(ec2.partial_peak, tmp, endptr, json_tree,
                 "/energy_consumption/partial_peak");

    MARCO_STRTOD(info2.partial_peak_contract.contracted_demand, tmp, endptr,
                 json_tree, "info/partial_peak_contract/contracted_demand");

    MARCO_STRTOD(info2.partial_peak_contract.demand_charge_rate, tmp, endptr,
                 json_tree, "info/partial_peak_contract/demand_charge_rate");

    MARCO_STRTOD(info2.partial_peak_contract.energy_charge_rate, tmp, endptr,
                 json_tree, "info/partial_peak_contract/energy_charge_rate");

    struct tou_o2_charge charge = {0};
    high_voltage_tou_o2_charge_calc(&charge, ec2, info2);

    fprintf(stdout,
            "Total charge:[%lf]\n->basic charge:[%lf]\n->energy "
            "charge:[%lf]\n",
            charge.total_charge, charge.detail.basic_charge,
            charge.detail.energy_charge);
    MARCO_OUTPUT_JSON(json_tree, charge, 100);
  } else {
    struct tou_o1_charge charge = {0};
    high_voltage_tou_o1_charge_calc(&charge, ec, info);

    fprintf(stdout,
            "Total charge:[%lf]\n->basic charge:[%lf]\n->energy "
            "charge:[%lf]\n",
            charge.total_charge, charge.detail.basic_charge,
            charge.detail.energy_charge);
    MARCO_OUTPUT_JSON(json_tree, charge, 100);
  }

  return TAIPOWER_SUCC;
}