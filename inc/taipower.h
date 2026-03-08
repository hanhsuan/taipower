#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG
#define TAIPOWER_DEBUG(fmt, ...)                                               \
  fprintf(stdout, "%s:%s:%d: " fmt, __FILE__, __FUNCTION__, __LINE__,          \
          ##__VA_ARGS__)
#else
#define TAIPOWER_DEBUG(fmt, ...)                                               \
  {}
#endif

/** 用於避免使用 assert 於 GCC 編譯時誤認為變數未使用
 * https://stackoverflow.com/questions/777261/avoiding-unused-variables-warnings-when-using-assert-in-a-release-build
 */
#define _unused(x) ((void)(x))

/** 非時間電價剩餘度數標記 */
#define TAIPOWER_EXCEED_KWH 100000
/** 自行標示不計算功率因素獎懲 */
#define TAIPOWER_DONT_COUNT_POWER_FACTOR -1
/** 處理正確 */
#define TAIPOWER_SUCC 0
/** 處理錯誤 */
#define TAIPOWER_ERROR -1

/** 浮點數比對容忍值 */
#define TAIPOWER_FLOAT_TOLERANCE 0.01
/** 負值浮點數比對容忍值 */
#define TAIPOWER_FLOAT_NEGATIVE_TOLERANCE -0.01
/** 僅週六半尖峰及離蜂需量其他時段用電需量契約罰款倍數 */
#define TAIPOWER_ONLY_SAT_PARTIAL_AND_OFF_PEAK_CONTRANT_DEMAND_FINE_RATE 4.8
/** 僅週六半尖峰及離蜂需量其他時段用電流動電費罰款倍數 */
#define TAIPOWER_ONLY_SAT_PARTIAL_AND_OFF_PEAK_CONTRANT_ENERGY_FINE_RATE 1.6

/**
 * @brief 判斷是否為閏年
 * @ref
 * https://stackoverflow.com/questions/9852837/leap-year-check-using-bitwise-operators-amazing-speed
 */
#define IS_LEAP_YEAR(year) (!(year & 3 || (year & 15 && !(year % 25))))

/**
 * @brief 功率因素物件
 * (power factor information)
 */
typedef struct power_factor_info {
  int power_factor;          /**< 實際功率因素 */
  int min_demand_threshold; /**< 超過此 kw 就需要計算功率因素，
                              * TAIPOWER_DONT_COUNT_POWER_FACTOR 代表不計算
                              */
  int baseline_percentage;      /**< 功率因素計算之基準功率因素(%) */
  int max_adjustment_percentage; /**< 功率因素超過多少百分比就不再有折扣 */
  double discount_per_percentage; /**< 每增加 1% 可獲得之折扣百分比(%) */
  double surcharge_per_percentage; /**< 每減少 1% 須增加之罰款百分比(%) */
} power_factor_info;

/**
 * @brief 表燈(住商)非時間電價級距細節物件
 * (non time of use rate for meter rate)
 */
typedef struct non_time_of_use_rate {
  double rate; /**< 各級距費率 */
  int level;   /**< 費率級距 */
} non_time_of_use_rate;

/**
 * @brief 表燈(住商)非時間電價物件
 * (basic information of non time of use for meter rate lighting service)
 */
typedef struct non_time_of_use_basic_info {
  non_time_of_use_rate *levels;        /**< 級距細節 */
  power_factor_info power_factor; /**< 功率因素物件 */
  int level_count;                /**< 費率分級數量 */
} non_time_of_use_basic_info;

/**
 * @brief 表燈(住商)兩段式簡易時間電價物件
 * (basic information of time of use for meter rate lighting service a option 1)
 */
typedef struct time_of_use_a_option_1_basic_info {
  double customer_charge;   /**< 按戶計收基本電費 */
  double peak_rate;         /**< 尖峰費率 (NTD/kwh) */
  double off_peak_rate;     /**< 離峰費率 (NTD/kwh) */
  double weekend_off_peak;  /**< 週六/日離峰費率 (NTD/kwh) */
  double exceed_limit_rate; /**< 超額加收費率 */
  power_factor_info power_factor; /**< 功率因素物件 */
  int usage_limit_kwh;     /**< 超過此度數(kwh)要加收 */
} time_of_use_a_option_1_basic_info;

/**
 * @brief 表燈(住商)簡易時間電價兩段式電力消費物件
 * (energy consumption of time of use for meter rate lighting service a option
 * 1)
 */
typedef struct time_of_use_a_option_1_energy_consumption {
  double peak;             /**< 尖峰用電量 (kwh) */
  double off_peak;         /**< 離峰用電量 (kwh) */
  double weekend_off_peak; /**< 週六/日離峰用電量 (kwh) */
} time_of_use_a_option_1_energy_consumption;

/**
 * @brief 表燈(住商)三段式簡易時間電價物件
 * (basic information of time of use for meter rate lighting service a option 2)
 */
typedef struct time_of_use_a_option_2_basic_info {
  double customer_charge; /**< 按戶計收基本電費 */
  double peak_rate; /**< 尖峰費率 (NTD/kwh)，非夏月時同非夏月半尖峰費率 */
  double partial_peak_rate; /**< 半尖峰費率 (NTD/kwh) */
  double off_peak_rate;     /**< 離峰費率 (NTD/kwh) */
  double weekend_off_peak;  /**< 週六/日離峰費率 (NTD/kwh) */
  double exceed_limit_rate; /**< 超額加收費率 */
  power_factor_info power_factor; /**< 功率因素物件 */
  int usage_limit_kwh;     /**< 超過此度數(kwh)要加收 */
} time_of_use_a_option_2_basic_info;

/**
 * @brief 表燈(住商)簡易時間電價三段式電力消費物件
 * (energy consumption of time of use for meter rate lighting service a option
 * 2)
 */
typedef struct time_of_use_a_option_2_energy_consumption {
  double peak;             /**< 尖峰用電量 (kwh) */
  double partial_peak;     /**< 半尖峰用電量 (kwh) */
  double off_peak;         /**< 離峰用電量 (kwh) */
  double weekend_off_peak; /**< 週六/日離峰用電量 (kwh) */
} time_of_use_a_option_2_energy_consumption;

/**
 * @brief 時間電價契約內容物件
 * (information of contract)
 */
typedef struct contract_info {
  double contracted_demand;  /**< 契約需量 (kwh) */
  double demand_charge_rate; /**< 基本費率 (NTD/kwh) */
  double energy_charge_rate; /**< 流動費率 (NTD/kwh) */
} contract_info;

/**
 * @brief 兩段式時間電價物件
 * (basic information of time of use option 1)
 */
typedef struct time_of_use_option_1_basic_info {
  double customer_charge;                   /**< 按戶計收基本電費 */
  contract_info regular_contract;    /**< 經常契約 */
  contract_info non_summer_contract; /**< 非夏月契約 */
  contract_info sat_partial_peak_contract; /**< 週六半尖峰契約 */
  contract_info off_peak_contract;         /**< 離峰契約 */
  power_factor_info power_factor;          /**< 功率因素物件 */
} time_of_use_option_1_basic_info;

/**
 * @brief 低壓兩段式時間電價基本設定費物件
 * (installed capacity contract of power time of use option 1)
 */
typedef struct low_voltage_time_of_use_option_1_capacity_contract {
  double customer_charge; /**< 按戶計收基本電費 */
  double based_on_the_contracted_installed_capacity_rate; /**<  裝置設定費率 */
} low_voltage_time_of_use_option_1_capacity_contract;

/**
 * @brief 低壓兩段式時間電價物件
 * (basic information of power time of use option 1)
 */
typedef struct low_voltage_time_of_use_option_1_basic_info {
  low_voltage_time_of_use_option_1_capacity_contract
      capacity; /**< 低壓兩段式時間電價基本設定費物件 */
  time_of_use_option_1_basic_info info; /**< 兩段式時間電價物件 */
} low_voltage_time_of_use_option_1_basic_info;

/**
 * @brief 三段式時間電價物件
 * (basic information of time of use option 2)
 */
typedef struct time_of_use_option_2_basic_info {
  double customer_charge;                     /**< 按戶計收基本電費 */
  contract_info regular_contract;      /**< 經常契約 */
  contract_info partial_peak_contract; /**< 半尖峰契約 */
  contract_info sat_partial_peak_contract; /**< 週六半尖峰契約 */
  contract_info off_peak_contract;         /**< 離峰契約 */
  power_factor_info power_factor;          /**< 功率因素物件 */
} time_of_use_option_2_basic_info;

/**
 * @brief 兩段式時間電價電力消費物件
 * (energy consumption of time of use option 1)
 */
typedef struct time_of_use_option_1_energy_consumption {
  double peak;       /**< 尖峰用電量 (kwh) */
  double non_summer; /**< 非夏月用電量 (kwh)，若為 0 則代表夏月 */
  double sat_partial_peak;            /**< 週六半尖峰用電量 (kwh) */
  double off_peak;                    /**< 離峰用電量 (kwh) */
  double peak_max_demand;             /**< 經常(尖峰)最大需量 (kw) */
  double non_summer_max_demand;       /**< 非夏月最大需量 (kw) */
  double sat_partial_peak_max_demand; /**< 週六半尖峰最大需量 (kw) */
  double off_peak_max_demand;         /**< 離峰最大需量 (kw) */
} time_of_use_option_1_energy_consumption;

/**
 * @brief 三段式時間電價電力消費物件
 * (energy consumption of time of use option 2)
 */
typedef struct time_of_use_option_2_energy_consumption {
  double peak;                        /**< 尖峰用電量 (kwh) */
  double partial_peak;                /**< 半尖峰用電量 (kwh) */
  double sat_partial_peak;            /**< 週六半尖峰用電量 (kwh) */
  double off_peak;                    /**< 離峰用電量 (kwh) */
  double peak_max_demand;             /**< 經常(尖峰)最大需量 (kw) */
  double partial_peak_max_demand;     /**< 半尖峰最大需量 (kw) */
  double sat_partial_peak_max_demand; /**< 週六半尖峰最大需量 (kw) */
  double off_peak_max_demand;         /**< 離峰最大需量 (kw) */
} time_of_use_option_2_energy_consumption;

/**
 * @brief 兩段式時間電價電費細項
 * (charge detail of time of use option 1)
 */
typedef struct time_of_use_option_1_charge_detail {
  double basic_charge;                     /**< 基本電費 */
  double energy_charge;                    /**< 流動電費 */
  double exceed_peak_contract_fine;        /**< 尖峰超約罰款 */
  double exceed_non_summer_contract_fine;  /**< 非夏月超約罰款 */
  double exceed_sat_partial_contract_fine; /**< 週六半尖峰超約罰款 */
  double exceed_off_peak_contract_fine;    /**< 離峰超約罰款 */
  double power_factor_adjustment;              /**< 功率因素獎懲 */
} time_of_use_option_1_charge_detail;

/**
 * @brief 三段式時間電價電費細項
 * (charge detail of time of use option 2)
 */
typedef struct time_of_use_option_2_charge_detail {
  double basic_charge;                      /**< 基本電費 */
  double energy_charge;                     /**< 流動電費 */
  double exceed_peak_contract_fine;         /**< 尖峰超約罰款 */
  double exceed_partial_peak_contract_fine; /**< 半尖峰超約罰款 */
  double exceed_sat_partial_contract_fine;  /**< 週六半尖峰超約罰款 */
  double exceed_off_peak_contract_fine;     /**< 離峰超約罰款 */
  double power_factor_adjustment;               /**< 功率因素獎懲 */
} time_of_use_option_2_charge_detail;

/**
 * @brief 兩段式時間電價電費
 * (charge of time of use option 1)
 */
typedef struct time_of_use_option_1_charge {
  double total_charge;             /**< 電費 */
  time_of_use_option_1_charge_detail detail; /**< 電費細項 */
} time_of_use_option_1_charge;

/**
 * @brief 三段式時間電價電費
 * (charge of time of use option 2)
 */
typedef struct time_of_use_option_2_charge {
  double total_charge;             /**< 電費 */
  time_of_use_option_2_charge_detail detail; /**< 電費細項 */
} time_of_use_option_2_charge;

/**
 * @brief 日期物件
 * (date object)
 */
typedef struct taipower_date {
  int year;  /**< 年 */
  int month; /**< 月 */
  int day;   /**< 日 */
} taipower_date;

/**
 * @brief 計算表燈(住商)非時間電價
 * @param ec 所用度數
 * @param info 表燈(住商)非時間電價物件
 * @return 表燈(住商)非時間電價
 */
double non_time_of_use_charge_calc(double ec, non_time_of_use_basic_info info);

/**
 * @brief 計算表燈(住商)簡易時間兩段式電價
 * @param ec 所用度數
 * @param info 表燈(住商)簡易時間電價兩段式電力消費物件
 * @return 表燈(住商)簡易時間兩段式電價
 */
double time_of_use_a_option_1_charge_calc(time_of_use_a_option_1_energy_consumption ec,
                                  time_of_use_a_option_1_basic_info info);

/**
 * @brief 計算表燈(住商)簡易時間三段式電價
 * @param ec 所用度數
 * @param info 表燈(住商)簡易時間電價三段式電力消費物件
 * @return 表燈(住商)簡易時間三段式電價
 */
double time_of_use_a_option_2_charge_calc(time_of_use_a_option_2_energy_consumption ec,
                                  time_of_use_a_option_2_basic_info info);

/**
 * @brief 計算表燈(住商)標準時間兩段式電價
 * @param charge 電費(time_of_use_option_1_charge物件)
 * @param ec 所用度數(time_of_use_option_1_energy_consumption物件)
 * @param info 兩段式時間電價電力消費(time_of_use_option_1_basic_info物件)
 * @return TAIPOWER_SUCC/TAIPOWER_ERROR
 */
int time_of_use_b_option_1_charge_calc(time_of_use_option_1_charge *charge,
                               time_of_use_option_1_energy_consumption ec,
                               time_of_use_option_1_basic_info info);

/**
 * @brief 計算表燈(住商)標準時間三段式電價
 * @param charge 電費(time_of_use_option_2_charge物件)
 * @param ec 所用度數(time_of_use_option_2_energy_consumption物件)
 * @param info 三段式時間電價電力消費(time_of_use_option_2_basic_info物件)
 * @return TAIPOWER_SUCC/TAIPOWER_ERROR
 */
int time_of_use_b_option_2_charge_calc(time_of_use_option_2_charge *charge,
                               time_of_use_option_2_energy_consumption ec,
                               time_of_use_option_2_basic_info info);

/**
 * @brief 低壓兩段式時間電價
 * @param charge 電費(time_of_use_option_1_charge物件)
 * @param ec 所用度數(time_of_use_option_1_energy_consumption物件)
 * @param info 兩段式時間電價電力消費(low_voltage_time_of_use_option_1_basic_info物件)
 * @return TAIPOWER_SUCC/TAIPOWER_ERROR
 */
int low_voltage_time_of_use_option_1_charge_calc(time_of_use_option_1_charge *charge,
                             time_of_use_option_1_energy_consumption ec,
                             low_voltage_time_of_use_option_1_basic_info info);

/**
 * @brief 低壓三段式時間電價
 * @param charge 電費(time_of_use_option_2_charge物件)
 * @param ec 所用度數(time_of_use_option_2_energy_consumption物件)
 * @param info 三段式時間電價電力消費(time_of_use_option_2_basic_info物件)
 * @return TAIPOWER_SUCC/TAIPOWER_ERROR
 */
int low_voltage_time_of_use_option_2_charge_calc(time_of_use_option_2_charge *charge,
                             time_of_use_option_2_energy_consumption ec,
                             time_of_use_option_2_basic_info info);

/**
 * @brief 高壓兩段式時間電價
 * @param charge 電費(time_of_use_option_1_charge物件)
 * @param ec 所用度數(time_of_use_option_1_energy_consumption物件)
 * @param info 兩段式時間電價電力消費(time_of_use_option_1_basic_info物件)
 * @return TAIPOWER_SUCC/TAIPOWER_ERROR
 */
int high_voltage_time_of_use_option_1_charge_calc(time_of_use_option_1_charge *charge,
                                    time_of_use_option_1_energy_consumption ec,
                                    time_of_use_option_1_basic_info info);

/**
 * @brief 高壓三段式時間電價
 * @param charge 電費(time_of_use_option_2_charge物件)
 * @param ec 所用度數(time_of_use_option_2_energy_consumption物件)
 * @param info 三段式時間電價電力消費(time_of_use_option_2_basic_info物件)
 * @return TAIPOWER_SUCC/TAIPOWER_ERROR
 */
int high_voltage_time_of_use_option_2_charge_calc(time_of_use_option_2_charge *charge,
                                    time_of_use_option_2_energy_consumption ec,
                                    time_of_use_option_2_basic_info info);

/**
 * @brief 特高壓兩段式時間電價
 * @param charge 電費(time_of_use_option_1_charge物件)
 * @param ec 所用度數(time_of_use_option_1_energy_consumption物件)
 * @param info 兩段式時間電價電力消費(time_of_use_option_1_basic_info物件)
 * @return TAIPOWER_SUCC/TAIPOWER_ERROR
 */
int extra_high_voltage_time_of_use_option_1_charge_calc(time_of_use_option_1_charge *charge,
                                          time_of_use_option_1_energy_consumption ec,
                                          time_of_use_option_1_basic_info info);

/**
 * @brief 特高壓三段式時間電價
 * @param charge 電費(time_of_use_option_2_charge物件)
 * @param ec 所用度數(time_of_use_option_2_energy_consumption物件)
 * @param info 三段式時間電價電力消費(time_of_use_option_2_basic_info物件)
 * @return TAIPOWER_SUCC/TAIPOWER_ERROR
 */
int extra_high_voltage_time_of_use_option_2_charge_calc(time_of_use_option_2_charge *charge,
                                          time_of_use_option_2_energy_consumption ec,
                                          time_of_use_option_2_basic_info info);

/**
 * @brief 計算兩段式時間電價
 * @param charge 電費(time_of_use_option_1_charge物件)
 * @param ec 所用度數(time_of_use_option_1_energy_consumption物件)
 * @param info 兩段式時間電價電力消費物件
 * @return TAIPOWER_SUCC/TAIPOWER_ERROR
 */
int time_of_use_option_1_charge_calc(time_of_use_option_1_charge *charge,
                       time_of_use_option_1_energy_consumption ec,
                       time_of_use_option_1_basic_info info);

/**
 * @brief 計算三段式時間電價
 * @param charge 電費(time_of_use_option_2_charge物件)
 * @param ec 所用度數(time_of_use_option_2_energy_consumption物件)
 * @param info 三段式時間電價電力消費物件
 * @return TAIPOWER_SUCC/TAIPOWER_ERROR
 */
int time_of_use_option_2_charge_calc(time_of_use_option_2_charge *charge,
                       time_of_use_option_2_energy_consumption ec,
                       time_of_use_option_2_basic_info info);

/**
 * @brief 計算時間電價超約罰款共通函式
 * @param contract 契約需量 (kw)
 * @param exceed_contract 超約量 (kw)
 * @param counted_contract 超約量中已計算部份
 * (kw)，正常執行後會將此次計算量累計儲存於此變數
 * @param base_rate 契約費率
 * @return 超約罰款
 */
double exceed_contract_fine_calc(double contract, double exceed_contract,
                 double *counted_contract, double demand_charge_rate);
/**
 * @brief 計算功率因素獎懲共用函式
 * @param subtotal_charge 基本電費加流動電費
 * @param pf 功率因素物件
 * @return 功率因素獎懲費用
 */
double power_factor_adjustment_calc(double subtotal_charge,
                                power_factor_info pf);

/**
 * @brief 依照指定格式檢查日期格式是否正確
 * @param date 儲存檢查正確並轉換為數字之日期物件
 * @param format 指定格式，c 表示民國年、y 表示西元年、M 表示月份、d 表示日
 * @param date_str 待檢查及轉換之日期
 * @param length 待檢查及轉換之日期字串長度
 * @return TAIPOWER_SUCC/TAIPOWER_ERROR
 */
int is_valid_date_format(taipower_date *date, const char *format,
                         const char *date_str, const int length);

/**
 * @brief 計算兩日期間之天數差異，以日期物件2為基準日，且不包含結尾日
 * @param date1 日期物件1，請先確認格式正確
 * @param date2 日期物件2，請先確認格式正確
 * @return 天數差異
 */
int diff_days(taipower_date date1, taipower_date date2);
