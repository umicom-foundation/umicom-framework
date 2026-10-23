/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/training_trust_batch.c
 *
 * PURPOSE:
 *   Qualify an ordered group of independently signed native AI training
 *   publications before issuing one bounded, all-or-nothing JSON decision.
 *
 * IMPLEMENTATION:
 *   Reuse the reviewed single-publication trust receipt service. SHA-256 only
 *   identifies the canonical bytes of this unsigned observation; it does not
 *   replace the Ed25519 verification or operator-chosen trust policy.
 *   Every result is staged privately, and caller-owned output is committed
 *   only after the final valid publication and complete JSON are available.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/training_trust_batch.h"
#include "umicom/base/sha256.h"

#include <inttypes.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UMI_BATCH_HASH_DOMAIN "UMICOM-AI-TRUST-BATCH-1"

typedef struct UmiBatchEntry {
    char receipt[UMI_AI_TRAINING_TRUST_RECEIPT_JSON_CAPACITY];
    char receiptSha256[UMI_SHA256_HEX_CAPACITY];
    size_t receiptLength;
} UmiBatchEntry;

/* Append staged JSON without truncation or integer overflow. All formatted
 * fields are canonical Framework-generated JSON, numeric values or fixed
 * lower-case digest strings; no user-supplied pathname enters the document. */
static UmiStatus AppendText(char *buffer, size_t capacity, size_t *used,
                            const char *text, size_t count)
{
    if (buffer == NULL || used == NULL || text == NULL || *used >= capacity ||
        count > capacity - 1U - *used)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(buffer + *used, text, count);
    *used += count;
    buffer[*used] = '\0';
    return UMI_STATUS_OK;
}

