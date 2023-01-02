#include "taipower.h"
#include <assert.h>

int test_is_valid_date_format() {
  struct taipower_date date;

  assert(is_valid_date_format(NULL, "ycyyMMdd", "20221007", 8) ==
         TAIPOWER_ERROR);

  assert(is_valid_date_format(&date, NULL, "20221007", 8) == TAIPOWER_ERROR);

  assert(is_valid_date_format(&date, "ycyyMMdd", NULL, 8) == TAIPOWER_ERROR);

  assert(is_valid_date_format(&date, "ycyyMMdd", "20221007", 8) ==
         TAIPOWER_ERROR);

  assert(is_valid_date_format(&date, "ycyyMMdd", "2022a007", 8) ==
         TAIPOWER_ERROR);

  assert(is_valid_date_format(&date, "yyyyMMdd", "2022", 8) == TAIPOWER_ERROR);

  assert(is_valid_date_format(&date, "MMdd", "20221007", 8) == TAIPOWER_ERROR);

  assert(is_valid_date_format(&date, "yyyyMMdd", "20221007", 8) ==
             TAIPOWER_SUCC &&
         date.year == 2022 && date.month == 10 && date.day == 7);

  assert(is_valid_date_format(&date, "yyyyMMdd", "20221031", 8) ==
             TAIPOWER_SUCC &&
         date.year == 2022 && date.month == 10 && date.day == 31);

  assert(is_valid_date_format(&date, "yyyyMMdd", "20220228", 8) ==
             TAIPOWER_SUCC &&
         date.year == 2022 && date.month == 2 && date.day == 28);

  assert(is_valid_date_format(&date, "yyyyMMdd", "20220229", 8) ==
         TAIPOWER_ERROR);

  assert(is_valid_date_format(&date, "yyyyMMdd", "20160229", 8) ==
             TAIPOWER_SUCC &&
         date.year == 2016 && date.month == 2 && date.day == 29);

  assert(is_valid_date_format(&date, "yyyyMMdd", "20160230", 8) ==
         TAIPOWER_ERROR);

  assert(is_valid_date_format(&date, "MMddyyyy", "02292016", 8) ==
             TAIPOWER_SUCC &&
         date.year == 2016 && date.month == 2 && date.day == 29);

  assert(is_valid_date_format(&date, "MMddccc", "0229105", 7) ==
             TAIPOWER_SUCC &&
         date.year == 2016 && date.month == 2 && date.day == 29);

  assert(is_valid_date_format(&date, "MMcccdd", "0210529", 7) ==
             TAIPOWER_SUCC &&
         date.year == 2016 && date.month == 2 && date.day == 29);

  _unused(date);
  return TAIPOWER_SUCC;
}
