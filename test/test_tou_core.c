#include "taipower.h"
#include <assert.h>

/**
 * @brief 測試時間電價共用函式
 */
int test_tou_core() {
  tou_o1_charge charge2 = {0};
  tou_o1_engery_consumption energy2 = {.peak = 2000,
                                       .sat_partial_peak = 500,
                                       .off_peak = 1500,
                                       .non_summer = 0,
                                       .peak_max_demand = 10,
                                       .sat_partial_peak_max_demand = 10,
                                       .off_peak_max_demand = 10,
                                       .non_summer_max_demand = 0};
  tou_o1_basic_info basic_info2 = {
      .customer_charge = 262.5,
      .regular_contract.demand_charge_rate = 236.20,
      .regular_contract.contracted_demand = 20,
      .regular_contract.energy_charge_rate = 3.24,
      .sat_partial_peak_contract.demand_charge_rate = 47.20,
      .sat_partial_peak_contract.contracted_demand = 0,
      .sat_partial_peak_contract.energy_charge_rate = 2.14,
      .off_peak_contract.demand_charge_rate = 47.20,
      .off_peak_contract.contracted_demand = 0,
      .off_peak_contract.energy_charge_rate = 1.39};
  tou_o2_charge charge3 = {0};
  tou_o2_engery_consumption energy3 = {.peak = 81240,
                                       .sat_partial_peak = 9480,
                                       .off_peak = 36000,
                                       .partial_peak = 0,
                                       .peak_max_demand = 534,
                                       .sat_partial_peak_max_demand = 312,
                                       .off_peak_max_demand = 313,
                                       .partial_peak_max_demand = 0};
  tou_o2_basic_info basic_info3 = {
      .customer_charge = 0,
      .regular_contract.demand_charge_rate = 166.9,
      .regular_contract.contracted_demand = 480,
      .regular_contract.energy_charge_rate = 3.08,
      .partial_peak_contract.demand_charge_rate = 166.9,
      .partial_peak_contract.contracted_demand = 0,
      .partial_peak_contract.energy_charge_rate = 3.08,
      .sat_partial_peak_contract.demand_charge_rate = 47.20,
      .sat_partial_peak_contract.contracted_demand = 0,
      .sat_partial_peak_contract.energy_charge_rate = 2.13,
      .off_peak_contract.demand_charge_rate = 47.20,
      .off_peak_contract.contracted_demand = 0,
      .off_peak_contract.energy_charge_rate = 1.53};

  tou_o1_charge_calc(&charge2, energy2, basic_info2);

  TAIPOWER_DEBUG("charge2.total_charge:[%lf]\n", charge2.total_charge);
  assert((charge2.total_charge - 14621.5) < TAIPOWER_FLOAT_TOLERANCE);
  TAIPOWER_DEBUG("charge2.detail.basic_charge:[%lf]\n",
                 charge2.detail.basic_charge);
  assert((charge2.detail.basic_charge - 4986.5) < TAIPOWER_FLOAT_TOLERANCE);
  TAIPOWER_DEBUG("charge2.detail.energy_charge:[%lf]\n",
                 charge2.detail.energy_charge);
  assert((charge2.detail.energy_charge - 9635) < TAIPOWER_FLOAT_TOLERANCE);
  TAIPOWER_DEBUG("charge2.detail.exceed_peak_contract_fine:[%lf]\n",
                 charge2.detail.exceed_peak_contract_fine);
  assert((charge2.detail.exceed_peak_contract_fine - 0) <
         TAIPOWER_FLOAT_TOLERANCE);
  TAIPOWER_DEBUG("charge2.detail.exceed_non_summer_contract_fine:[%lf]\n",
                 charge2.detail.exceed_non_summer_contract_fine);
  assert((charge2.detail.exceed_non_summer_contract_fine - 0) <
         TAIPOWER_FLOAT_TOLERANCE);
  TAIPOWER_DEBUG("charge2.detail.exceed_sat_partial_contract_fine:[%lf]\n",
                 charge2.detail.exceed_sat_partial_contract_fine);
  assert((charge2.detail.exceed_sat_partial_contract_fine - 0) <
         TAIPOWER_FLOAT_TOLERANCE);
  TAIPOWER_DEBUG("charge2.detail.exceed_off_peak_contract_fine:[%lf]\n",
                 charge2.detail.exceed_off_peak_contract_fine);
  assert((charge2.detail.exceed_off_peak_contract_fine - 0) <
         TAIPOWER_FLOAT_TOLERANCE);

  tou_o2_charge_calc(&charge3, energy3, basic_info3);

  TAIPOWER_DEBUG("charge3.total_charge:[%lf]\n", charge3.total_charge);
  assert((charge3.total_charge - 424630.2) < TAIPOWER_FLOAT_TOLERANCE);
  TAIPOWER_DEBUG("charge3.detail.basic_charge:[%lf]\n",
                 charge3.detail.basic_charge);
  assert((charge3.detail.basic_charge - 80112.0) < TAIPOWER_FLOAT_TOLERANCE);
  TAIPOWER_DEBUG("charge3.detail.energy_charge:[%lf]\n",
                 charge3.detail.energy_charge);
  assert((charge3.detail.energy_charge - 325491.6) < TAIPOWER_FLOAT_TOLERANCE);
  TAIPOWER_DEBUG("charge3.detail.exceed_peak_contract_fine:[%lf]\n",
                 charge3.detail.exceed_peak_contract_fine);
  assert((charge3.detail.exceed_peak_contract_fine - 19026.6) <
         TAIPOWER_FLOAT_TOLERANCE);
  TAIPOWER_DEBUG("charge3.detail.exceed_partial_peak_contract_fine:[%lf]\n",
                 charge3.detail.exceed_partial_peak_contract_fine);
  assert((charge3.detail.exceed_partial_peak_contract_fine - 0) <
         TAIPOWER_FLOAT_TOLERANCE);
  TAIPOWER_DEBUG("charge3.detail.exceed_sat_partial_contract_fine:[%lf]\n",
                 charge3.detail.exceed_sat_partial_contract_fine);
  assert((charge3.detail.exceed_sat_partial_contract_fine - 0) <
         TAIPOWER_FLOAT_TOLERANCE);
  TAIPOWER_DEBUG("charge3.detail.exceed_off_peak_contract_fine:[%lf]\n",
                 charge3.detail.exceed_off_peak_contract_fine);
  assert((charge3.detail.exceed_off_peak_contract_fine - 0) <
         TAIPOWER_FLOAT_TOLERANCE);

  return 0;
}