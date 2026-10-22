/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/gradient_policy.c
 *
 * PURPOSE:
 *   Serialize and verify a separately published AdamW clipping policy record
 *   against the precise combined checkpoint encoding and Framework SHA-256.
 *
 * LIFETIME:
 *   Inputs are borrowed; formatted outputs are published only on success.
 *   File creation is exclusive and never overwrites previous evidence.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/gradient_policy.h"
#include "umicom/base/sha256.h"

#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#else
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#define UMI_GRADIENT_POLICY_PREFIX "UMICOM-AI-ADAMW-CLIP-POLICY-1\n"
#define UMI_GRADIENT_POLICY_MAX_PATH 4096U

static bool ValidPolicy(const UmiAiTrainingGradientPolicy *policy)
{
    return policy != NULL &&
           isfinite(policy->maximumGradientNorm) &&
           policy->maximumGradientNorm >= 0.0F &&
           policy->batchSequences > 0U &&
           policy->microbatchSequences <= policy->batchSequences &&
           (policy->maximumGradientNorm == 0.0F ||
            policy->microbatchSequences > 0U);
}

/* No pointer dereference is performed when zero input size or an invalid
 * policy is presented. Explicitly canonicalise -0.0f as disabled instead
 * of recording two distinguishable representations of the same policy. */
UmiStatus UmiAiTrainingGradientPolicyFormat(
    const uint8_t *checkpointBytes, size_t checkpointByteCount,
    const UmiAiTrainingGradientPolicy *policy,
    char *outRecord, size_t capacity, size_t *outWritten)
{
    char identity[UMI_SHA256_HEX_CAPACITY];
    char staged[UMI_AI_GRADIENT_POLICY_RECORD_CAPACITY];
    uint32_t gradientBits;
    int written;
    UmiStatus status;
    if (checkpointBytes == NULL || checkpointByteCount == 0U ||
        checkpointByteCount > 8U * 1024U * 1024U ||
        !ValidPolicy(policy) || outRecord == NULL || outWritten == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiSha256Buffer(checkpointBytes, checkpointByteCount, identity);
    if (status != UMI_STATUS_OK) return status;
    memcpy(&gradientBits, &policy->maximumGradientNorm, sizeof(gradientBits));
    if (policy->maximumGradientNorm == 0.0F) gradientBits = UINT32_C(0);
    written = snprintf(staged, sizeof(staged),
                       UMI_GRADIENT_POLICY_PREFIX
                       "maximum_gradient_norm_bits=%08" PRIx32 "\n"
                       "batch_sequences=%" PRIu32 "\n"
                       "microbatch_sequences=%" PRIu32 "\n"
                       "checkpoint_sha256=%s\n",
                       gradientBits, policy->batchSequences,
                       policy->microbatchSequences, identity);
    if (written <= 0 || (size_t)written >= sizeof(staged))
        return UMI_STATUS_INTERNAL_ERROR;
    if ((size_t)written + 1U > capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(outRecord, staged, (size_t)written + 1U);
    *outWritten = (size_t)written;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingGradientPolicyVerify(
    const uint8_t *checkpointBytes, size_t checkpointByteCount,
    const UmiAiTrainingGradientPolicy *policy,
    const char *record, size_t recordLength)
{
    char expected[UMI_AI_GRADIENT_POLICY_RECORD_CAPACITY];
    size_t expectedLength = 0U;
    UmiStatus status;
    if (record == NULL || recordLength == 0U ||
        recordLength >= UMI_AI_GRADIENT_POLICY_RECORD_CAPACITY)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingGradientPolicyFormat(
        checkpointBytes, checkpointByteCount, policy,
        expected, sizeof(expected), &expectedLength);
    if (status != UMI_STATUS_OK) return status;
    return recordLength == expectedLength &&
           memcmp(record, expected, expectedLength) == 0 ?
           UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}

UmiStatus UmiAiTrainingGradientPolicySaveNewFile(
    const char *recordPath,
    const uint8_t *checkpointBytes, size_t checkpointByteCount,
    const UmiAiTrainingGradientPolicy *policy)
{
    char record[UMI_AI_GRADIENT_POLICY_RECORD_CAPACITY];
    size_t count = 0U, position = 0U;
    int descriptor = -1;
    UmiStatus status;
    if (recordPath == NULL || recordPath[0] == '\0' ||
        strlen(recordPath) >= UMI_GRADIENT_POLICY_MAX_PATH)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingGradientPolicyFormat(
        checkpointBytes, checkpointByteCount, policy,
        record, sizeof(record), &count);
    if (status != UMI_STATUS_OK) return status;
#if defined(_WIN32)
    descriptor = _open(recordPath, _O_CREAT | _O_EXCL | _O_WRONLY | _O_BINARY,
                       _S_IREAD | _S_IWRITE);
#else
    descriptor = open(recordPath, O_CREAT | O_EXCL | O_WRONLY, 0600);
#endif
    if (descriptor < 0)
        return errno == EEXIST ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_IO_ERROR;
    while (position < count) {
#if defined(_WIN32)
        int amount = _write(descriptor, record + position,
                            (unsigned int)(count - position));
#else
        ssize_t amount = write(descriptor, record + position, count - position);
#endif
        if (amount <= 0) { status = UMI_STATUS_IO_ERROR; goto remove_partial; }
        position += (size_t)amount;
    }
#if defined(_WIN32)
    int syncResult = _commit(descriptor);
    int closeResult = _close(descriptor);
#else
    int syncResult = fsync(descriptor);
    int closeResult = close(descriptor);
#endif
    descriptor = -1;
    if (syncResult == 0 && closeResult == 0) return UMI_STATUS_OK;
    status = UMI_STATUS_IO_ERROR;
    goto remove_partial;
remove_partial:
    if (descriptor >= 0) {
#if defined(_WIN32)
        (void)_close(descriptor);
#else
        (void)close(descriptor);
#endif
    }
    (void)remove(recordPath);
    return status;
}

UmiStatus UmiAiTrainingGradientPolicyVerifyFile(
    const char *recordPath,
    const uint8_t *checkpointBytes, size_t checkpointByteCount,
    const UmiAiTrainingGradientPolicy *policy)
{
    char record[UMI_AI_GRADIENT_POLICY_RECORD_CAPACITY];
    FILE *stream;
    size_t length;
    int extra, ioError;
    if (recordPath == NULL || recordPath[0] == '\0' ||
        strlen(recordPath) >= UMI_GRADIENT_POLICY_MAX_PATH)
        return UMI_STATUS_INVALID_ARGUMENT;
    stream = fopen(recordPath, "rb");
    if (stream == NULL) return UMI_STATUS_IO_ERROR;
    length = fread(record, 1U, sizeof(record), stream);
    extra = fgetc(stream);
    ioError = ferror(stream);
    if (fclose(stream) != 0) ioError = 1;
    if (ioError) return UMI_STATUS_IO_ERROR;
    if (length == sizeof(record) || extra != EOF)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    return UmiAiTrainingGradientPolicyVerify(
        checkpointBytes, checkpointByteCount, policy, record, length);
}
