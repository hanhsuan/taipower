#include "taipower.h"
#include <assert.h>

/**
 * @brief 測試表燈(住商)非時間電價-非商用
 */
int test_residential() {
  double kwh;

  meter_ntou_rate summer_levels[] = {
      {.level = 120, .rate = 1.63},
      {.level = 330, .rate = 2.38},
      {.level = 500, .rate = 3.52},
      {.level = 700, .rate = 4.80},
      {.level = 1000, .rate = 5.66},
      {.level = TAIPOWER_EXCEED_KWH, .rate = 6.99}};

  meter_ntou_rate non_summer_levels[] = {
      {.level = 120, .rate = 1.63},
      {.level = 330, .rate = 2.10},
      {.level = 500, .rate = 2.89},
      {.level = 700, .rate = 3.94},
      {.level = 1000, .rate = 4.60},
      {.level = TAIPOWER_EXCEED_KWH, .rate = 5.48}};

  power_factor_info pf = {.threshold_for_counted =
                              TAIPOWER_DONT_COUNT_POWER_FACTOR};

  meter_ntou_basic_info summer_info = {.level_count = sizeof(summer_levels) /
                                                      sizeof(meter_ntou_rate),
                                       .levels = summer_levels,
                                       .power_factor = pf};
  meter_ntou_basic_info non_summer_info = {
      .level_count = sizeof(non_summer_levels) / sizeof(meter_ntou_rate),
      .levels = non_summer_levels,
      .power_factor = pf};

  kwh = 100;

  assert((meter_ntou_charge_calc(kwh, summer_info) - 163) <
         TAIPOWER_FLOAT_TOLERANCE);

  assert((meter_ntou_charge_calc(kwh, non_summer_info) - 163) <
         TAIPOWER_FLOAT_TOLERANCE);

  kwh = 200;

  assert((meter_ntou_charge_calc(kwh, summer_info) - 386) <
         TAIPOWER_FLOAT_TOLERANCE);

  assert((meter_ntou_charge_calc(kwh, non_summer_info) - 363.6) <
         TAIPOWER_FLOAT_TOLERANCE);

  kwh = 350;

  assert((meter_ntou_charge_calc(kwh, summer_info) - 765.8) <
         TAIPOWER_FLOAT_TOLERANCE);

  assert((meter_ntou_charge_calc(kwh, non_summer_info) - 694.4) <
         TAIPOWER_FLOAT_TOLERANCE);

  kwh = 550;

  assert((meter_ntou_charge_calc(kwh, summer_info) - 1533.8) <
         TAIPOWER_FLOAT_TOLERANCE);

  assert((meter_ntou_charge_calc(kwh, non_summer_info) - 1324.9) <
         TAIPOWER_FLOAT_TOLERANCE);

  kwh = 750;

  assert((meter_ntou_charge_calc(kwh, summer_info) - 2537.3) <
         TAIPOWER_FLOAT_TOLERANCE);

  assert((meter_ntou_charge_calc(kwh, non_summer_info) - 2146.4) <
         TAIPOWER_FLOAT_TOLERANCE);

  kwh = 1750;

  assert((meter_ntou_charge_calc(kwh, summer_info) - 9194.8) <
         TAIPOWER_FLOAT_TOLERANCE);

  assert((meter_ntou_charge_calc(kwh, non_summer_info) - 7406.4) <
         TAIPOWER_FLOAT_TOLERANCE);

  return 0;
}