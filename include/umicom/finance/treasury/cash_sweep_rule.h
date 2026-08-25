/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/cash_sweep_rule.h
 *
 * PURPOSE:
 *   Define threshold-based cash sweeping between treasury accounts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_CASH_SWEEP_RULE_H
#define UMICOM_FINANCE_TREASURY_CASH_SWEEP_RULE_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryCashSweepRule {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t trigger_minor;
    int64_t target_minor;
} UmiTreasuryCashSweepRule;
UmiStatus umi_treasury_cash_sweep_rule_init(UmiTreasuryCashSweepRule *value,
    const char *id,
    int64_t trigger_minor,
    int64_t target_minor);
bool umi_treasury_cash_sweep_rule_valid(const UmiTreasuryCashSweepRule *value);
int64_t umi_treasury_cash_sweep_rule_sweep_minor(const UmiTreasuryCashSweepRule *value);
#ifdef __cplusplus
}
#endif
#endif
