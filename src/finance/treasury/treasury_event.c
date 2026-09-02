/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/treasury_event.c
 *
 * PURPOSE:
 *   Implement record sequence-ordered treasury domain events with event timestamp.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/treasury_event.h"
#include <string.h>
/*
 * Initialise treasury treasury event from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_treasury_event_init(UmiTreasuryTreasuryEvent *value,
    const char *id,
    uint64_t sequence,
    int64_t event_epoch_millis) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->sequence=sequence;
    value->event_epoch_millis=event_epoch_millis;
    return umi_treasury_treasury_event_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury treasury event satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_treasury_event_valid(const UmiTreasuryTreasuryEvent *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->sequence > 0U && value->event_epoch_millis >= 0);
}

/*
 * Provide the treasury treasury event event sequence operation used by this module and its
 * client applications.
 */
uint64_t umi_treasury_treasury_event_event_sequence(const UmiTreasuryTreasuryEvent *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (uint64_t)0;
    return value->sequence;
}
