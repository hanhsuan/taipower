#include "taipower.h"
#include <assert.h>

int test_is_leap_year() {

  assert(IS_LEAP_YEAR(1980));
  assert(IS_LEAP_YEAR(2000));
  assert(IS_LEAP_YEAR(2020));
  assert(!IS_LEAP_YEAR(2021));
  assert(!IS_LEAP_YEAR(2022));

  return 0;
}