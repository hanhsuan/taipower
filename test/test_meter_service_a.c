#include "taipower.h"
#include <assert.h>

/**
 * @brief 測試表燈(住商)簡易時間電價
 */
int test_meter_service_a() {
  struct power_factor_info pf = {.threshold_for_counted =
                                     TAIPOWER_DONT_COUNT_POWER_FACTOR};
  struct meter_tou_a_o1_engery_consumption kwhs_for_2_level = {
      .peak = 1200,
      .off_peak = 500,
      .sun_sat_off_peak = 500,
  };

  struct meter_tou_a_o1_basic_info summer_info_for_2_level = {
      .customer_charge = 75,
      .portion_of_usage_limit = 2000,
      .peak_rate = 4.44,
      .off_peak_rate = 1.80,
      .sun_sat_off_peak = 1.80,
      .exceed_limit_rate = 0.96,
      .power_factor = pf};

  struct meter_tou_a_o2_engery_consumption kwhs_for_3_level = {
      .peak = 200,
      .partial_peak = 1000,
      .off_peak = 500,
      .sun_sat_off_peak = 500,
  };

  struct meter_tou_a_o2_basic_info summer_info_for_3_level = {
      .customer_charge = 75,
      .portion_of_usage_limit = 2000,
      .peak_rate = 6.20,
      .partial_peak_rate = 4.07,
      .off_peak_rate = 1.80,
      .sun_sat_off_peak = 1.80,
      .exceed_limit_rate = 0.96,
      .power_factor = pf};

  assert(
      (meter_tou_a_o1_charge_calc(kwhs_for_2_level, summer_info_for_2_level) -
       7395) < TAIPOWER_FLOAT_TOLERANCE);
  assert(
      (meter_tou_a_o2_charge_calc(kwhs_for_3_level, summer_info_for_3_level) -
       7377) < TAIPOWER_FLOAT_TOLERANCE);

  return 0;
}