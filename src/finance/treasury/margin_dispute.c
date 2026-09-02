/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/margin_dispute.c
 *
 * PURPOSE:
 *   Implement track margin dispute amount and resolution state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/margin_dispute.h"
#include <string.h>
/*
 * Initialise treasury margin dispute from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_margin_dispute_init(UmiTreasuryMarginDispute *value,
    const char *id,
    int64_t disputed_minor,
    int64_t resolved_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->disputed_minor=disputed_minor;
    value->resolved_minor=resolved_minor;
    return umi_treasury_margin_dispute_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury margin dispute satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_margin_dispute_valid(const UmiTreasuryMarginDispute *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->disputed_minor >= 0 && value->resolved_minor >= 0 && value->resolved_minor <= value->disputed_minor);
}

/*
 * Provide the treasury margin dispute outstanding minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_margin_dispute_outstanding_minor(const UmiTreasuryMarginDispute *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->disputed_minor - value->resolved_minor;
}
