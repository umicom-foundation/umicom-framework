/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance_operations/checkpoint.c
 *
 * PURPOSE:
 *   Reproduce a read-only record-chain tip over Finance Operations' already
 *   validated canonical commands, preserving independent state ownership.
 *
 * ENGINEERING:
 *   The existing FinanceEncode codec defines the committed byte stream;
 *   no raw struct bytes, additional serializers or ad-hoc hashing are used.
 *   A temporary checkpoint is published only after the complete event history
 *   has passed revision, ordering and serialization checks.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance_operations/checkpoint.h"
#include "internal.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

UmiStatus UmiFinanceOperationsCheckpoint(
    const UmiFinanceOperations *operations,
    const unsigned char chainId[UMI_RECORD_CHAIN_ID_BYTES],
    uint64_t expectedRevision,
    UmiRecordChainCheckpoint *outCheckpoint)
{
    UmiRecordChainCheckpoint candidate;
    UmiRecordChainRecord receipt;
    char encoded[FINANCE_RECORD_CAPACITY];
    const FinanceState *state;
    UmiStatus status;

    if (operations == NULL || chainId == NULL || outCheckpoint == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (operations->poisoned || operations->writesBlocked ||
        operations->state == NULL)
        return UMI_STATUS_INVALID_STATE;
    state = operations->state;

    if (state->counts.events > UMI_FINANCE_OPERATIONS_EVENTS ||
        (uint64_t)state->counts.events != state->counts.revision)
        return UMI_STATUS_INVALID_STATE;
    if (state->counts.revision != expectedRevision)
        return UMI_STATUS_BUSY;

    status = umi_record_chain_start(chainId, &candidate);
    if (status != UMI_STATUS_OK) return status;

    for (size_t index = 0U; index < state->counts.events; ++index) {
        const UmiFinanceOperationCommand *command = &state->commands[index];
        size_t byteCount;
        uint32_t kind;
        if (command->expectedRevision != (uint64_t)index ||
            command->kind < UMI_FINANCE_CREATE_ACCOUNT ||
            command->kind > UMI_FINANCE_REOPEN_REVIEW)
            return UMI_STATUS_INVALID_STATE;
        status = FinanceEncode(command, encoded, sizeof encoded);
        if (status != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
        byteCount = strlen(encoded);
        if (byteCount == 0U || byteCount >= sizeof encoded)
            return UMI_STATUS_INVALID_STATE;
        kind = (uint32_t)command->kind;
        status = umi_record_chain_append(&candidate, kind,
                                         encoded, byteCount, &receipt);
        if (status != UMI_STATUS_OK) return status;
    }
    if (candidate.sequence != expectedRevision)
        return UMI_STATUS_INVALID_STATE;
    *outCheckpoint = candidate;
    return UMI_STATUS_OK;
}
