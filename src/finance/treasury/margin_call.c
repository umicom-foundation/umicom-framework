/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/margin_call.c
 *
 * PURPOSE:
 *   Implement represent a margin call amount, agreed amount and lifecycle state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/margin_call.h"
#include <string.h>
/*
 * Initialise treasury margin call from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_margin_call_init(UmiTreasuryMarginCall *value,
    const char *id,
    int64_t called_minor,
    int64_t agreed_minor,
    UmiTreasuryMarginState state) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->called_minor=called_minor;
    value->agreed_minor=agreed_minor;
    value->state=state;
    return umi_treasury_margin_call_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury margin call satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_margin_call_valid(const UmiTreasuryMarginCall *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->called_minor >= 0 && value->agreed_minor >= 0 && value->agreed_minor <= value->called_minor && value->state >= UMI_TREASURY_MARGIN_OPEN && value->state <= UMI_TREASURY_MARGIN_SETTLED);
}

/*
 * Provide the treasury margin call unagreed minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_margin_call_unagreed_minor(const UmiTreasuryMarginCall *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->called_minor - value->agreed_minor;
}
