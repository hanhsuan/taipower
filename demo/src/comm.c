#include "comm.h"

int level_cmp(const void *p1, const void *p2) {
  return (((const non_time_of_use_rate *)p1)->level >
          ((const non_time_of_use_rate *)p2)->level);
}

int meter_power_factor_init(long max_kw, struct json_tree *json_tree,
                            power_factor_info *pf) {
  char *endptr = NULL;
  char *tmp = NULL;

  MARCO_STRTOL(long power_factor, tmp, endptr, json_tree, "/power_factor");

  pf->min_demand_threshold =
      ((max_kw < 20) ? TAIPOWER_DONT_COUNT_POWER_FACTOR : 20);
  pf->surcharge_per_percentage = 0.1;
  pf->max_adjustment_percentage = 15;
  pf->baseline_percentage = 80;
  pf->power_factor = power_factor;
  pf->discount_per_percentage = 0.1;

  return TAIPOWER_SUCC;
}

int ntou_charge(struct json_tree *json_tree) {
  char *endptr = NULL;
  char *tmp = NULL;

  MARCO_STRTOL(long max_kw, tmp, endptr, json_tree, "/max_kw");

  power_factor_info pf = {0};
  if (meter_power_factor_init(max_kw, json_tree, &pf) == TAIPOWER_ERROR) {
    fprintf(stdout, "init power factor failed\n");
    return TAIPOWER_ERROR;
  }

  MARCO_STRTOD(double energy_consumption, tmp, endptr, json_tree,
               "/energy_consumption");

  struct json_token *root = NULL;
  root = query(json_tree, "/level");
  if (root == NULL) {
    fprintf(stdout, "there is no level informations\n");
    return TAIPOWER_ERROR;
  }

  struct json_token *cur = next_child(json_tree, root, NULL);
  int array_size = 0;
  while (cur != NULL) {
    cur = next_child(json_tree, root, cur);
    array_size++;
  }
  if (array_size == 0) {
    fprintf(stdout, "there is no level informations\n");
    return TAIPOWER_ERROR;
  }
  cur = next_child(json_tree, root, NULL);
  non_time_of_use_rate levels[array_size];
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

    cur = next_child(json_tree, root, cur);
  }
  array_size = sizeof(levels) / sizeof(non_time_of_use_rate);

  qsort(levels, array_size, sizeof(non_time_of_use_rate), level_cmp);

  non_time_of_use_basic_info info = {
      .level_count = array_size, .levels = levels, .power_factor = pf};

  double charge = non_time_of_use_charge_calc(energy_consumption, info);
  fprintf(stdout, "charge:[%lf]\n", charge);
  MARCO_OUTPUT_JSON_SINGLE(json_tree, charge, 100);
  return TAIPOWER_SUCC;
}

int meter_simple_time_of_use(struct json_tree *json_tree) {
  char *endptr = NULL;
  char *tmp = NULL;

  MARCO_STRTOL(long max_kw, tmp, endptr, json_tree, "/max_kw");

  time_of_use_a_option_1_energy_consumption ec = {0};

  MARCO_STRTOD(ec.peak, tmp, endptr, json_tree, "/energy_consumption/peak");

  MARCO_STRTOD(ec.off_peak, tmp, endptr, json_tree,
               "/energy_consumption/off_peak");

  MARCO_STRTOD(ec.weekend_off_peak, tmp, endptr, json_tree,
               "/energy_consumption/weekend_off_peak");

  time_of_use_a_option_1_basic_info info = {0};

  MARCO_STRTOD(info.customer_charge, tmp, endptr, json_tree,
               "/info/customer_charge");

  MARCO_STRTOD(info.usage_limit_kwh, tmp, endptr, json_tree,
               "/info/usage_limit_kwh");

  MARCO_STRTOD(info.peak_rate, tmp, endptr, json_tree, "/info/peak_rate");

  MARCO_STRTOD(info.off_peak_rate, tmp, endptr, json_tree,
               "/info/off_peak_rate");

  MARCO_STRTOD(info.weekend_off_peak, tmp, endptr, json_tree,
               "/info/weekend_off_peak");

  MARCO_STRTOD(info.exceed_limit_rate, tmp, endptr, json_tree,
               "/info/exceed_limit_rate");

  if (meter_power_factor_init(max_kw, json_tree, &info.power_factor) ==
      TAIPOWER_ERROR) {
    fprintf(stdout, "init power factor failed\n");
    return TAIPOWER_ERROR;
  }

  if (*to_string_pointer(json_tree, query(json_tree, "/option")) == '2') {
    time_of_use_a_option_2_energy_consumption ec2 = {.peak = ec.peak,
                                                    .off_peak = ec.off_peak,
                                                    .weekend_off_peak =
                                                        ec.weekend_off_peak};
    time_of_use_a_option_2_basic_info info2 = {
        .customer_charge = info.customer_charge,
        .usage_limit_kwh = info.usage_limit_kwh,
        .peak_rate = info.peak_rate,
        .off_peak_rate = info.off_peak_rate,
        .weekend_off_peak = info.weekend_off_peak,
        .exceed_limit_rate = info.exceed_limit_rate,
        .power_factor = info.power_factor};

    MARCO_STRTOD(ec2.partial_peak, tmp, endptr, json_tree,
                 "/energy_consumption/partial_peak");

    MARCO_STRTOD(info2.partial_peak_rate, tmp, endptr, json_tree,
                 "/info/partial_peak_rate");

    double charge = time_of_use_a_option_2_charge_calc(ec2, info2);
    fprintf(stdout, "charge:[%lf]\n", charge);
    MARCO_OUTPUT_JSON_SINGLE(json_tree, charge, 100);
  } else {
    double charge = time_of_use_a_option_1_charge_calc(ec, info);
    fprintf(stdout, "charge:[%lf]\n", charge);
    MARCO_OUTPUT_JSON_SINGLE(json_tree, charge, 100);
  }

  return TAIPOWER_SUCC;
}

