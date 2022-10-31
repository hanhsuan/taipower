#include "taipower.h"

double meter_ntou_charge_calc(double ec, struct meter_ntou_basic_info info) {
  double charge = 0;
  int level = 0;
  int level_diff = 0;

  for (level = 0; level < info.level_count; level++) {
    level_diff = ((level - 1) < 0) ? 0 : info.levels[level - 1].level;

    if (info.levels[level].level < ec &&
        info.levels[level].level != TAIPOWER_EXCEED_KWH) {
      charge +=
          info.levels[level].rate * (info.levels[level].level - level_diff);
    } else if ((ec - level_diff) > 0) {
      charge += info.levels[level].rate * (ec - level_diff);
      break;
    }
  }

  // 計算功率因素獎懲
  charge += (info.power_factor.threshold_for_counted ==
             TAIPOWER_DONT_COUNT_POWER_FACTOR)
                ? 0
                : power_factor_reward_calc(charge, info.power_factor);

  return charge;
}

double meter_tou_a_o1_charge_calc(struct meter_tou_a_o1_engery_consumption ec,
                                  struct meter_tou_a_o1_basic_info info) {
  double charge = 0;
  double exceed_ec =
      ec.peak + ec.off_peak + ec.sun_sat_off_peak - info.portion_of_usage_limit;
  // 基本電費
  charge += info.customer_charge;

  // 流動電費
  charge += info.peak_rate * ec.peak;
  charge += info.off_peak_rate * ec.off_peak;
  charge += info.sun_sat_off_peak * ec.sun_sat_off_peak;

  // 計算功率因素獎懲
  charge += (info.power_factor.threshold_for_counted ==
             TAIPOWER_DONT_COUNT_POWER_FACTOR)
                ? 0
                : power_factor_reward_calc(charge, info.power_factor);

  // 超額
  charge += (exceed_ec > 0) ? exceed_ec * info.exceed_limit_rate : 0;

  return charge;
}

double meter_tou_a_o2_charge_calc(struct meter_tou_a_o2_engery_consumption ec,
                                  struct meter_tou_a_o2_basic_info info) {
  double charge = 0;
  double exceed_ec = ec.peak + ec.partial_peak + ec.off_peak +
                     ec.sun_sat_off_peak - info.portion_of_usage_limit;
  // 基本電費
  charge += info.customer_charge;

  // 流動電費
  charge += info.peak_rate * ec.peak;
  charge += info.partial_peak_rate * ec.partial_peak;
  charge += info.off_peak_rate * ec.off_peak;
  charge += info.sun_sat_off_peak * ec.sun_sat_off_peak;

  // 計算功率因素獎懲
  charge += (info.power_factor.threshold_for_counted ==
             TAIPOWER_DONT_COUNT_POWER_FACTOR)
                ? 0
                : power_factor_reward_calc(charge, info.power_factor);

  // 超額
  charge += (exceed_ec > 0) ? exceed_ec * info.exceed_limit_rate : 0;

  return charge;
}

