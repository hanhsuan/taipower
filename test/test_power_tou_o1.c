#include "taipower.h"
#include <assert.h>

/**
 * @brief 測試低壓兩段式時間電價
 * 測試條件
 * 裝置用戶：契約容量30瓩，8月尖峰時間用電2,000度、週六半尖峰時間1,000度、離峰時間1,500度，當月應計電費為用戶費
 * 105.00元＋基本電費137.50元×30瓩＋流動電費3.24元×2,000度+2.14元×1,000度+1.39元×1,500度＝總電費
 * 14,935元
 * 需量用戶：經常契約容量95瓩，8月尖峰時間用電6,000度、週六半尖峰時間1,000度、離峰時間4,000度，倘最高需量未超過契約容量，當月應計電費為用戶費
 * 262.50元＋基本電費236.20元×95瓩＋流動電費3.24元×6,000度+2.14元×1,000度+1.39元×4,000度＝總電費
 * 49,841.5元
 */
int test_power_tou_o1() {
  tou_o1_charge device_charge = {0};
  tou_o1_engery_consumption device_ec = {
      .peak = 2000, .sat_partial_peak = 1000, .off_peak = 1500};
  power_tou_o1_basic_info device_info = {
      .capacity.customer_charge = 105,
      .capacity.based_on_the_contracted_installed_capacity_rate = 137.5,
      .info.customer_charge = 0,
      .info.regular_contract.contracted_demand = 30,
      .info.regular_contract.energy_charge_rate = 3.24,
      .info.sat_partial_peak_contract.energy_charge_rate = 2.14,
      .info.off_peak_contract.energy_charge_rate = 1.39};
  tou_o1_charge demand_charge = {0};
  tou_o1_engery_consumption demand_ec = {
      .peak = 6000, .sat_partial_peak = 1000, .off_peak = 4000};
  power_tou_o1_basic_info demand_info = {
      .capacity.customer_charge = 0,
      .capacity.based_on_the_contracted_installed_capacity_rate = 0,
      .info.customer_charge = 262.5,
      .info.regular_contract.contracted_demand = 95,
      .info.regular_contract.demand_charge_rate = 236.2,
      .info.regular_contract.energy_charge_rate = 3.24,
      .info.sat_partial_peak_contract.energy_charge_rate = 2.14,
      .info.off_peak_contract.energy_charge_rate = 1.39};

  if (TAIPOWER_SUCC !=
      power_tou_o1_charge_calc(&device_charge, device_ec, device_info)) {
    return TAIPOWER_ERROR;
  }

  TAIPOWER_DEBUG("total charge:[%lf]\n", device_charge.total_charge);
  TAIPOWER_DEBUG("detail.basic_charge:[%lf]\n",
                 device_charge.detail.basic_charge);
  TAIPOWER_DEBUG("detail.energy_charge:[%lf]\n",
                 device_charge.detail.energy_charge);
  TAIPOWER_DEBUG("detail.exceed_peak_contract_fine:[%lf]\n",
                 device_charge.detail.exceed_peak_contract_fine);
  TAIPOWER_DEBUG("detail.exceed_non_summer_contract_fine:[%lf]\n",
                 device_charge.detail.exceed_non_summer_contract_fine);
  TAIPOWER_DEBUG("detail.exceed_sat_partial_contract_fine:[%lf]\n",
                 device_charge.detail.exceed_sat_partial_contract_fine);
  TAIPOWER_DEBUG("detail.exceed_off_peak_contract_fine:[%lf]\n",
                 device_charge.detail.exceed_off_peak_contract_fine);

  assert((device_charge.total_charge - 14935) < TAIPOWER_FLOAT_TOLERANCE &&
         (device_charge.total_charge - 14935) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((device_charge.detail.basic_charge - 4230) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (device_charge.detail.basic_charge - 4230) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((device_charge.detail.energy_charge - 10705) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (device_charge.detail.energy_charge - 10705) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((device_charge.detail.exceed_peak_contract_fine - 0) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (device_charge.detail.exceed_peak_contract_fine - 0) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((device_charge.detail.exceed_non_summer_contract_fine - 0) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (device_charge.detail.exceed_non_summer_contract_fine - 0) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((device_charge.detail.exceed_sat_partial_contract_fine - 0) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (device_charge.detail.exceed_sat_partial_contract_fine - 0) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((device_charge.detail.exceed_off_peak_contract_fine - 0) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (device_charge.detail.exceed_off_peak_contract_fine - 0) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  if (TAIPOWER_SUCC !=
      power_tou_o1_charge_calc(&demand_charge, demand_ec, demand_info)) {
    return TAIPOWER_ERROR;
  }

  TAIPOWER_DEBUG("total charge:[%lf]\n", demand_charge.total_charge);
  TAIPOWER_DEBUG("detail.basic_charge:[%lf]\n",
                 demand_charge.detail.basic_charge);
  TAIPOWER_DEBUG("detail.energy_charge:[%lf]\n",
                 demand_charge.detail.energy_charge);
  TAIPOWER_DEBUG("detail.exceed_peak_contract_fine:[%lf]\n",
                 demand_charge.detail.exceed_peak_contract_fine);
  TAIPOWER_DEBUG("detail.exceed_non_summer_contract_fine:[%lf]\n",
                 demand_charge.detail.exceed_non_summer_contract_fine);
  TAIPOWER_DEBUG("detail.exceed_sat_partial_contract_fine:[%lf]\n",
                 demand_charge.detail.exceed_sat_partial_contract_fine);
  TAIPOWER_DEBUG("detail.exceed_off_peak_contract_fine:[%lf]\n",
                 demand_charge.detail.exceed_off_peak_contract_fine);

  assert((demand_charge.total_charge - 49841.5) < TAIPOWER_FLOAT_TOLERANCE &&
         (demand_charge.total_charge - 49841.5) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((demand_charge.detail.basic_charge - 22701.5) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (demand_charge.detail.basic_charge - 22701.5) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((demand_charge.detail.energy_charge - 27140) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (demand_charge.detail.energy_charge - 27140) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((demand_charge.detail.exceed_peak_contract_fine - 0) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (demand_charge.detail.exceed_peak_contract_fine - 0) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((demand_charge.detail.exceed_non_summer_contract_fine - 0) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (demand_charge.detail.exceed_non_summer_contract_fine - 0) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((demand_charge.detail.exceed_sat_partial_contract_fine - 0) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (demand_charge.detail.exceed_sat_partial_contract_fine - 0) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  assert((demand_charge.detail.exceed_off_peak_contract_fine - 0) <
             TAIPOWER_FLOAT_TOLERANCE &&
         (demand_charge.detail.exceed_off_peak_contract_fine - 0) >
             TAIPOWER_FLOAT_NEGATIVE_TOLERANCE);

  return TAIPOWER_SUCC;
}