int power_time_of_use(struct json_tree *json_tree) {
  char *endptr = NULL;
  char *tmp = NULL;
  char *basic_type = NULL;

  time_of_use_option_1_energy_consumption ec = {0};

  MARCO_STRTOD(ec.peak, tmp, endptr, json_tree, "/energy_consumption/peak");

  MARCO_STRTOD(ec.off_peak, tmp, endptr, json_tree,
               "/energy_consumption/off_peak");

  MARCO_STRTOD(ec.sat_partial_peak, tmp, endptr, json_tree,
               "/energy_consumption/sat_partial_peak");

  low_voltage_time_of_use_option_1_basic_info info = {0};

  MARCO_STRTOL(long power_factor, tmp, endptr, json_tree, "/power_factor");

  info.info.power_factor.min_demand_threshold = 20;
  info.info.power_factor.surcharge_per_percentage = 0.1;
  info.info.power_factor.max_adjustment_percentage = 15;
  info.info.power_factor.baseline_percentage = 80;
  info.info.power_factor.power_factor = power_factor;
  info.info.power_factor.discount_per_percentage = 0.1;

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

  time_of_use_option_1_charge charge = {0};
  if (TAIPOWER_SUCC != low_voltage_time_of_use_option_1_charge_calc(&charge, ec, info)) {
    fprintf(stdout, "low_voltage_time_of_use_option_1_charge_calc failed!!\n");
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

  time_of_use_option_1_energy_consumption ec = {0};
  MARCO_STRTOD(ec.peak, tmp, endptr, json_tree, "energy_consumption/peak");

  MARCO_STRTOD(ec.sat_partial_peak, tmp, endptr, json_tree,
               "energy_consumption/sat_partial_peak");

  MARCO_STRTOD(ec.off_peak, tmp, endptr, json_tree,
               "energy_consumption/off_peak");

  time_of_use_option_1_basic_info info = {0};

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

  info.power_factor.min_demand_threshold = 20;
  info.power_factor.surcharge_per_percentage = 0.1;
  info.power_factor.max_adjustment_percentage = 15;
  info.power_factor.baseline_percentage = 80;
  info.power_factor.power_factor = power_factor;
  info.power_factor.discount_per_percentage = 0.1;

  if (*to_string_pointer(json_tree, query(json_tree, "/option")) == '2') {
    time_of_use_option_2_energy_consumption ec2 = {.peak = ec.peak,
                                            .off_peak = ec.off_peak,
                                            .sat_partial_peak =
                                                ec.sat_partial_peak};
    time_of_use_option_2_basic_info info2 = {
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

    time_of_use_option_2_charge charge = {0};
    high_voltage_time_of_use_option_2_charge_calc(&charge, ec2, info2);

    fprintf(stdout,
            "Total charge:[%lf]\n->basic charge:[%lf]\n->energy "
            "charge:[%lf]\n",
            charge.total_charge, charge.detail.basic_charge,
            charge.detail.energy_charge);
    MARCO_OUTPUT_JSON(json_tree, charge, 100);
  } else {
    time_of_use_option_1_charge charge = {0};
    high_voltage_time_of_use_option_1_charge_calc(&charge, ec, info);

    fprintf(stdout,
            "Total charge:[%lf]\n->basic charge:[%lf]\n->energy "
            "charge:[%lf]\n",
            charge.total_charge, charge.detail.basic_charge,
            charge.detail.energy_charge);
    MARCO_OUTPUT_JSON(json_tree, charge, 100);
  }

  return TAIPOWER_SUCC;
}