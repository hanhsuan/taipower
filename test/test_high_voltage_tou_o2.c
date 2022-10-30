#include "taipower.h"
#include <assert.h>

/**
 * @brief 測試高壓三段式時間電價
 * 測試條件
 * 高壓電力用戶：經常契約容量500瓩，8月尖峰時間用電10,000度、
 * 半尖峰時間15,000度、週六半尖峰時間5,000度、離峰時間20,000度，倘最高需量未超過契約容量，
 * 當月應計電費為
 * 223.60元×500瓩＋4.41元×10,000度+2.76元×15,000度+1.78元×5,000度+1.26元×20,000度＝231,400元
 *
 * 高壓電力用戶：經常契約容量500瓩，8月指定尖峰時間用電3,000度、
 * 半尖峰時間22,000度、週六半尖峰時間5,000度、離峰時間20,000度，倘最高需量未超過契約容量，
 * 當月應計電費為
 * 223.60元×500瓩＋7.49元×3,000度+2.76元×22,000度+1.78元×5,000度+1.26元×20,000度＝229,090元
 */
int test_high_voltage_tou_o2() {
  struct tou_o2_charge charge = {0};
  struct tou_o2_engery_consumption ec = {.peak = 10000,
                                         .partial_peak = 15000,
                                         .sat_partial_peak = 5000,
                                         .off_peak = 20000};
  struct tou_o2_basic_info info = {
      .customer_charge = 0,
      .regular_contract.contracted_demand = 500,
      .regular_contract.demand_charge_rate = 223.6,
      .regular_contract.energy_charge_rate = 4.41,
      .partial_peak_contract.energy_charge_rate = 2.76,
      .sat_partial_peak_contract.energy_charge_rate = 1.78,
      .off_peak_contract.energy_charge_rate = 1.26};

  struct tou_o2_charge charge2 = {0};
  struct tou_o2_engery_consumption ec2 = {.peak = 3000,
                                          .partial_peak = 22000,
                                          .sat_partial_peak = 5000,
                                          .off_peak = 20000};
  struct tou_o2_basic_info info2 = {
      .customer_charge = 0,
      .regular_contract.contracted_demand = 500,
      .regular_contract.demand_charge_rate = 223.6,
      .regular_contract.energy_charge_rate = 7.49,
      .partial_peak_contract.energy_charge_rate = 2.76,
      .sat_partial_peak_contract.energy_charge_rate = 1.78,
      .off_peak_contract.energy_charge_rate = 1.26};

  if (TAIPOWER_SUCC != high_voltage_tou_o2_charge_calc(&charge, ec, info) ||
      TAIPOWER_SUCC != high_voltage_tou_o2_charge_calc(&charge2, ec2, info2)) {
    return TAIPOWER_ERROR;
  }

  TAIPOWER_DEBUG("total charge:[%lf]\n", charge.total_charge);
  TAIPOWER_DEBUG("detail.basic_charge:[%lf]\n", charge.detail.basic_charge);
  TAIPOWER_DEBUG("detail.energy_charge:[%lf]\n", charge.detail.energy_charge);
  TAIPOWER_DEBUG("detail.exceed_peak_contract_fine:[%lf]\n",
                 charge.detail.exceed_peak_contract_fine);
  TAIPOWER_DEBUG("detail.exceed_partial_peak_contract_fine:[%lf]\n",
                 charge.detail.exceed_partial_peak_contract_fine);
  TAIPOWER_DEBUG("detail.exceed_sat_partial_contract_fine:[%lf]\n",
                 charge.detail.exceed_sat_partial_contract_fine);
  TAIPOWER_DEBUG("detail.exceed_off_peak_contract_fine:[%lf]\n",
                 charge.detail.exceed_off_peak_contract_fine);

  assert((charge.total_charge - 231400) < TAIPOWER_FLOAT_TOLERANCE &&
         (charge.total_charge - 231400) > TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((charge.detail.basic_charge - 111800) < TAIPOWER_FLOAT_TOLERANCE &&
         (charge.detail.basic_charge - 111800) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((charge.detail.energy_charge - 119600) < TAIPOWER_FLOAT_TOLERANCE &&
         (charge.detail.energy_charge - 119600) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((charge.detail.exceed_peak_contract_fine - 0) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (charge.detail.exceed_peak_contract_fine - 0) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((charge.detail.exceed_partial_peak_contract_fine - 0) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (charge.detail.exceed_partial_peak_contract_fine - 0) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((charge.detail.exceed_sat_partial_contract_fine - 0) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (charge.detail.exceed_sat_partial_contract_fine - 0) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((charge.detail.exceed_off_peak_contract_fine - 0) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (charge.detail.exceed_off_peak_contract_fine - 0) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  TAIPOWER_DEBUG("total charge:[%lf]\n", charge2.total_charge);
  TAIPOWER_DEBUG("detail.basic_charge:[%lf]\n", charge2.detail.basic_charge);
  TAIPOWER_DEBUG("detail.energy_charge:[%lf]\n", charge2.detail.energy_charge);
  TAIPOWER_DEBUG("detail.exceed_peak_contract_fine:[%lf]\n",
                 charge2.detail.exceed_peak_contract_fine);
  TAIPOWER_DEBUG("detail.exceed_partial_peak_contract_fine:[%lf]\n",
                 charge2.detail.exceed_partial_peak_contract_fine);
  TAIPOWER_DEBUG("detail.exceed_sat_partial_contract_fine:[%lf]\n",
                 charge2.detail.exceed_sat_partial_contract_fine);
  TAIPOWER_DEBUG("detail.exceed_off_peak_contract_fine:[%lf]\n",
                 charge2.detail.exceed_off_peak_contract_fine);

  assert((charge2.total_charge - 229090) < TAIPOWER_FLOAT_TOLERANCE &&
         (charge2.total_charge - 229090) > TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((charge2.detail.basic_charge - 111800) < TAIPOWER_FLOAT_TOLERANCE &&
         (charge2.detail.basic_charge - 111800) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((charge2.detail.energy_charge - 117290) < TAIPOWER_FLOAT_TOLERANCE &&
         (charge2.detail.energy_charge - 117290) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((charge2.detail.exceed_peak_contract_fine - 0) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (charge2.detail.exceed_peak_contract_fine - 0) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((charge2.detail.exceed_partial_peak_contract_fine - 0) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (charge2.detail.exceed_partial_peak_contract_fine - 0) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((charge2.detail.exceed_sat_partial_contract_fine - 0) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (charge2.detail.exceed_sat_partial_contract_fine - 0) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((charge2.detail.exceed_off_peak_contract_fine - 0) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (charge2.detail.exceed_off_peak_contract_fine - 0) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  return TAIPOWER_SUCC;
}