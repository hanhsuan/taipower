#include "taipower.h"
#include <assert.h>

int test_power_factor_adjustment_calc() {
  power_factor_info pf = {.surcharge_per_percentage = 0.1,
                                 .max_adjustment_percentage = 15,
                                 .power_factor = 100,
                                 .baseline_percentage = 15,
                                 .discount_per_percentage = 0.1,
                                 .min_demand_threshold = 20};
  double charge = power_factor_adjustment_calc(80112 + 309244.8, pf);

  TAIPOWER_DEBUG("power factor discount:[%lf]\n", charge);
  assert((charge - (-5840.35)) < TAIPOWER_FLOAT_TOLERANCE &&
         (charge - (-5840.35)) > TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  _unused(charge);
  return TAIPOWER_SUCC;
}