int tou_o1_charge_calc(struct tou_o1_charge *charge,
                       struct tou_o1_engery_consumption ec,
                       struct tou_o1_basic_info info) {
  double tmp = 0;
  double peak_exceed_contract = 0;
  double non_summer_exceed_contract = 0;
  double partial_peak_exceed_contract = 0;
  double off_peak_exceed_contract = 0;

  if (charge == NULL)
    return TAIPOWER_ERROR;

  /*
   * 僅訂週六半尖峰契約容量之用戶於週六半尖峰及離峰以外時間之用電，
   * 或僅訂離峰契約容量之用戶於離峰以外時間之用電，應就該用電最高需量及用電量，
   * 分別按相關時段單價之4.8倍計收基本電費，及1.6倍計收流動電費。
   */
  if (info.regular_contract.contracted_demand < TAIPOWER_FLOAT_TOLERANCE &&
      info.non_summer_contract.contracted_demand < TAIPOWER_FLOAT_TOLERANCE) {
    info.regular_contract.energy_charge_rate *=
        TAIPOWER_ONLY_SAT_PARTIAL_AND_OFF_PEAK_CONTRANT_ENERGY_FINE_RATE;
    charge->detail.basic_charge +=
        info.regular_contract.demand_charge_rate * ec.peak_max_demand *
        TAIPOWER_ONLY_SAT_PARTIAL_AND_OFF_PEAK_CONTRANT_DEMAND_FINE_RATE;
    // 避免後續又再算一次超約罰款，將尖峰最大需量設定為 0
    ec.peak_max_demand = 0;

    info.non_summer_contract.energy_charge_rate *=
        TAIPOWER_ONLY_SAT_PARTIAL_AND_OFF_PEAK_CONTRANT_ENERGY_FINE_RATE;
    charge->detail.basic_charge +=
        info.non_summer_contract.demand_charge_rate * ec.non_summer_max_demand *
        TAIPOWER_ONLY_SAT_PARTIAL_AND_OFF_PEAK_CONTRANT_DEMAND_FINE_RATE;

    // 避免後續又再算一次超約罰款，將非夏月最大需量設定為 0
    ec.non_summer_max_demand = 0;

    if (info.sat_partial_peak_contract.contracted_demand <
        TAIPOWER_FLOAT_TOLERANCE) {
      info.sat_partial_peak_contract.energy_charge_rate *=
          TAIPOWER_ONLY_SAT_PARTIAL_AND_OFF_PEAK_CONTRANT_ENERGY_FINE_RATE;
      charge->detail.basic_charge +=
          info.sat_partial_peak_contract.demand_charge_rate *
          ec.sat_partial_peak_max_demand *
          TAIPOWER_ONLY_SAT_PARTIAL_AND_OFF_PEAK_CONTRANT_DEMAND_FINE_RATE;
      // 避免後續又再算一次超約罰款，將週六半尖峰最大需量設定為 0
      ec.sat_partial_peak_max_demand = 0;
    }
  }

  // 開始計算基本電費
  tmp = (info.sat_partial_peak_contract.contracted_demand +
         info.off_peak_contract.contracted_demand) -
        ((info.regular_contract.contracted_demand +
          info.non_summer_contract.contracted_demand) *
         0.5);

  charge->detail.basic_charge +=
      ((ec.non_summer == 0) ? info.regular_contract.demand_charge_rate
                            : info.non_summer_contract.demand_charge_rate) *
          info.regular_contract.contracted_demand +
      (((tmp < 0) ? 0 : tmp) * info.off_peak_contract.demand_charge_rate) +
      info.customer_charge;

  // 開始計算流動電費
  // 尖峰電費
  charge->detail.energy_charge =
      ((ec.non_summer == 0) ? ec.peak : ec.non_summer) *
      info.regular_contract.energy_charge_rate;
  // 半尖峰電費
  charge->detail.energy_charge +=
      ec.sat_partial_peak * info.sat_partial_peak_contract.energy_charge_rate;
  // 離蜂電費
  charge->detail.energy_charge +=
      ec.off_peak * info.off_peak_contract.energy_charge_rate;

  // 開始計算超約罰款，在契約容量10%以下部分按2倍計收基本電費，超過契約容量10%部分按3倍計收基本電費
  // 週六半尖峰時間超約瓩數＝當月週六半尖峰時間用電最高需量－（經常契約容量＋非夏月契約容量＋週六半尖峰契約容量）
  partial_peak_exceed_contract =
      ec.sat_partial_peak_max_demand -
      (info.regular_contract.contracted_demand +
       info.non_summer_contract.contracted_demand +
       info.sat_partial_peak_contract.contracted_demand);

  // 離峰時間超約瓩數＝當月離峰時間用電最高需量－（經常契約容量＋非夏月契約容量＋週六半尖峰契約容量＋離峰契約容量）
  off_peak_exceed_contract = ec.off_peak_max_demand -
                             (info.regular_contract.contracted_demand +
                              info.non_summer_contract.contracted_demand +
                              info.sat_partial_peak_contract.contracted_demand +
                              info.off_peak_contract.contracted_demand);

  if (ec.non_summer == 0) { // 夏月
    // 夏月尖峰時間超約瓩數＝當月尖峰時間用電最高需量－經常契約容量
    peak_exceed_contract =
        ec.peak_max_demand - info.regular_contract.contracted_demand;
    // 夏月尖峰時間超約罰款
    tmp = 0;
    charge->detail.exceed_peak_contract_fine =
        fine_calc(info.regular_contract.contracted_demand, peak_exceed_contract,
                  &tmp, info.regular_contract.demand_charge_rate);

    // 在夏月，所以絕對不會有非夏月超約罰款
    charge->detail.exceed_non_summer_contract_fine = 0;

    // 週六半尖峰時間超約罰款
    charge->detail.exceed_sat_partial_contract_fine =
        fine_calc(info.sat_partial_peak_contract.contracted_demand,
                  partial_peak_exceed_contract, &tmp,
                  info.sat_partial_peak_contract.demand_charge_rate);

    // 離峰時間超約罰款
    charge->detail.exceed_off_peak_contract_fine = fine_calc(
        info.off_peak_contract.contracted_demand, off_peak_exceed_contract,
        &tmp, info.off_peak_contract.demand_charge_rate);
  } else {
    // 不在夏月，所以絕對不會有夏月超約罰款
    charge->detail.exceed_peak_contract_fine = 0;

    // 非夏月尖峰時間超約瓩數＝當月尖峰時間用電最高需量－（經常契約容量＋非夏月契約容量）
    non_summer_exceed_contract =
        ec.non_summer_max_demand - (info.regular_contract.contracted_demand +
                                    info.non_summer_contract.contracted_demand);
    tmp = 0;
    // 非夏月尖峰時間超約罰款
    charge->detail.exceed_non_summer_contract_fine = fine_calc(
        info.non_summer_contract.contracted_demand, non_summer_exceed_contract,
        &tmp, info.non_summer_contract.demand_charge_rate);

    // 週六半尖峰時間超約罰款
    charge->detail.exceed_sat_partial_contract_fine =
        fine_calc(info.sat_partial_peak_contract.contracted_demand,
                  partial_peak_exceed_contract, &tmp,
                  info.sat_partial_peak_contract.demand_charge_rate);

    // 離峰時間超約罰款
    charge->detail.exceed_off_peak_contract_fine = fine_calc(
        info.off_peak_contract.contracted_demand, off_peak_exceed_contract,
        &tmp, info.off_peak_contract.demand_charge_rate);
  }

  // 計算功率因素獎懲
  if (ec.peak_max_demand > info.power_factor.threshold_for_counted ||
      ec.non_summer_max_demand > info.power_factor.threshold_for_counted ||
      ec.sat_partial_peak_max_demand >
          info.power_factor.threshold_for_counted ||
      ec.off_peak_max_demand > info.power_factor.threshold_for_counted) {
    charge->detail.power_factor_reward = power_factor_reward_calc(
        charge->detail.basic_charge + charge->detail.energy_charge,
        info.power_factor);
  }

  charge->total_charge = charge->detail.basic_charge +
                         charge->detail.energy_charge +
                         charge->detail.exceed_peak_contract_fine +
                         charge->detail.exceed_non_summer_contract_fine +
                         charge->detail.exceed_sat_partial_contract_fine +
                         charge->detail.exceed_off_peak_contract_fine +
                         charge->detail.power_factor_reward;

  return TAIPOWER_SUCC;
}

