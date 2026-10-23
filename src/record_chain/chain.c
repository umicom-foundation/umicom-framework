/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/record_chain/chain.c
 * PURPOSE: Build and verify domain-separated commitments without partial state changes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <limits.h>
#include <string.h>

_Static_assert(CHAR_BIT == 8, "Record-chain interchange requires eight-bit bytes");
static const unsigned char GENESIS_DOMAIN[] = "UMICOM-RECORD-CHAIN-GENESIS:1\n";
static const unsigned char RECORD_DOMAIN[] = "UMICOM-RECORD-CHAIN-RECORD:1\n";

static bool IdentityValid(const unsigned char *id)
{
    unsigned char combined = 0U;
    if (id == NULL) return false;
    for (size_t i = 0U; i < UMI_RECORD_CHAIN_ID_BYTES; ++i) combined |= id[i];
    return combined != 0U;
}

void RecordChainWriteNumber(unsigned char *out, uint64_t value, size_t width)
{
    for (size_t i = 0U; i < width; ++i) {
        out[width - i - 1U] = (unsigned char)(value & UINT64_C(255));
        value >>= 8U;
    }
}

uint64_t RecordChainReadNumber(const unsigned char *bytes, size_t width)
{
    uint64_t value = 0U;
    for (size_t i = 0U; i < width; ++i) value = (value << 8U) | bytes[i];
    return value;
}

/* The same field sequence feeds hashing and archives. No ABI padding, pointer,
 * native integer byte order, locale or text terminator enters the commitment. */
void RecordChainFields(const UmiRecordChainRecord *record, unsigned char out[116])
{
    memcpy(out, record->chain_id, 32U);
    RecordChainWriteNumber(out + 32U, record->sequence, 8U);
    RecordChainWriteNumber(out + 40U, record->kind, 4U);
    RecordChainWriteNumber(out + 44U, record->payload_size, 8U);
    memcpy(out + 52U, record->previous_digest, 32U);
    memcpy(out + 84U, record->payload_digest, 32U);
}

static UmiStatus HashParts(const void *prefix, size_t prefix_size,
    const void *data, size_t size, unsigned char out[32])
{
    UmiSha256 hash;
    UmiStatus status;
    UmiSha256Init(&hash);
    status = UmiSha256Update(&hash, prefix, prefix_size);
    if (status == UMI_STATUS_OK) status = UmiSha256Update(&hash, data, size);
    if (status == UMI_STATUS_OK) status = UmiSha256Final(&hash, out);
    return status;
}

UmiStatus umi_record_chain_start(const unsigned char chain_id[32],
    UmiRecordChainCheckpoint *out)
{
    UmiRecordChainCheckpoint candidate = {0};
    UmiStatus status;
    if (out == NULL || !IdentityValid(chain_id)) return UMI_STATUS_INVALID_ARGUMENT;
    memcpy(candidate.chain_id, chain_id, 32U);
    status = HashParts(GENESIS_DOMAIN, sizeof GENESIS_DOMAIN - 1U,
        chain_id, 32U, candidate.digest);
    if (status == UMI_STATUS_OK) *out = candidate;
    return status;
}

