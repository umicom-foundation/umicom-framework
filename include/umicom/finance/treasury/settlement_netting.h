/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/settlement_netting.h
 *
 * PURPOSE:
 *   Calculate bilateral settlement netting across gross pay and receive legs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_SETTLEMENT_NETTING_H
#define UMICOM_FINANCE_TREASURY_SETTLEMENT_NETTING_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury settlement netting data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasurySettlementNetting {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t pay_minor;
    int64_t receive_minor;
} UmiTreasurySettlementNetting;
/**
 * Initialise treasury settlement netting from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_settlement_netting_init(UmiTreasurySettlementNetting *value,
    const char *id,
    int64_t pay_minor,
    int64_t receive_minor);
/**
 * Check that treasury settlement netting satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_settlement_netting_valid(const UmiTreasurySettlementNetting *value);
/**
 * Provide the treasury settlement netting net minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_settlement_netting_net_minor(const UmiTreasurySettlementNetting *value);
#ifdef __cplusplus
}
#endif
#endif