int tou_o2_charge_calc(struct tou_o2_charge *charge,
                       struct tou_o2_engery_consumption ec,
                       struct tou_o2_basic_info info) {
  double tmp = 0;
  double peak_exceed_contract = 0;
  double partial_peak_exceed_contract = 0;
  double sat_partial_peak_exceed_contract = 0;
  double off_peak_exceed_contract = 0;

  if (charge == NULL)
    return TAIPOWER_ERROR;

  /*
   * 僅訂週六半尖峰契約容量之用戶於週六半尖峰及離峰以外時間之用電，
   * 或僅訂離峰契約容量之用戶於離峰以外時間之用電，應就該用電最高需量及用電量，
   * 分別按相關時段單價之4.8倍計收基本電費，及1.6倍計收流動電費。
   */
  if (info.regular_contract.contracted_demand < TAIPOWER_FLOAT_TOLERANCE &&
      info.partial_peak_contract.contracted_demand < TAIPOWER_FLOAT_TOLERANCE) {
    info.regular_contract.energy_charge_rate *=
        TAIPOWER_ONLY_SAT_PARTIAL_AND_OFF_PEAK_CONTRANT_ENERGY_FINE_RATE;
    charge->detail.basic_charge +=
        info.regular_contract.demand_charge_rate * ec.peak_max_demand *
        TAIPOWER_ONLY_SAT_PARTIAL_AND_OFF_PEAK_CONTRANT_DEMAND_FINE_RATE;
    // 避免後續又再算一次超約罰款，將尖峰最大需量設定為 0
    ec.peak_max_demand = 0;

    info.partial_peak_contract.energy_charge_rate *=
        TAIPOWER_ONLY_SAT_PARTIAL_AND_OFF_PEAK_CONTRANT_ENERGY_FINE_RATE;
    charge->detail.basic_charge +=
        info.partial_peak_contract.demand_charge_rate *
        ec.partial_peak_max_demand *
        TAIPOWER_ONLY_SAT_PARTIAL_AND_OFF_PEAK_CONTRANT_DEMAND_FINE_RATE;
    // 避免後續又再算一次超約罰款，將半尖峰最大需量設定為 0
    ec.partial_peak_max_demand = 0;

    if (info.sat_partial_peak_contract.contracted_demand <
        TAIPOWER_FLOAT_TOLERANCE) {
      info.sat_partial_peak_contract.energy_charge_rate *=
          TAIPOWER_ONLY_SAT_PARTIAL_AND_OFF_PEAK_CONTRANT_ENERGY_FINE_RATE;
      charge->detail.basic_charge +=
          info.sat_partial_peak_contract.demand_charge_rate *
          ec.sat_partial_peak_max_demand *
          TAIPOWER_ONLY_SAT_PARTIAL_AND_OFF_PEAK_CONTRANT_DEMAND_FINE_RATE;
      // 避免後續又再算一次超約罰款，將週六半尖峰最大需量設定為 0
      ec.sat_partial_peak_max_demand = 0;
    }
  }

  // 開始計算基本電費
  tmp = (info.sat_partial_peak_contract.contracted_demand +
         info.off_peak_contract.contracted_demand) -
        ((info.regular_contract.contracted_demand +
          info.partial_peak_contract.contracted_demand) *
         0.5);

  charge->detail.basic_charge +=
      info.regular_contract.demand_charge_rate *
          info.regular_contract.contracted_demand +
      info.partial_peak_contract.demand_charge_rate *
          info.partial_peak_contract.contracted_demand +
      (((tmp < 0) ? 0 : tmp) * info.off_peak_contract.demand_charge_rate) +
      info.customer_charge;

  // 開始計算流動電費
  // 尖峰電費
  charge->detail.energy_charge =
      ec.peak * info.regular_contract.energy_charge_rate;
  // 半尖峰電費
  charge->detail.energy_charge +=
      ec.partial_peak * info.partial_peak_contract.energy_charge_rate;
  // 週六半尖峰電費
  charge->detail.energy_charge +=
      ec.sat_partial_peak * info.sat_partial_peak_contract.energy_charge_rate;
  // 離蜂電費
  charge->detail.energy_charge +=
      ec.off_peak * info.off_peak_contract.energy_charge_rate;

  // 開始計算超約罰款，在契約容量10%以下部分按2倍計收基本電費，超過契約容量10%部分按3倍計收基本電費
  // 尖峰時間超約瓩數＝當月尖峰時間用電最高需量－經常契約容量
  peak_exceed_contract =
      ec.peak_max_demand - info.regular_contract.contracted_demand;
  tmp = 0;
  charge->detail.exceed_peak_contract_fine =
      fine_calc(info.regular_contract.contracted_demand, peak_exceed_contract,
                &tmp, info.regular_contract.demand_charge_rate);

  // 半尖峰時間超約瓩數＝當月半尖峰時間用電最高需量－（經常契約容量＋半尖峰契約容量）
  partial_peak_exceed_contract = ec.partial_peak_max_demand -
                                 (info.regular_contract.contracted_demand +
                                  info.partial_peak_contract.contracted_demand);
  charge->detail.exceed_partial_peak_contract_fine =
      fine_calc(info.partial_peak_contract.contracted_demand,
                partial_peak_exceed_contract, &tmp,
                info.partial_peak_contract.demand_charge_rate);

  // 週六半尖峰時間超約瓩數＝當月週六半尖峰時間用電最高需量－（經常契約容量＋半尖峰契約容量＋週六半尖峰契約容量）
  sat_partial_peak_exceed_contract =
      ec.sat_partial_peak_max_demand -
      (info.regular_contract.contracted_demand +
       info.partial_peak_contract.contracted_demand +
       info.sat_partial_peak_contract.contracted_demand);
  charge->detail.exceed_sat_partial_contract_fine =
      fine_calc(info.sat_partial_peak_contract.contracted_demand,
                sat_partial_peak_exceed_contract, &tmp,
                info.sat_partial_peak_contract.demand_charge_rate);

  // 離峰時間超約瓩數＝當月離峰時間用電最高需量－（經常契約容量＋半尖峰契約容量＋週六半尖峰契約容量＋離峰契約容量）
  off_peak_exceed_contract = ec.off_peak_max_demand -
                             (info.regular_contract.contracted_demand +
                              info.partial_peak_contract.contracted_demand +
                              info.sat_partial_peak_contract.contracted_demand +
                              info.off_peak_contract.contracted_demand);
  charge->detail.exceed_off_peak_contract_fine = fine_calc(
      info.off_peak_contract.contracted_demand, off_peak_exceed_contract, &tmp,
      info.off_peak_contract.demand_charge_rate);

  // 計算功率因素獎懲
  if (ec.peak_max_demand > info.power_factor.threshold_for_counted ||
      ec.partial_peak_max_demand > info.power_factor.threshold_for_counted ||
      ec.sat_partial_peak_max_demand >
          info.power_factor.threshold_for_counted ||
      ec.off_peak_max_demand > info.power_factor.threshold_for_counted) {
    charge->detail.power_factor_reward = power_factor_reward_calc(
        charge->detail.basic_charge + charge->detail.energy_charge,
        info.power_factor);
  }

  charge->total_charge = charge->detail.basic_charge +
                         charge->detail.energy_charge +
                         charge->detail.exceed_peak_contract_fine +
                         charge->detail.exceed_partial_peak_contract_fine +
                         charge->detail.exceed_sat_partial_contract_fine +
                         charge->detail.exceed_off_peak_contract_fine +
                         charge->detail.power_factor_reward;
  ;

  return TAIPOWER_SUCC;
}

