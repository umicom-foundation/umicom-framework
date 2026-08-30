/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/settlement_cycle.h
 *
 * PURPOSE:
 *   Define settlement cycle trade-date and settlement-date offsets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_SETTLEMENT_CYCLE_H
#define UMICOM_FINANCE_TREASURY_SETTLEMENT_CYCLE_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasurySettlementCycle {
    char id[UMI_TREASURY_ID_CAPACITY];
    int32_t settlement_days;
} UmiTreasurySettlementCycle;
UmiStatus umi_treasury_settlement_cycle_init(UmiTreasurySettlementCycle *value,
    const char *id,
    int32_t settlement_days);
bool umi_treasury_settlement_cycle_valid(const UmiTreasurySettlementCycle *value);
int32_t umi_treasury_settlement_cycle_offset_days(const UmiTreasurySettlementCycle *value);
#ifdef __cplusplus
}
#endif
#endif
