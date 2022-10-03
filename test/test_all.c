#include "taipower.h"

int test_residential();
int test_commercial();
int test_meter_service_a();
int test_tou_core();
int test_meter_tou_b_o1();
int test_power_tou_o1();
int test_high_voltage_tou_o1();
int test_high_voltage_tou_o2();
int test_extra_high_voltage_tou_o1();
int test_extra_high_voltage_tou_o2();
int test_power_factor_reward_calc();

int main() {

  test_residential();
  test_commercial();
  test_meter_service_a();
  test_tou_core();
  test_meter_tou_b_o1();
  test_power_tou_o1();
  test_high_voltage_tou_o1();
  test_high_voltage_tou_o2();
  test_extra_high_voltage_tou_o1();
  test_extra_high_voltage_tou_o2();
  test_power_factor_reward_calc();

  return 0;
}