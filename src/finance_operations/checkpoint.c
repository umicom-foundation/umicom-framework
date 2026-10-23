/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance_operations/checkpoint.c
 * PURPOSE: Commit to ordered canonical financial commands using the shared chain service.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include "umicom/finance_operations/checkpoint.h"
#include <string.h>

UmiStatus UmiFinanceOperationsCheckpoint(const UmiFinanceOperations *operations,
    const unsigned char chain_id[32], uint64_t expected_revision,
    UmiRecordChainCheckpoint *out)
{
    UmiRecordChainCheckpoint candidate;
    UmiRecordChainRecord record;
    char payload[FINANCE_RECORD_CAPACITY];
    UmiStatus status;
    if (operations == NULL || out == NULL || chain_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (operations->poisoned || operations->writesBlocked || operations->state == NULL)
        return UMI_STATUS_INVALID_STATE;
    const FinanceState *state = operations->state;
    if (state->counts.revision != expected_revision) return UMI_STATUS_BUSY;
    if (state->counts.events > UMI_FINANCE_OPERATIONS_EVENTS ||
        state->counts.events != state->counts.revision) return UMI_STATUS_INVALID_STATE;
    status = umi_record_chain_start(chain_id, &candidate);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < state->counts.events; ++i) {
        const UmiFinanceOperationCommand *command = &state->commands[i];
        if (command->expectedRevision != i) return UMI_STATUS_INVALID_STATE;
        status = FinanceEncode(command, payload, sizeof payload);
        if (status == UMI_STATUS_OK) status = umi_record_chain_append(&candidate,
            UINT32_C(0x55464931), payload, strlen(payload), &record);
        if (status != UMI_STATUS_OK) return status;
    }
    *out = candidate;
    return UMI_STATUS_OK;
}
