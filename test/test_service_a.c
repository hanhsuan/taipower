#include "taipower.h"
#include <assert.h>

/**
 * @brief 測試表燈(住商)簡易時間電價
 */
int test_service_a() {
  power_factor_info pf = {.min_demand_threshold =
                                     TAIPOWER_DONT_COUNT_POWER_FACTOR};
  time_of_use_a_option_1_energy_consumption kwhs_for_2_level = {
      .peak = 1200,
      .off_peak = 500,
      .weekend_off_peak = 500,
  };

  time_of_use_a_option_1_basic_info summer_info_for_2_level = {
      .customer_charge = 75,
      .usage_limit_kwh = 2000,
      .peak_rate = 4.44,
      .off_peak_rate = 1.80,
      .weekend_off_peak = 1.80,
      .exceed_limit_rate = 0.96,
      .power_factor = pf};

  time_of_use_a_option_2_energy_consumption kwhs_for_3_level = {
      .peak = 200,
      .partial_peak = 1000,
      .off_peak = 500,
      .weekend_off_peak = 500,
  };

  time_of_use_a_option_2_basic_info summer_info_for_3_level = {
      .customer_charge = 75,
      .usage_limit_kwh = 2000,
      .peak_rate = 6.20,
      .partial_peak_rate = 4.07,
      .off_peak_rate = 1.80,
      .weekend_off_peak = 1.80,
      .exceed_limit_rate = 0.96,
      .power_factor = pf};

  assert(
      (time_of_use_a_option_1_charge_calc(kwhs_for_2_level, summer_info_for_2_level) -
       7395) < TAIPOWER_FLOAT_TOLERANCE);
  assert(
      (time_of_use_a_option_2_charge_calc(kwhs_for_3_level, summer_info_for_3_level) -
       7377) < TAIPOWER_FLOAT_TOLERANCE);

  _unused(summer_info_for_2_level);
  _unused(summer_info_for_3_level);
  _unused(kwhs_for_2_level);
  _unused(kwhs_for_3_level);
  return 0;
}