static UmiStatus AppendFormat(char *buffer, size_t capacity, size_t *used,
                              const char *format, ...)
{
    int length;
    va_list arguments;
    if (buffer == NULL || used == NULL || format == NULL || *used >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    va_start(arguments, format);
    length = vsnprintf(buffer + *used, capacity - *used, format, arguments);
    va_end(arguments);
    if (length < 0) return UMI_STATUS_INTERNAL_ERROR;
    if ((size_t)length >= capacity - *used) return UMI_STATUS_CAPACITY_EXCEEDED;
    *used += (size_t)length;
    return UMI_STATUS_OK;
}

/* Little-endian fixed-size framing prevents ambiguous concatenation. We do
 * not hash JSON text of the summary itself, which would be self-referential.
 * The domain, count and evaluation time separate this use from other hashes. */
static void Put64Le(uint8_t bytes[8], uint64_t number)
{
    for (unsigned index = 0U; index < 8U; ++index)
        bytes[index] = (uint8_t)(number >> (index * 8U));
}

static UmiStatus CalculateBatchDigest(
    const UmiBatchEntry *entries, size_t entryCount,
    uint64_t evaluationSeconds, char digestHex[UMI_SHA256_HEX_CAPACITY])
{
    UmiSha256 hash;
    unsigned char digest[UMI_SHA256_BYTES];
    uint8_t framed[8];
    UmiStatus status;
    UmiSha256Init(&hash);
    status = UmiSha256Update(&hash, UMI_BATCH_HASH_DOMAIN,
                            sizeof(UMI_BATCH_HASH_DOMAIN) - 1U);
    if (status != UMI_STATUS_OK) return status;
    Put64Le(framed, (uint64_t)entryCount);
    status = UmiSha256Update(&hash, framed, sizeof(framed));
    if (status != UMI_STATUS_OK) return status;
    Put64Le(framed, evaluationSeconds);
    status = UmiSha256Update(&hash, framed, sizeof(framed));
    if (status != UMI_STATUS_OK) return status;
    for (size_t index = 0U; index < entryCount; ++index) {
        Put64Le(framed, (uint64_t)entries[index].receiptLength);
        status = UmiSha256Update(&hash, framed, sizeof(framed));
        if (status != UMI_STATUS_OK) return status;
        status = UmiSha256Update(&hash, entries[index].receipt,
                                 entries[index].receiptLength);
        if (status != UMI_STATUS_OK) return status;
    }
    status = UmiSha256Final(&hash, digest);
    if (status != UMI_STATUS_OK) return status;
    UmiSha256Hex(digest, digestHex);
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingTrustBatchVerifyJson(
    const char *const *checkpointPaths, size_t checkpointCount,
    const char *independentlyApprovedPolicyFile,
    uint64_t verificationUnixSeconds,
    char *outJson, size_t outputCapacity, size_t *outWritten)
{
    UmiBatchEntry *entries = NULL;
    char *stagedJson = NULL;
    char batchSha256[UMI_SHA256_HEX_CAPACITY];
    size_t used = 0U;
    UmiStatus status = UMI_STATUS_OK;

    if (checkpointPaths == NULL || checkpointCount == 0U ||
        independentlyApprovedPolicyFile == NULL ||
        independentlyApprovedPolicyFile[0] == '\0' ||
        outJson == NULL || outWritten == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (checkpointCount > UMI_AI_TRAINING_TRUST_BATCH_MAX_CHECKPOINTS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t index = 0U; index < checkpointCount; ++index) {
        if (checkpointPaths[index] == NULL || checkpointPaths[index][0] == '\0')
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t previous = 0U; previous < index; ++previous)
            if (strcmp(checkpointPaths[previous], checkpointPaths[index]) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
    }

    entries = calloc(checkpointCount, sizeof(*entries));
    if (entries == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    stagedJson = malloc(UMI_AI_TRAINING_TRUST_BATCH_JSON_CAPACITY);
    if (stagedJson == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto cleanup; }
    stagedJson[0] = '\0';

    for (size_t index = 0U; index < checkpointCount; ++index) {
        UmiBatchEntry *entry = &entries[index];
        status = UmiAiTrainingTrustReceiptVerifyJson(
            checkpointPaths[index], independentlyApprovedPolicyFile,
            verificationUnixSeconds, entry->receipt, sizeof(entry->receipt),
            &entry->receiptLength);
        if (status != UMI_STATUS_OK) goto cleanup;
        if (entry->receiptLength == 0U ||
            entry->receiptLength >= sizeof(entry->receipt) ||
            entry->receipt[entry->receiptLength] != '\0') {
            status = UMI_STATUS_INVALID_STATE;
            goto cleanup;
        }
        status = UmiSha256Buffer(entry->receipt, entry->receiptLength,
                                 entry->receiptSha256);
        if (status != UMI_STATUS_OK) goto cleanup;
        /* Exact same verified evidence at the same clock instant is not an
         * independent second checkpoint, even if reachable by two paths. */
        for (size_t prior = 0U; prior < index; ++prior) {
            if (strcmp(entries[prior].receiptSha256,
                       entry->receiptSha256) == 0) {
                status = UMI_STATUS_ALREADY_EXISTS;
                goto cleanup;
            }
        }
    }
    status = CalculateBatchDigest(entries, checkpointCount,
                                  verificationUnixSeconds, batchSha256);
    if (status != UMI_STATUS_OK) goto cleanup;
    status = AppendFormat(stagedJson, UMI_AI_TRAINING_TRUST_BATCH_JSON_CAPACITY,
        &used,
        "{\"schema\":\"umicom.ai.training.trust-batch\","
        "\"result\":\"verified\",\"evaluated_at_unix_seconds\":%" PRIu64
        ",\"publication_count\":%zu,\"batch_sha256\":\"%s\","
        "\"entries\":[",
        verificationUnixSeconds, checkpointCount, batchSha256);
    if (status != UMI_STATUS_OK) goto cleanup;
    for (size_t index = 0U; index < checkpointCount; ++index) {
        const UmiBatchEntry *entry = &entries[index];
        if (index != 0U) {
            status = AppendText(stagedJson,
                UMI_AI_TRAINING_TRUST_BATCH_JSON_CAPACITY, &used, ",", 1U);
            if (status != UMI_STATUS_OK) goto cleanup;
        }
        status = AppendFormat(stagedJson,
            UMI_AI_TRAINING_TRUST_BATCH_JSON_CAPACITY, &used,
            "{\"index\":%zu,\"receipt_sha256\":\"%s\",\"receipt\":",
            index, entry->receiptSha256);
        if (status != UMI_STATUS_OK) goto cleanup;
        status = AppendText(stagedJson,
            UMI_AI_TRAINING_TRUST_BATCH_JSON_CAPACITY, &used,
            entry->receipt, entry->receiptLength);
        if (status != UMI_STATUS_OK) goto cleanup;
        status = AppendText(stagedJson,
            UMI_AI_TRAINING_TRUST_BATCH_JSON_CAPACITY, &used, "}", 1U);
        if (status != UMI_STATUS_OK) goto cleanup;
    }
    status = AppendText(stagedJson, UMI_AI_TRAINING_TRUST_BATCH_JSON_CAPACITY,
        &used,
        "],\"checks\":{\"all_publications_verified\":true,"
        "\"batch_record_signed\":false,"
        "\"trusted_signing_timestamp\":false,"
        "\"host_integrity_attested\":false}}",
        sizeof("],\"checks\":{\"all_publications_verified\":true,"
               "\"batch_record_signed\":false,"
               "\"trusted_signing_timestamp\":false,"
               "\"host_integrity_attested\":false}}") - 1U);
    if (status != UMI_STATUS_OK) goto cleanup;
    if (used >= outputCapacity) { status = UMI_STATUS_CAPACITY_EXCEEDED; goto cleanup; }
    memcpy(outJson, stagedJson, used + 1U);
    *outWritten = used;
cleanup:
    free(stagedJson);
    free(entries);
    return status;
}
