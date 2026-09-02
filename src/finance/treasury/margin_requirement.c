/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/margin_requirement.c
 *
 * PURPOSE:
 *   Implement calculate required margin after threshold and independent amount.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/margin_requirement.h"
#include <string.h>
/*
 * Initialise treasury margin requirement from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_margin_requirement_init(UmiTreasuryMarginRequirement *value,
    const char *id,
    int64_t exposure_minor,
    int64_t threshold_minor,
    int64_t independent_amount_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->exposure_minor=exposure_minor;
    value->threshold_minor=threshold_minor;
    value->independent_amount_minor=independent_amount_minor;
    return umi_treasury_margin_requirement_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury margin requirement satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_margin_requirement_valid(const UmiTreasuryMarginRequirement *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->exposure_minor >= 0 && value->threshold_minor >= 0 && value->independent_amount_minor >= 0);
}

/*
 * Provide the treasury margin requirement required minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_margin_requirement_required_minor(const UmiTreasuryMarginRequirement *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return (value->exposure_minor > value->threshold_minor ? value->exposure_minor - value->threshold_minor : 0) + value->independent_amount_minor;
}