int meter_tou_b_o1_charge_calc(struct tou_o1_charge *charge,
                               struct tou_o1_engery_consumption ec,
                               struct tou_o1_basic_info info) {
  return tou_o1_charge_calc(charge, ec, info);
}

int meter_tou_b_o2_charge_calc(struct tou_o2_charge *charge,
                               struct tou_o2_engery_consumption ec,
                               struct tou_o2_basic_info info) {
  return tou_o2_charge_calc(charge, ec, info);
}

int power_tou_o1_charge_calc(struct tou_o1_charge *charge,
                             struct tou_o1_engery_consumption ec,
                             struct power_tou_o1_basic_info info) {
  // 依裝置容量計算基本費之情況
  charge->detail.basic_charge = info.capacity.customer_charge;
  charge->detail.basic_charge +=
      info.capacity.based_on_the_contracted_installed_capacity_rate *
      info.info.regular_contract.contracted_demand;
  return tou_o1_charge_calc(charge, ec, info.info);
}

int power_tou_o2_charge_calc(struct tou_o2_charge *charge,
                             struct tou_o2_engery_consumption ec,
                             struct tou_o2_basic_info info) {
  return tou_o2_charge_calc(charge, ec, info);
}

int high_voltage_tou_o1_charge_calc(struct tou_o1_charge *charge,
                                    struct tou_o1_engery_consumption ec,
                                    struct tou_o1_basic_info info) {
  // 高壓沒有 customer_charge，固定設為 0
  info.customer_charge = 0;
  return tou_o1_charge_calc(charge, ec, info);
}

