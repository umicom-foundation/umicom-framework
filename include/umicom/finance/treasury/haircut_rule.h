/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/haircut_rule.h
 *
 * PURPOSE:
 *   Define collateral valuation haircut in basis points.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_HAIRCUT_RULE_H
#define UMICOM_FINANCE_TREASURY_HAIRCUT_RULE_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryHaircutRule {
    char id[UMI_TREASURY_ID_CAPACITY];
    uint32_t haircut_bps;
} UmiTreasuryHaircutRule;
UmiStatus umi_treasury_haircut_rule_init(UmiTreasuryHaircutRule *value,
    const char *id,
    uint32_t haircut_bps);
bool umi_treasury_haircut_rule_valid(const UmiTreasuryHaircutRule *value);
uint32_t umi_treasury_haircut_rule_remaining_bps(const UmiTreasuryHaircutRule *value);
#ifdef __cplusplus
}
#endif
#endif
