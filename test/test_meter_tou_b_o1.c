#include "taipower.h"
#include <assert.h>

/**
 * @brief 測試表燈(住商)標準時間電價
 * 測試條件
 * 表燈時間電價用戶三相供電，經常契約容量20瓩，8月尖峰時間用電2,000度、
 * 週六半尖峰時間500度、離峰時間1,500度，倘最高需量未超過契約容量，當月應計電費為：
 * 用戶費 262.50元 ＋基本電費 236.20元×20瓩
 * ＋流動電費 3.24元×2,000度+2.14元×500度+1.39元×1,500度 ＝總電費 14,621.5元
 */
int test_meter_tou_b_o1() {
  struct tou_o1_charge charge = {0};
  struct tou_o1_engery_consumption ec = {
      .peak = 2000, .sat_partial_peak = 500, .off_peak = 1500};
  struct tou_o1_basic_info info = {
      .customer_charge = 262.50,
      .regular_contract.contracted_demand = 20,
      .regular_contract.demand_charge_rate = 236.2,
      .regular_contract.energy_charge_rate = 3.24,
      .sat_partial_peak_contract.contracted_demand = 0,
      .sat_partial_peak_contract.demand_charge_rate = 47.2,
      .sat_partial_peak_contract.energy_charge_rate = 2.14,
      .off_peak_contract.contracted_demand = 0,
      .off_peak_contract.demand_charge_rate = 47.2,
      .off_peak_contract.energy_charge_rate = 1.39};
  if (TAIPOWER_SUCC != meter_tou_b_o1_charge_calc(&charge, ec, info)) {
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

  assert((charge.total_charge - 14621.5) < TAIPOWER_FLOAT_TOLERANCE &&
         (charge.total_charge - 14621.5) > -0.01);

  assert((charge.detail.basic_charge - 4986.5) < TAIPOWER_FLOAT_TOLERANCE &&
         (charge.detail.basic_charge - 4986.5) > -0.01);

  assert((charge.detail.energy_charge - 9635) < TAIPOWER_FLOAT_TOLERANCE &&
         (charge.detail.energy_charge - 9635) > -0.01);

  assert((charge.detail.exceed_peak_contract_fine - 0) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (charge.detail.exceed_peak_contract_fine - 0) > -0.01);

  assert((charge.detail.exceed_non_summer_contract_fine - 0) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (charge.detail.exceed_non_summer_contract_fine - 0) > -0.01);

  assert((charge.detail.exceed_sat_partial_contract_fine - 0) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (charge.detail.exceed_sat_partial_contract_fine - 0) > -0.01);

  assert((charge.detail.exceed_off_peak_contract_fine - 0) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (charge.detail.exceed_off_peak_contract_fine - 0) > -0.01);

  return TAIPOWER_SUCC;
}