/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/custody_position.c
 *
 * PURPOSE:
 *   Implement represent settled, pending-in and pending-out custody quantities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/custody_position.h"
#include <string.h>
/*
 * Initialise treasury custody position from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_custody_position_init(UmiTreasuryCustodyPosition *value,
    const char *id,
    int64_t settled_quantity,
    int64_t pending_in_quantity,
    int64_t pending_out_quantity) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->settled_quantity=settled_quantity;
    value->pending_in_quantity=pending_in_quantity;
    value->pending_out_quantity=pending_out_quantity;
    return umi_treasury_custody_position_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury custody position satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_custody_position_valid(const UmiTreasuryCustodyPosition *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->settled_quantity >= 0 && value->pending_in_quantity >= 0 && value->pending_out_quantity >= 0);
}

/*
 * Provide the treasury custody position projected quantity operation used by this module
 * and its client applications.
 */
int64_t umi_treasury_custody_position_projected_quantity(const UmiTreasuryCustodyPosition *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->settled_quantity + value->pending_in_quantity - value->pending_out_quantity;
}
