/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/limit_utilization.h
 *
 * PURPOSE:
 *   Calculate risk-limit utilisation using basis points.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_LIMIT_UTILIZATION_H
#define UMICOM_FINANCE_TREASURY_LIMIT_UTILIZATION_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury limit utilization data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryLimitUtilization {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t used_minor;
    int64_t limit_minor;
} UmiTreasuryLimitUtilization;
/**
 * Initialise treasury limit utilization from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_limit_utilization_init(UmiTreasuryLimitUtilization *value,
    const char *id,
    int64_t used_minor,
    int64_t limit_minor);
/**
 * Check that treasury limit utilization satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_limit_utilization_valid(const UmiTreasuryLimitUtilization *value);
/**
 * Provide the treasury limit utilization utilization bps operation used by this module and
 * its client applications.
 */
uint32_t umi_treasury_limit_utilization_utilization_bps(const UmiTreasuryLimitUtilization *value);
#ifdef __cplusplus
}
#endif
#endif
