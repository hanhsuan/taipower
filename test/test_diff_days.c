#include "taipower.h"
#include <assert.h>

int test_diff_days() {
  struct taipower_date date1 = {.year = 2022, .month = 10, .day = 7};
  struct taipower_date date2 = {.year = 2022, .month = 10, .day = 1};

  assert(diff_days(date1, date2) == 6);
  assert(diff_days(date2, date1) == -6);

  date2.year = 2021;

  assert(diff_days(date1, date2) == 371);
  assert(diff_days(date2, date1) == -371);

  date2.year = 2016;

  assert(diff_days(date1, date2) == 2197);
  assert(diff_days(date2, date1) == -2197);

  return TAIPOWER_SUCC;
}
