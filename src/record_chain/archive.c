/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/record_chain/archive.c
 * PURPOSE: Encode canonical checkpoint and receipt fields and reject malformed envelopes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <string.h>
static const unsigned char CHECKPOINT_MAGIC[8] = {'U','R','C','P',0,0,0,1};
static const unsigned char RECORD_MAGIC[8] = {'U','R','C','R',0,0,0,1};

UmiStatus umi_record_chain_checkpoint_encode(const UmiRecordChainCheckpoint *value,
    void *bytes, size_t capacity)
{
    unsigned char candidate[UMI_RECORD_CHAIN_CHECKPOINT_BYTES];
    UmiStatus status = RecordChainCheckpointValid(value);
    if (status != UMI_STATUS_OK) return status;
    if (bytes == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < sizeof candidate) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(candidate, CHECKPOINT_MAGIC, 8U);
    memcpy(candidate + 8U, value->chain_id, 32U);
    RecordChainWriteNumber(candidate + 40U, value->sequence, 8U);
    memcpy(candidate + 48U, value->digest, 32U);
    memcpy(bytes, candidate, sizeof candidate);
    return UMI_STATUS_OK;
}

UmiStatus umi_record_chain_checkpoint_decode(const void *bytes, size_t size,
    UmiRecordChainCheckpoint *out)
{
    const unsigned char *input = bytes;
    UmiRecordChainCheckpoint candidate = {0};
    if (input == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (size != UMI_RECORD_CHAIN_CHECKPOINT_BYTES || memcmp(input, CHECKPOINT_MAGIC, 8U) != 0)
        return UMI_STATUS_PARSE_ERROR;
    memcpy(candidate.chain_id, input + 8U, 32U);
    candidate.sequence = RecordChainReadNumber(input + 40U, 8U);
    memcpy(candidate.digest, input + 48U, 32U);
    if (RecordChainCheckpointValid(&candidate) != UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
    *out = candidate;
    return UMI_STATUS_OK;
}

UmiStatus umi_record_chain_record_encode(const UmiRecordChainRecord *value,
    void *bytes, size_t capacity)
{
    unsigned char candidate[UMI_RECORD_CHAIN_RECORD_BYTES];
    UmiStatus status = RecordChainRecordValid(value);
    if (status != UMI_STATUS_OK) return status;
    if (bytes == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < sizeof candidate) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(candidate, RECORD_MAGIC, 8U);
    RecordChainFields(value, candidate + 8U);
    memcpy(candidate + 124U, value->digest, 32U);
    memcpy(bytes, candidate, sizeof candidate);
    return UMI_STATUS_OK;
}

UmiStatus umi_record_chain_record_decode(const void *bytes, size_t size,
    UmiRecordChainRecord *out)
{
    const unsigned char *input = bytes;
    UmiRecordChainRecord candidate = {0};
    if (input == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (size != UMI_RECORD_CHAIN_RECORD_BYTES || memcmp(input, RECORD_MAGIC, 8U) != 0)
        return UMI_STATUS_PARSE_ERROR;
    memcpy(candidate.chain_id, input + 8U, 32U);
    candidate.sequence = RecordChainReadNumber(input + 40U, 8U);
    candidate.kind = (uint32_t)RecordChainReadNumber(input + 48U, 4U);
    candidate.payload_size = RecordChainReadNumber(input + 52U, 8U);
    memcpy(candidate.previous_digest, input + 60U, 32U);
    memcpy(candidate.payload_digest, input + 92U, 32U);
    memcpy(candidate.digest, input + 124U, 32U);
    if (RecordChainRecordValid(&candidate) != UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
    *out = candidate;
    return UMI_STATUS_OK;
}
