#include "taipower.h"
#include <assert.h>

/**
 * @brief 測試高壓兩段式時間電價
 * 測試條件
 * 高壓電力用戶：經常契約容量150瓩，8月尖峰時間用電15,600度、週六半尖峰時間5,000度、
 * 離峰時間13,000度，倘最高需量未超過契約容量，當月應計電費為
 * 223.60元×150瓩＋3.13元×15,600度+1.97元×5,000度+1.35元×13,000度＝109,768元
 */
int test_high_voltage_tou_o1() {
  struct tou_o1_charge charge = {0};
  struct tou_o1_engery_consumption ec = {
      .peak = 15600, .sat_partial_peak = 5000, .off_peak = 13000};
  struct tou_o1_basic_info info = {
      .customer_charge = 0,
      .regular_contract.contracted_demand = 150,
      .regular_contract.demand_charge_rate = 223.6,
      .regular_contract.energy_charge_rate = 3.13,
      .sat_partial_peak_contract.energy_charge_rate = 1.97,
      .off_peak_contract.energy_charge_rate = 1.35};

  if (TAIPOWER_SUCC != high_voltage_tou_o1_charge_calc(&charge, ec, info)) {
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

  assert((charge.total_charge - 109768) < TAIPOWER_FLOAT_TOLERANCE &&
         (charge.total_charge - 109768) > TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((charge.detail.basic_charge - 33540) < TAIPOWER_FLOAT_TOLERANCE &&
         (charge.detail.basic_charge - 33540) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((charge.detail.energy_charge - 76228) < TAIPOWER_FLOAT_TOLERANCE &&
         (charge.detail.energy_charge - 76228) >
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