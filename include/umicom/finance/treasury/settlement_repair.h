/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/settlement_repair.h
 *
 * PURPOSE:
 *   Represent a settlement repair action with bounded attempt governance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_SETTLEMENT_REPAIR_H
#define UMICOM_FINANCE_TREASURY_SETTLEMENT_REPAIR_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury settlement repair data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasurySettlementRepair {
    char id[UMI_TREASURY_ID_CAPACITY];
    uint32_t attempt;
    uint32_t maximum_attempts;
} UmiTreasurySettlementRepair;
/**
 * Initialise treasury settlement repair from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_settlement_repair_init(UmiTreasurySettlementRepair *value,
    const char *id,
    uint32_t attempt,
    uint32_t maximum_attempts);
/**
 * Check that treasury settlement repair satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_settlement_repair_valid(const UmiTreasurySettlementRepair *value);
/**
 * Provide the treasury settlement repair retry allowed operation used by this module and
 * its client applications.
 */
bool umi_treasury_settlement_repair_retry_allowed(const UmiTreasurySettlementRepair *value);
#ifdef __cplusplus
}
#endif
#endif