UmiStatus RecordChainCheckpointValid(const UmiRecordChainCheckpoint *checkpoint)
{
    UmiRecordChainCheckpoint genesis;
    UmiStatus status;
    if (checkpoint == NULL || !IdentityValid(checkpoint->chain_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (checkpoint->sequence != 0U) return UMI_STATUS_OK;
    status = umi_record_chain_start(checkpoint->chain_id, &genesis);
    if (status != UMI_STATUS_OK) return status;
    return memcmp(genesis.digest, checkpoint->digest, 32U) == 0
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

UmiStatus RecordChainDigest(const UmiRecordChainRecord *record, unsigned char out[32])
{
    unsigned char fields[RECORD_CHAIN_FIELDS_BYTES];
    if (record == NULL || !IdentityValid(record->chain_id) || record->kind == 0U ||
        record->sequence == 0U || record->payload_size > UMI_RECORD_CHAIN_PAYLOAD_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    RecordChainFields(record, fields);
    return HashParts(RECORD_DOMAIN, sizeof RECORD_DOMAIN - 1U, fields, sizeof fields, out);
}

UmiStatus RecordChainRecordValid(const UmiRecordChainRecord *record)
{
    unsigned char digest[32];
    UmiStatus status = RecordChainDigest(record, digest);
    if (status != UMI_STATUS_OK) return status;
    return memcmp(digest, record->digest, sizeof digest) == 0
        ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}

UmiStatus umi_record_chain_append(UmiRecordChainCheckpoint *checkpoint,
    uint32_t kind, const void *payload, size_t size, UmiRecordChainRecord *out)
{
    UmiRecordChainRecord record = {0};
    UmiStatus status = RecordChainCheckpointValid(checkpoint);
    if (status != UMI_STATUS_OK) return status;
    if (out == NULL || kind == 0U || (payload == NULL && size != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (size > UMI_RECORD_CHAIN_PAYLOAD_LIMIT || checkpoint->sequence == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(record.chain_id, checkpoint->chain_id, 32U);
    record.sequence = checkpoint->sequence + 1U;
    record.kind = kind;
    record.payload_size = (uint64_t)size;
    memcpy(record.previous_digest, checkpoint->digest, 32U);
    status = HashParts(NULL, 0U, payload, size, record.payload_digest);
    if (status == UMI_STATUS_OK) status = RecordChainDigest(&record, record.digest);
    if (status != UMI_STATUS_OK) return status;
    /* Publishing both values last keeps invalid input and arithmetic exhaustion
     * from consuming a sequence number or exposing an incomplete receipt. */
    *out = record;
    checkpoint->sequence = record.sequence;
    memcpy(checkpoint->digest, record.digest, 32U);
    return UMI_STATUS_OK;
}

UmiStatus umi_record_chain_accept(UmiRecordChainCheckpoint *checkpoint,
    const UmiRecordChainRecord *record, const void *payload, size_t size)
{
    unsigned char digest[32];
    UmiStatus status = RecordChainCheckpointValid(checkpoint);
    if (status != UMI_STATUS_OK) return status;
    if (record == NULL || (payload == NULL && size != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    if (size > UMI_RECORD_CHAIN_PAYLOAD_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = RecordChainRecordValid(record);
    if (status != UMI_STATUS_OK) return status;
    if (checkpoint->sequence == UINT64_MAX || record->sequence != checkpoint->sequence + 1U ||
        memcmp(record->chain_id, checkpoint->chain_id, 32U) != 0 ||
        memcmp(record->previous_digest, checkpoint->digest, 32U) != 0)
        return UMI_STATUS_INVALID_STATE;
    if (record->payload_size != (uint64_t)size) return UMI_STATUS_PARSE_ERROR;
    status = HashParts(NULL, 0U, payload, size, digest);
    if (status != UMI_STATUS_OK) return status;
    if (memcmp(digest, record->payload_digest, 32U) != 0) return UMI_STATUS_PARSE_ERROR;
    checkpoint->sequence = record->sequence;
    memcpy(checkpoint->digest, record->digest, 32U);
    return UMI_STATUS_OK;
}

bool umi_record_chain_checkpoint_equal(const UmiRecordChainCheckpoint *left,
    const UmiRecordChainCheckpoint *right)
{
    return RecordChainCheckpointValid(left) == UMI_STATUS_OK &&
        RecordChainCheckpointValid(right) == UMI_STATUS_OK &&
        left->sequence == right->sequence &&
        memcmp(left->chain_id, right->chain_id, 32U) == 0 &&
        memcmp(left->digest, right->digest, 32U) == 0;
}

UmiStatus umi_record_chain_verify(UmiRecordChainCheckpoint *checkpoint,
    const UmiRecordChainInput *inputs, size_t count,
    const UmiRecordChainCheckpoint *expected_tip)
{
    UmiRecordChainCheckpoint candidate;
    UmiStatus status = RecordChainCheckpointValid(checkpoint);
    if (status != UMI_STATUS_OK) return status;
    status = RecordChainCheckpointValid(expected_tip);
    if (status != UMI_STATUS_OK) return status;
    if (inputs == NULL && count != 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_RECORD_CHAIN_BATCH_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    candidate = *checkpoint;
    for (size_t i = 0U; i < count; ++i) {
        status = umi_record_chain_accept(&candidate, inputs[i].record,
            inputs[i].payload, inputs[i].payload_size);
        if (status != UMI_STATUS_OK) return status;
    }
    if (!umi_record_chain_checkpoint_equal(&candidate, expected_tip)) return UMI_STATUS_INVALID_STATE;
    *checkpoint = candidate;
    return UMI_STATUS_OK;
}
