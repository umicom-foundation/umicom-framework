/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/checkpoint.c
 * PURPOSE: Reuse canonical banking event encoding for independent integrity anchors.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include "umicom/bank_operations/checkpoint.h"
#include <string.h>

UmiStatus UmiBankOperationsCheckpoint(const UmiBankOperations *operations,
    const unsigned char chain_id[32], uint64_t expected_revision,
    UmiRecordChainCheckpoint *out)
{
    UmiRecordChainCheckpoint candidate;
    UmiRecordChainRecord record;
    char payload[BANK_RECORD_TEXT_CAPACITY];
    UmiStatus status;
    if (operations == NULL || out == NULL || chain_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (operations->poisoned || operations->state == NULL) return UMI_STATUS_INVALID_STATE;
    const BankState *state = operations->state;
    if (state->counts.revision != expected_revision) return UMI_STATUS_BUSY;
    if (state->counts.events > UMI_BANK_EVENT_CAPACITY ||
        state->counts.events != state->counts.revision) return UMI_STATUS_INVALID_STATE;
    status = umi_record_chain_start(chain_id, &candidate);
    if (status != UMI_STATUS_OK) return status;
    /* Canonical retained commands include actors and exact money. Reusing the
     * persistence codec keeps padding and presentation text out of the anchor. */
    for (size_t i = 0U; i < state->counts.events; ++i) {
        const UmiBankAuditEvent *event = &state->events[i];
        if (event->revision != i + 1U || event->command.expectedRevision != i)
            return UMI_STATUS_INVALID_STATE;
        status = BankCommandValid(&event->actor, &event->command);
        if (status == UMI_STATUS_OK) status = BankEncode(event, payload, sizeof payload);
        if (status == UMI_STATUS_OK) status = umi_record_chain_append(&candidate,
            UINT32_C(0x55424f31), payload, strlen(payload), &record);
        if (status != UMI_STATUS_OK) return status;
    }
    *out = candidate;
    return UMI_STATUS_OK;
}
