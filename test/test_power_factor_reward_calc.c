#include "taipower.h"
#include <assert.h>

int test_power_factor_reward_calc() {
  power_factor_info pf = {.fine_per_percentage = 0.1,
                          .max_percentage_for_reward = 15,
                          .power_factor = 100,
                          .reward_base_line = 15,
                          .reward_per_percentage = 0.1,
                          .threshold_for_counted = 20};
  double charge = power_factor_reward_calc(80112 + 309244.8, pf);

  TAIPOWER_DEBUG("power factor discount:[%lf]\n", charge);
  assert((charge - (-5840.35)) < TAIPOWER_FLOAT_TOLERANCE &&
         (charge - (-5840.35)) > TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  return TAIPOWER_SUCC;
}