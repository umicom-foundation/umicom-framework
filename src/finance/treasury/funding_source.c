/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/funding_source.c
 *
 * PURPOSE:
 *   Implement model a funding facility with capacity, drawn amount and cost.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/funding_source.h"
#include <string.h>
/*
 * Initialise treasury funding source from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_funding_source_init(UmiTreasuryFundingSource *value,
    const char *id,
    int64_t capacity_minor,
    int64_t drawn_minor,
    int32_t spread_bps) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->capacity_minor=capacity_minor;
    value->drawn_minor=drawn_minor;
    value->spread_bps=spread_bps;
    return umi_treasury_funding_source_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury funding source satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_funding_source_valid(const UmiTreasuryFundingSource *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->capacity_minor >= 0 && value->drawn_minor >= 0 && value->drawn_minor <= value->capacity_minor && value->spread_bps >= 0);
}

/*
 * Provide the treasury funding source available minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_funding_source_available_minor(const UmiTreasuryFundingSource *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->capacity_minor - value->drawn_minor;
}
