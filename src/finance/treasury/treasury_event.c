/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/treasury_event.c
 *
 * PURPOSE:
 *   Implement record sequence-ordered treasury domain events with event timestamp.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/treasury_event.h"
#include <string.h>
UmiStatus umi_treasury_treasury_event_init(UmiTreasuryTreasuryEvent *value,
    const char *id,
    uint64_t sequence,
    int64_t event_epoch_millis) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->sequence=sequence;
    value->event_epoch_millis=event_epoch_millis;
    return umi_treasury_treasury_event_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_treasury_event_valid(const UmiTreasuryTreasuryEvent *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->sequence > 0U && value->event_epoch_millis >= 0);
}

uint64_t umi_treasury_treasury_event_event_sequence(const UmiTreasuryTreasuryEvent *value) {
    if (value == NULL) return (uint64_t)0;
    return value->sequence;
}
