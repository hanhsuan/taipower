#include "taipower.h"

int test_residential();
int test_commercial();
int test_service_a();
int test_time_of_use_option_core();
int test_time_of_use_b_option_1();
int test_low_voltage_time_of_use_option_1();
int test_high_voltage_time_of_use_option_1();
int test_high_voltage_time_of_use_option_2();
int test_extra_high_voltage_time_of_use_option_1();
int test_extra_high_voltage_time_of_use_option_2();
int test_power_factor_adjustment_calc();
int test_diff_days();
int test_is_leap_year();
int test_is_valid_date_format();

int main() {

  test_residential();
  test_commercial();
  test_service_a();
  test_time_of_use_option_core();
  test_time_of_use_b_option_1();
  test_low_voltage_time_of_use_option_1();
  test_high_voltage_time_of_use_option_1();
  test_high_voltage_time_of_use_option_2();
  test_extra_high_voltage_time_of_use_option_1();
  test_extra_high_voltage_time_of_use_option_2();
  test_power_factor_adjustment_calc();
  test_diff_days();
  test_is_leap_year();
  test_is_valid_date_format();

  return 0;
}