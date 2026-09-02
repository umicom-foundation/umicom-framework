/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/margin_dispute.h
 *
 * PURPOSE:
 *   Track margin dispute amount and resolution state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_MARGIN_DISPUTE_H
#define UMICOM_FINANCE_TREASURY_MARGIN_DISPUTE_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury margin dispute data shared with callers of this public contract.
 */
typedef struct UmiTreasuryMarginDispute {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t disputed_minor;
    int64_t resolved_minor;
} UmiTreasuryMarginDispute;
/**
 * Initialise treasury margin dispute from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_margin_dispute_init(UmiTreasuryMarginDispute *value,
    const char *id,
    int64_t disputed_minor,
    int64_t resolved_minor);
/**
 * Check that treasury margin dispute satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_margin_dispute_valid(const UmiTreasuryMarginDispute *value);
/**
 * Provide the treasury margin dispute outstanding minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_margin_dispute_outstanding_minor(const UmiTreasuryMarginDispute *value);
#ifdef __cplusplus
}
#endif
#endif
