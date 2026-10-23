/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/training_release_approval.c
 *
 * PURPOSE:
 *   Strictly parse an independently reviewed ordered release identity and
 *   require exact agreement with fresh, real Ed25519 trust-batch verification.
 *
 * IMPLEMENTATION:
 *   Deliberately compose the existing Framework JSON verifier. The verified
 *   batch JSON is a trusted INTERNAL canonical representation, not a generic
 *   parser input. Extracting its fixed batch_sha256 field therefore neither
 *   reparses arbitrary external JSON nor duplicates cryptographic algorithms.
 *   Caller-owned outputs are committed only after full successful checking.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/training_release_approval.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UMI_RELEASE_APPROVAL_HEADER "UMICOM-AI-RELEASE-APPROVAL-1"
#define UMI_RELEASE_BATCH_HASH_FIELD "\"batch_sha256\":\""

static bool ParseUnsigned(const char *text, uint64_t *outValue)
{
    uint64_t value = UINT64_C(0);
    if (text == NULL || outValue == NULL || text[0] == '\0') return false;
    for (const char *position = text; *position != '\0'; ++position) {
        uint64_t digit;
        if (*position < '0' || *position > '9') return false;
        digit = (uint64_t)(unsigned)(*position - '0');
        if (value > (UINT64_MAX - digit) / UINT64_C(10)) return false;
        value = value * UINT64_C(10) + digit;
    }
    *outValue = value;
    return true;
}

