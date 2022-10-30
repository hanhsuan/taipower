#include "taipower.h"
#include <assert.h>

/**
 * @brief 測試特高壓三段式時間電價
 * 測試條件
 * 特高壓電力用戶：經常契約容量10,000瓩，8月尖峰時間用電100,000度、半尖峰時間150,000度、
 * 週六半尖峰時間50,000度、離峰時間200,000度，倘最高需量未超過契約容量，
 * 當月應計電費為
 * 217.30元×10,000瓩＋4.38元×100,000度+2.74元×150,000度+1.73元×50,000度+1.24元×200,000度
 * ＝3,356,500元
 *
 * 特高壓電力用戶：經常契約容量10,000瓩，8月指定尖峰時間用電30,000度、半尖峰時間220,000度、
 * 週六半尖峰時間50,000度、離峰時間200,000度，倘最高需量未超過契約容量，
 * 當月應計電費為
 * 217.30元×10,000瓩＋7.44元×30,000度+2.74元×220,000度+1.73元×50,000度+1.24元×200,000度
 * ＝3,333,500元
 */
int test_extra_high_voltage_tou_o2() {
  struct tou_o2_charge charge = {0};
  struct tou_o2_engery_consumption ec = {.peak = 100000,
                                         .partial_peak = 150000,
                                         .sat_partial_peak = 50000,
                                         .off_peak = 200000};
  struct tou_o2_basic_info info = {
      .customer_charge = 0,
      .regular_contract.contracted_demand = 10000,
      .regular_contract.demand_charge_rate = 217.3,
      .regular_contract.energy_charge_rate = 4.38,
      .partial_peak_contract.energy_charge_rate = 2.74,
      .sat_partial_peak_contract.energy_charge_rate = 1.73,
      .off_peak_contract.energy_charge_rate = 1.24};

  struct tou_o2_charge charge2 = {0};
  struct tou_o2_engery_consumption ec2 = {.peak = 30000,
                                          .partial_peak = 220000,
                                          .sat_partial_peak = 50000,
                                          .off_peak = 200000};
  struct tou_o2_basic_info info2 = {
      .customer_charge = 0,
      .regular_contract.contracted_demand = 10000,
      .regular_contract.demand_charge_rate = 217.3,
      .regular_contract.energy_charge_rate = 7.44,
      .partial_peak_contract.energy_charge_rate = 2.74,
      .sat_partial_peak_contract.energy_charge_rate = 1.73,
      .off_peak_contract.energy_charge_rate = 1.24};

  if (TAIPOWER_SUCC !=
          extra_high_voltage_tou_o2_charge_calc(&charge, ec, info) ||
      TAIPOWER_SUCC !=
          extra_high_voltage_tou_o2_charge_calc(&charge2, ec2, info2)) {
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

  assert((charge.total_charge - 3356500) < TAIPOWER_FLOAT_TOLERANCE &&
         (charge.total_charge - 3356500) > TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((charge.detail.basic_charge - 2173000) < TAIPOWER_FLOAT_TOLERANCE &&
         (charge.detail.basic_charge - 2173000) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((charge.detail.energy_charge - 1183500) < TAIPOWER_FLOAT_TOLERANCE &&
         (charge.detail.energy_charge - 1183500) >
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

  assert((charge2.total_charge - 3333500) < TAIPOWER_FLOAT_TOLERANCE &&
         (charge2.total_charge - 3333500) > TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((charge2.detail.basic_charge - 2173000) < TAIPOWER_FLOAT_TOLERANCE &&
         (charge2.detail.basic_charge - 2173000) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((charge2.detail.energy_charge - 1160500) < TAIPOWER_FLOAT_TOLERANCE &&
         (charge2.detail.energy_charge - 1160500) >
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