int high_voltage_tou_o2_charge_calc(struct tou_o2_charge *charge,
                                    struct tou_o2_engery_consumption ec,
                                    struct tou_o2_basic_info info) {
  // 高壓沒有 customer_charge，固定設為 0
  info.customer_charge = 0;
  return tou_o2_charge_calc(charge, ec, info);
}

int extra_high_voltage_tou_o1_charge_calc(struct tou_o1_charge *charge,
                                          struct tou_o1_engery_consumption ec,
                                          struct tou_o1_basic_info info) {
  // 特高壓沒有 customer_charge，固定設為 0
  info.customer_charge = 0;
  return tou_o1_charge_calc(charge, ec, info);
}

int extra_high_voltage_tou_o2_charge_calc(struct tou_o2_charge *charge,
                                          struct tou_o2_engery_consumption ec,
                                          struct tou_o2_basic_info info) {
  // 特高壓沒有 customer_charge，固定設為 0
  info.customer_charge = 0;
  return tou_o2_charge_calc(charge, ec, info);
}

double fine_calc(double contract, double exceed_contract,
                 double *conuted_contract, double demand_charge_rate) {
  double tmp = contract * 0.1;

  if (conuted_contract == NULL)
    return TAIPOWER_ERROR;

  *conuted_contract = (*conuted_contract > 0) ? *conuted_contract : 0;
  // 在契約容量10%以下部分按2倍計收基本電費，超過契約容量10%部分按3倍計收基本電費
  if (exceed_contract > 0 && exceed_contract > *conuted_contract) {
    exceed_contract -= *conuted_contract;
    *conuted_contract += exceed_contract;
    if (tmp > exceed_contract) {
      return exceed_contract * demand_charge_rate * 2;
    } else {
      return (((tmp * 2) + ((exceed_contract - tmp) * 3)) * demand_charge_rate);
    }
  }
  return 0;
}