static bool IsSha256Hex(const char *digest)
{
    if (digest == NULL) return false;
    for (size_t index = 0U; index < 64U; ++index) {
        unsigned char ch = (unsigned char)digest[index];
        if (!((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f')))
            return false;
    }
    return digest[64] == '\0';
}

static bool ValidApproval(const UmiAiTrainingReleaseApproval *approval)
{
    return approval != NULL && approval->publicationCount >= 1U &&
           approval->publicationCount <=
               UMI_AI_TRAINING_TRUST_BATCH_MAX_CHECKPOINTS &&
           IsSha256Hex(approval->batchSha256);
}

UmiStatus UmiAiTrainingReleaseApprovalParse(
    const char *text, size_t textLength,
    UmiAiTrainingReleaseApproval *outApproval)
{
    UmiAiTrainingReleaseApproval selected = {0};
    size_t cursor = 0U;
    uint32_t seen = 0U;
    bool headerSeen = false;

    if (text == NULL || outApproval == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (textLength == 0U) return UMI_STATUS_PARSE_ERROR;
    if (textLength > UMI_AI_TRAINING_RELEASE_APPROVAL_MAX_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    while (cursor < textLength) {
        char line[UMI_AI_TRAINING_RELEASE_APPROVAL_MAX_BYTES + 1U];
        size_t start = cursor;
        size_t length;
        char *separator;
        uint64_t value = 0U;
        uint32_t bit;

        while (cursor < textLength && text[cursor] != '\n') {
            if (text[cursor] == '\0') return UMI_STATUS_PARSE_ERROR;
            ++cursor;
        }
        length = cursor - start;
        if (cursor < textLength) ++cursor;
        if (length != 0U && text[start + length - 1U] == '\r') --length;
        if (length == 0U) continue;
        memcpy(line, text + start, length);
        line[length] = '\0';
        if (line[0] == '#') continue;
        if (!headerSeen) {
            if (strcmp(line, UMI_RELEASE_APPROVAL_HEADER) != 0)
                return UMI_STATUS_PARSE_ERROR;
            headerSeen = true;
            continue;
        }
        separator = strchr(line, '=');
        if (separator == NULL || separator == line || separator[1] == '\0')
            return UMI_STATUS_PARSE_ERROR;
        *separator = '\0';

        if (strcmp(line, "publication_count") == 0) {
            bit = UINT32_C(1);
            if ((seen & bit) != 0U ||
                !ParseUnsigned(separator + 1U, &value) ||
                value == 0U ||
                value > UMI_AI_TRAINING_TRUST_BATCH_MAX_CHECKPOINTS)
                return UMI_STATUS_PARSE_ERROR;
            selected.publicationCount = (uint32_t)value;
        } else if (strcmp(line, "evaluated_at_unix_seconds") == 0) {
            bit = UINT32_C(2);
            if ((seen & bit) != 0U ||
                !ParseUnsigned(separator + 1U, &value))
                return UMI_STATUS_PARSE_ERROR;
            selected.evaluatedAtUnixSeconds = value;
        } else if (strcmp(line, "batch_sha256") == 0) {
            bit = UINT32_C(4);
            if ((seen & bit) != 0U || strlen(separator + 1U) != 64U)
                return UMI_STATUS_PARSE_ERROR;
            memcpy(selected.batchSha256, separator + 1U, 65U);
            if (!IsSha256Hex(selected.batchSha256))
                return UMI_STATUS_PARSE_ERROR;
        } else {
            return UMI_STATUS_PARSE_ERROR;
        }
        seen |= bit;
    }
    if (!headerSeen || seen != UINT32_C(7) || !ValidApproval(&selected))
        return UMI_STATUS_PARSE_ERROR;
    *outApproval = selected;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingReleaseApprovalLoadFile(
    const char *approvalPath, UmiAiTrainingReleaseApproval *outApproval)
{
    char input[UMI_AI_TRAINING_RELEASE_APPROVAL_MAX_BYTES + 1U];
    FILE *stream;
    long measured;
    size_t length;
    int closeStatus;
    if (approvalPath == NULL || approvalPath[0] == '\0' ||
        outApproval == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    stream = fopen(approvalPath, "rb");
    if (stream == NULL) return UMI_STATUS_IO_ERROR;
    if (fseek(stream, 0L, SEEK_END) != 0 ||
        (measured = ftell(stream)) < 0L) {
        (void)fclose(stream);
        return UMI_STATUS_IO_ERROR;
    }
    if ((unsigned long)measured > UMI_AI_TRAINING_RELEASE_APPROVAL_MAX_BYTES) {
        (void)fclose(stream);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (fseek(stream, 0L, SEEK_SET) != 0) {
        (void)fclose(stream);
        return UMI_STATUS_IO_ERROR;
    }
    length = (size_t)measured;
    if (fread(input, 1U, length, stream) != length ||
        fgetc(stream) != EOF || ferror(stream) != 0) {
        (void)fclose(stream);
        return UMI_STATUS_IO_ERROR;
    }
    closeStatus = fclose(stream);
    if (closeStatus != 0) return UMI_STATUS_IO_ERROR;
    return UmiAiTrainingReleaseApprovalParse(input, length, outApproval);
}

UmiStatus UmiAiTrainingReleaseApprovalVerifyJson(
    const char *const *checkpointPaths,
    size_t checkpointCount,
    const char *independentlyApprovedPolicyFile,
    const UmiAiTrainingReleaseApproval *approval,
    char *outJson,
    size_t outputCapacity,
    size_t *outWritten)
{
    static const char APPROVED_TRAILER[] =
        ",\"release_approval\":{\"exact_ordered_batch_pinned\":true,"
        "\"approval_record_signed\":false}}";
    char *batchJson = NULL;
    const char *digest;
    size_t batchLength = 0U;
    size_t totalLength;
    UmiStatus status;

    if (checkpointPaths == NULL || independentlyApprovedPolicyFile == NULL ||
        outJson == NULL || outWritten == NULL || approval == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!ValidApproval(approval)) return UMI_STATUS_INVALID_ARGUMENT;
    if (checkpointCount != (size_t)approval->publicationCount)
        return UMI_STATUS_PERMISSION_DENIED;

    batchJson = malloc(UMI_AI_TRAINING_TRUST_BATCH_JSON_CAPACITY);
    if (batchJson == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiAiTrainingTrustBatchVerifyJson(
        checkpointPaths, checkpointCount, independentlyApprovedPolicyFile,
        approval->evaluatedAtUnixSeconds, batchJson,
        UMI_AI_TRAINING_TRUST_BATCH_JSON_CAPACITY, &batchLength);
    if (status != UMI_STATUS_OK) goto cleanup;

    if (batchLength < 2U ||
        batchLength >= UMI_AI_TRAINING_TRUST_BATCH_JSON_CAPACITY ||
        batchJson[batchLength] != '\0' || batchJson[batchLength - 1U] != '}' ||
        strncmp(batchJson, "{\"schema\":\"umicom.ai.training.trust-batch\"",
                sizeof("{\"schema\":\"umicom.ai.training.trust-batch\"") - 1U) != 0) {
        status = UMI_STATUS_INVALID_STATE;
        goto cleanup;
    }

    /* Only this Framework-generated canonical JSON is searched for the fixed
     * internal property. The property is unique by the emitting schema.
     * No external JSON, filenames or arbitrary user-provided strings are
     * interpreted. Approval and policy must be obtained independently. */
    digest = strstr(batchJson, UMI_RELEASE_BATCH_HASH_FIELD);
    if (digest == NULL ||
        strstr(digest + 1U, UMI_RELEASE_BATCH_HASH_FIELD) != NULL) {
        status = UMI_STATUS_INVALID_STATE;
        goto cleanup;
    }
    digest += sizeof(UMI_RELEASE_BATCH_HASH_FIELD) - 1U;
    if ((size_t)(batchJson + batchLength - digest) < 65U ||
        digest[64] != '"') {
        status = UMI_STATUS_INVALID_STATE;
        goto cleanup;
    }
    for (size_t i = 0U; i < 64U; ++i) {
        unsigned char ch = (unsigned char)digest[i];
        if (!((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f'))) {
            status = UMI_STATUS_INVALID_STATE;
            goto cleanup;
        }
    }
    if (memcmp(digest, approval->batchSha256, 64U) != 0) {
        status = UMI_STATUS_PERMISSION_DENIED;
        goto cleanup;
    }

    /* Replace only the final object brace. The appended field explicitly
     * indicates that the externally protected approval record was unsigned.
     * Verify all lengths BEFORE touching the caller's output buffer. */
    if (batchLength - 1U > SIZE_MAX - (sizeof(APPROVED_TRAILER) - 1U)) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto cleanup;
    }
    totalLength = batchLength - 1U + sizeof(APPROVED_TRAILER) - 1U;
    if (totalLength >= outputCapacity) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto cleanup;
    }
    memcpy(outJson, batchJson, batchLength - 1U);
    memcpy(outJson + batchLength - 1U,
           APPROVED_TRAILER, sizeof(APPROVED_TRAILER));
    *outWritten = totalLength;
    status = UMI_STATUS_OK;
cleanup:
    free(batchJson);
    return status;
}
