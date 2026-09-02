/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/collateral_allocation.c
 *
 * PURPOSE:
 *   Implement represent collateral allocation against a secured requirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/collateral_allocation.h"
#include <string.h>
/*
 * Initialise treasury collateral allocation from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_treasury_collateral_allocation_init(UmiTreasuryCollateralAllocation *value,
    const char *id,
    int64_t required_minor,
    int64_t allocated_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->required_minor=required_minor;
    value->allocated_minor=allocated_minor;
    return umi_treasury_collateral_allocation_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury collateral allocation satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_collateral_allocation_valid(const UmiTreasuryCollateralAllocation *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->required_minor >= 0 && value->allocated_minor >= 0);
}

/*
 * Provide the treasury collateral allocation shortfall minor operation used by this module
 * and its client applications.
 */
int64_t umi_treasury_collateral_allocation_shortfall_minor(const UmiTreasuryCollateralAllocation *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->required_minor > value->allocated_minor ? value->required_minor - value->allocated_minor : 0;
}
