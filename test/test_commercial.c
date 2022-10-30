#include "taipower.h"
#include <assert.h>

/**
 * @brief 測試表燈(住商)非時間電價-商用
 */
int test_commercial() {
  double kwh;

  struct meter_ntou_rate summer_levels[] = {
      {.level = 330, .rate = 2.53},
      {.level = 700, .rate = 3.55},
      {.level = 1500, .rate = 4.25},
      {.level = TAIPOWER_EXCEED_KWH, .rate = 6.43}};

  struct meter_ntou_rate non_summer_levels[] = {
      {.level = 330, .rate = 2.12},
      {.level = 700, .rate = 2.91},
      {.level = 1500, .rate = 3.44},
      {.level = TAIPOWER_EXCEED_KWH, .rate = 5.05}};

  struct power_factor_info pf = {.threshold_for_counted =
                                     TAIPOWER_DONT_COUNT_POWER_FACTOR};

  struct meter_ntou_basic_info summer_info = {
      .level_count = sizeof(summer_levels) / sizeof(struct meter_ntou_rate),
      .levels = summer_levels,
      .power_factor = pf};

  struct meter_ntou_basic_info non_summer_info = {
      .level_count = sizeof(non_summer_levels) / sizeof(struct meter_ntou_rate),
      .levels = non_summer_levels,
      .power_factor = pf};

  kwh = 100;

  assert((meter_ntou_charge_calc(kwh, summer_info) - 253) <
         TAIPOWER_FLOAT_TOLERANCE);

  assert((meter_ntou_charge_calc(kwh, non_summer_info) - 212) <
         TAIPOWER_FLOAT_TOLERANCE);

  kwh = 350;

  assert((meter_ntou_charge_calc(kwh, summer_info) - 905.9) <
         TAIPOWER_FLOAT_TOLERANCE);

  assert((meter_ntou_charge_calc(kwh, non_summer_info) - 757.8) <
         TAIPOWER_FLOAT_TOLERANCE);

  kwh = 750;

  assert((meter_ntou_charge_calc(kwh, summer_info) - 2360.9) <
         TAIPOWER_FLOAT_TOLERANCE);

  assert((meter_ntou_charge_calc(kwh, non_summer_info) - 1948.3) <
         TAIPOWER_FLOAT_TOLERANCE);

  kwh = 1750;

  assert((meter_ntou_charge_calc(kwh, summer_info) - 7155.9) <
         TAIPOWER_FLOAT_TOLERANCE);

  assert((meter_ntou_charge_calc(kwh, non_summer_info) - 5790.8) <
         TAIPOWER_FLOAT_TOLERANCE);

  return 0;
}