#include "taipower.h"
#include <assert.h>

/**
 * @brief 測試特高壓兩段式時間電價
 * 測試條件
 * 特高壓電力用戶：經常契約容量10,000瓩，8月尖峰時間用電160,000度、週六半尖峰時間50,000度、
 * 離峰時間130,000度，倘最高需量未超過契約容量，當月應計電費為
 * 217.30元×10,000瓩＋3.11元×160,000度+1.95元×50,000度+1.32元×130,000度＝2,939,700元
 */
int test_extra_high_voltage_tou_o1() {
  struct tou_o1_charge charge = {0};
  struct tou_o1_engery_consumption ec = {
      .peak = 160000, .sat_partial_peak = 50000, .off_peak = 130000};
  struct tou_o1_basic_info info = {
      .customer_charge = 0,
      .regular_contract.contracted_demand = 10000,
      .regular_contract.demand_charge_rate = 217.3,
      .regular_contract.energy_charge_rate = 3.11,
      .sat_partial_peak_contract.energy_charge_rate = 1.95,
      .off_peak_contract.energy_charge_rate = 1.32};

  if (TAIPOWER_SUCC !=
      extra_high_voltage_tou_o1_charge_calc(&charge, ec, info)) {
    return TAIPOWER_ERROR;
  }

  TAIPOWER_DEBUG("total charge:[%lf]\n", charge.total_charge);
  TAIPOWER_DEBUG("detail.basic_charge:[%lf]\n", charge.detail.basic_charge);
  TAIPOWER_DEBUG("detail.energy_charge:[%lf]\n", charge.detail.energy_charge);
  TAIPOWER_DEBUG("detail.exceed_peak_contract_fine:[%lf]\n",
                 charge.detail.exceed_peak_contract_fine);
  TAIPOWER_DEBUG("detail.exceed_non_summer_contract_fine:[%lf]\n",
                 charge.detail.exceed_non_summer_contract_fine);
  TAIPOWER_DEBUG("detail.exceed_sat_partial_contract_fine:[%lf]\n",
                 charge.detail.exceed_sat_partial_contract_fine);
  TAIPOWER_DEBUG("detail.exceed_off_peak_contract_fine:[%lf]\n",
                 charge.detail.exceed_off_peak_contract_fine);

  assert((charge.total_charge - 2939700) < TAIPOWER_FLOAT_TOLERANCE &&
         (charge.total_charge - 2939700) > TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((charge.detail.basic_charge - 2173000) < TAIPOWER_FLOAT_TOLERANCE &&
         (charge.detail.basic_charge - 2173000) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((charge.detail.energy_charge - 766700) < TAIPOWER_FLOAT_TOLERANCE &&
         (charge.detail.energy_charge - 766700) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((charge.detail.exceed_peak_contract_fine - 0) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (charge.detail.exceed_peak_contract_fine - 0) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((charge.detail.exceed_non_summer_contract_fine - 0) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (charge.detail.exceed_non_summer_contract_fine - 0) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((charge.detail.exceed_sat_partial_contract_fine - 0) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (charge.detail.exceed_sat_partial_contract_fine - 0) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((charge.detail.exceed_off_peak_contract_fine - 0) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (charge.detail.exceed_off_peak_contract_fine - 0) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  return TAIPOWER_SUCC;
}