double power_factor_reward_calc(double pre_total_charge,
                                struct power_factor_info pf) {
  double charge = 0;
  int pf_diff = pf.power_factor - pf.reward_base_line;

  charge =
      ((pf_diff > pf.max_percentage_for_reward) ? pf.max_percentage_for_reward
                                                : pf_diff) *
      ((pf_diff > 0 ? pf.reward_per_percentage : pf.fine_per_percentage)) *
      pre_total_charge / 100;

  return -charge;
}

int is_valid_date_format(struct taipower_date *date, const char *format,
                         const char *date_str, const int length) {
  char check_symbol[] = {'c', 'y', 'M', 'd'};
  char tmp_date_str[length + 1];
  char *start_ptr = NULL;
  char *end_ptr = NULL;

  if (!date || !format || !date_str || length <= 0) {
    return TAIPOWER_ERROR;
  }

  end_ptr = memchr(format, '\0', length + 1);
  if (!end_ptr || (end_ptr - format) != length) {
    return TAIPOWER_ERROR;
  }

  end_ptr = memchr(date_str, '\0', length + 1);
  if (!end_ptr || (end_ptr - date_str) != length) {
    return TAIPOWER_ERROR;
  }

  for (int i = 0; i < length; i++) {
    if (*(date_str + i) < '0' || *(date_str + i) > '9') {
      TAIPOWER_DEBUG("date_str should be all number [%.*s]\n", length,
                     date_str);
      return TAIPOWER_ERROR;
    }
  }

  for (int i = 0; i < sizeof(check_symbol); i++) {
    start_ptr = strchr(format, check_symbol[i]);
    end_ptr = strrchr(format, check_symbol[i]);
    if (!start_ptr || !end_ptr) {
      continue;
    }

    for (int j = 1; j < (end_ptr - start_ptr - 1); j++) {
      if (*(start_ptr + j) != check_symbol[i]) {
        return TAIPOWER_ERROR;
      }
    }

    memset(tmp_date_str, 0, sizeof(tmp_date_str));
    memcpy(tmp_date_str, date_str + (start_ptr - format),
           (end_ptr - start_ptr + 1));
    if (check_symbol[i] == 'c') {
      date->year = atoi(tmp_date_str) + 1911;
    } else if (check_symbol[i] == 'y') {
      date->year = atoi(tmp_date_str);
    } else if (check_symbol[i] == 'M') {
      date->month = atoi(tmp_date_str);
      if (date->month > 12 || date->month < 1) {
        return TAIPOWER_ERROR;
      }
    } else if (check_symbol[i] == 'd') {
      date->day = atoi(tmp_date_str);
      if (date->day < 1) {
        return TAIPOWER_ERROR;
      }
    }
  }

  if (date->month == 2) {
    if (date->day > (IS_LEAP_YEAR(date->year) ? 29 : 28)) {
      return TAIPOWER_ERROR;
    }
  } else {
    if (date->day > days_per_month[date->month]) {
      return TAIPOWER_ERROR;
    }
  }

  return TAIPOWER_SUCC;
}

int diff_days(struct taipower_date date1, struct taipower_date date2) {
  int loop_index = 0;
  int diff_days = 0;
  int is_negtive = 1;

  if (date1.year < date2.year) {
    struct taipower_date tmp = date1;
    date1 = date2;
    date2 = tmp;
    is_negtive = -1;
  }

  for (loop_index = date2.year; loop_index < date1.year; loop_index++) {
    diff_days += IS_LEAP_YEAR(loop_index) ? 366 : 365;
  }

  for (loop_index = 1; loop_index < date2.month; loop_index++) {
    diff_days -= days_per_month[loop_index];
  }
  if (date2.month > 2 && IS_LEAP_YEAR(date2.year)) {
    diff_days--;
  }
  diff_days -= date2.day;

  for (loop_index = 1; loop_index < date1.month; loop_index++) {
    diff_days += days_per_month[loop_index];
  }
  if (date1.month > 2 && IS_LEAP_YEAR(date1.year)) {
    diff_days++;
  }

  diff_days += date1.day;

  return (diff_days * is_negtive);
}