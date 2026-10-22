/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/training_run_lock.c
 *
 * PURPOSE:
 *   Construct deterministic, failure-atomic policy and source-snapshot
 *   identity for exact AdamW training continuation, entirely in native C23.
 *
 * ENGINEERING NOTES:
 *   Use the Framework SHA-256 engine and the existing, immutable dataset
 *   document catalogue; do not reopen arbitrary source paths in a second
 *   importer. The latter publishes FNV64 byte fingerprints, so an SHA-256
 *   digest over the catalogue is a change detector, NOT proof against a
 *   deliberately chosen FNV collision. The separate checkpoint hash covers
 *   caller-supplied raw bytes, not a re-encoded replacement.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/training_run_lock.h"
#include "umicom/ai/training/optimizer_checkpoint.h"
#include "umicom/base/sha256.h"

#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
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

#define UMI_RUN_LOCK_PREFIX "UMICOM-AI-ADAMW-RUN-LOCK-1\n"
#define UMI_RUN_LOCK_DATASET_DOMAIN "UMICOM-AI-DATASET-SNAPSHOT-1\n"
#define UMI_RUN_LOCK_POLICY_DOMAIN "UMICOM-AI-ADAMW-RUN-SETTINGS-1\n"
#define UMI_RUN_LOCK_PATH_MAX 4096U

static UmiStatus Add(UmiSha256 *hash, const void *bytes, size_t count)
{
    return UmiSha256Update(hash, bytes, count);
}

static UmiStatus Add32(UmiSha256 *hash, uint32_t value)
{
    unsigned char bytes[4];
    for (size_t i = 0U; i < sizeof(bytes); ++i)
        bytes[i] = (unsigned char)(value >> (8U * (unsigned)i));
    return Add(hash, bytes, sizeof(bytes));
}

static UmiStatus Add64(UmiSha256 *hash, uint64_t value)
{
    unsigned char bytes[8];
    for (size_t i = 0U; i < sizeof(bytes); ++i)
        bytes[i] = (unsigned char)(value >> (8U * (unsigned)i));
    return Add(hash, bytes, sizeof(bytes));
}

static UmiStatus AddFloat(UmiSha256 *hash, float value)
{
    uint32_t bits;
    /* The new policy service requires IEEE binary32. Its exact bit pattern is
     * hashed rather than an implementation-defined decimal representation. */
    _Static_assert(sizeof(float) == sizeof(uint32_t),
                   "Native AdamW run locks require 32-bit float");
    memcpy(&bits, &value, sizeof(bits));
    if (value == 0.0F) bits = UINT32_C(0); /* canonical signed zero */
    return Add32(hash, bits);
}

static UmiStatus Finish(UmiSha256 *hash, char outHex[UMI_SHA256_HEX_CAPACITY])
{
    unsigned char digest[UMI_SHA256_BYTES];
    UmiStatus status = UmiSha256Final(hash, digest);
    if (status == UMI_STATUS_OK) UmiSha256Hex(digest, outHex);
    return status;
}

/* Digest ordered DOCUMENT METADATA of the immutable corpus snapshot. The
 * original corpus bytes are not exposed by DatasetDocument; its FNV64 value
 * is a non-cryptographic source fingerprint. Keep that security limit clear. */
static UmiStatus SnapshotDigest(const UmiAiTrainingDataset *dataset,
                                char outHex[UMI_SHA256_HEX_CAPACITY])
{
    UmiSha256 hash;
    UmiAiTrainingDatasetSummary summary;
    UmiStatus status;
    uint64_t trainCount = 0U, evalCount = 0U;
    uint64_t trainBytes = 0U, evalBytes = 0U;
    if (dataset == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingDatasetInspect(dataset, &summary);
    if (status != UMI_STATUS_OK) return status;
    if (summary.documentCount == 0U ||
        summary.documentCount > UMI_AI_DATASET_MAX_FILES ||
        summary.trainingDocuments == 0U || summary.evaluationDocuments == 0U)
        return UMI_STATUS_INVALID_STATE;
    UmiSha256Init(&hash);
    status = Add(&hash, UMI_RUN_LOCK_DATASET_DOMAIN,
                 sizeof(UMI_RUN_LOCK_DATASET_DOMAIN) - 1U);
    if (status != UMI_STATUS_OK) return status;
    status = Add64(&hash, (uint64_t)summary.documentCount);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < summary.documentCount; ++i) {
        UmiAiTrainingDocumentInfo info;
        const char *end;
        size_t length;
        status = UmiAiTrainingDatasetDocument(dataset, i, &info);
        if (status != UMI_STATUS_OK) return status;
        end = memchr(info.relativePath, '\0', sizeof(info.relativePath));
        if (end == NULL) return UMI_STATUS_INVALID_STATE;
        length = (size_t)(end - info.relativePath);
        if (length == 0U || info.byteCount == 0U) return UMI_STATUS_INVALID_STATE;
        if (info.partition == UMI_AI_TRAINING_PARTITION_TRAIN) {
            trainCount++;
            trainBytes += (uint64_t)info.byteCount;
        } else if (info.partition == UMI_AI_TRAINING_PARTITION_EVALUATE) {
            evalCount++;
            evalBytes += (uint64_t)info.byteCount;
        } else return UMI_STATUS_INVALID_STATE;
        status = Add32(&hash, (uint32_t)info.partition);
        if (status != UMI_STATUS_OK) return status;
        status = Add64(&hash, (uint64_t)length);
        if (status != UMI_STATUS_OK) return status;
        status = Add(&hash, info.relativePath, length);
        if (status != UMI_STATUS_OK) return status;
        status = Add64(&hash, (uint64_t)info.byteCount);
        if (status != UMI_STATUS_OK) return status;
        status = Add64(&hash, info.fingerprint);
        if (status != UMI_STATUS_OK) return status;
    }
    if (trainCount != (uint64_t)summary.trainingDocuments ||
        evalCount != (uint64_t)summary.evaluationDocuments ||
        trainBytes != (uint64_t)summary.trainingBytes ||
        evalBytes != (uint64_t)summary.evaluationBytes)
        return UMI_STATUS_INVALID_STATE;
    return Finish(&hash, outHex);
}

/* Commit all settings which alter training trajectory, and also evaluation
 * settings for an interpretable run record. The requested END step is omitted
 * ONLY to allow planned intermediate publication and exact continuation. */
static UmiStatus SettingsDigest(const UmiAiTrainingRunLockInputs *inputs,
                                char outHex[UMI_SHA256_HEX_CAPACITY])
{
    const UmiAiTrainingExperimentConfig *e = inputs->experiment;
    const UmiAiTrainingAdamWConfig *a = inputs->adamw;
    const UmiAiTrainingLearningRateSchedule *s = inputs->schedule;
    const UmiAiTrainingGradientPolicy *c = inputs->clipping;
    UmiSha256 hash;
    UmiStatus status;
    if (e == NULL || a == NULL || s == NULL || c == NULL ||
        !isfinite(c->maximumGradientNorm) || c->maximumGradientNorm <= 0.0F ||
        c->batchSequences != e->batchSequences ||
        c->microbatchSequences == 0U ||
        c->microbatchSequences > c->batchSequences)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingExperimentAdamWValidate(e, a, s);
    if (status != UMI_STATUS_OK) return status;
    UmiSha256Init(&hash);
    status = Add(&hash, UMI_RUN_LOCK_POLICY_DOMAIN,
                 sizeof(UMI_RUN_LOCK_POLICY_DOMAIN) - 1U);
    if (status != UMI_STATUS_OK) return status;
    const uint32_t ints[] = {
        e->decoder.vocabularySize,
        e->decoder.contextLength,
        e->decoder.channels,
        e->decoder.hiddenChannels,
        e->decoder.attentionHeads,
        e->evalInterval, e->evalBatches, e->batchSequences,
        (uint32_t)s->kind, c->batchSequences, c->microbatchSequences
    };
    /* A stable named sequence avoids compiler-dependent struct padding. */
    for (size_t i = 0U; i < sizeof(ints) / sizeof(ints[0]); ++i) {
        status = Add32(&hash, ints[i]);
        if (status != UMI_STATUS_OK) return status;
    }
    const uint64_t longs[] = {
        e->decoder.initialSeed, e->trainSeed, e->evalSeed,
        s->warmupSteps, s->totalSteps
    };
    for (size_t i = 0U; i < sizeof(longs) / sizeof(longs[0]); ++i) {
        status = Add64(&hash, longs[i]);
        if (status != UMI_STATUS_OK) return status;
    }
    const float floats[] = {
        e->learningRate, a->beta1, a->beta2, a->epsilon, a->weightDecay,
        s->peakLearningRate, s->minimumLearningRate, c->maximumGradientNorm
    };
    for (size_t i = 0U; i < sizeof(floats) / sizeof(floats[0]); ++i) {
        status = AddFloat(&hash, floats[i]);
        if (status != UMI_STATUS_OK) return status;
    }
    return Finish(&hash, outHex);
}

UmiStatus UmiAiTrainingRunLockFormat(
    const uint8_t *checkpointBytes, size_t checkpointByteCount,
    const UmiAiTrainingRunLockInputs *inputs,
    char *outRecord, size_t capacity, size_t *outWritten)
{
    char checkpointHex[UMI_SHA256_HEX_CAPACITY];
    char snapshotHex[UMI_SHA256_HEX_CAPACITY];
    char settingsHex[UMI_SHA256_HEX_CAPACITY];
    char staged[UMI_AI_TRAINING_RUN_LOCK_RECORD_CAPACITY];
    UmiStatus status;
    int written;
    if (checkpointBytes == NULL || checkpointByteCount == 0U ||
        checkpointByteCount > UMI_AI_ADAMW_MAX_CHECKPOINT_BYTES ||
        inputs == NULL || inputs->dataset == NULL ||
        outRecord == NULL || outWritten == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = SettingsDigest(inputs, settingsHex);
    if (status != UMI_STATUS_OK) return status;
    status = SnapshotDigest(inputs->dataset, snapshotHex);
    if (status != UMI_STATUS_OK) return status;
    status = UmiSha256Buffer(checkpointBytes, checkpointByteCount,
                             checkpointHex);
    if (status != UMI_STATUS_OK) return status;
    written = snprintf(staged, sizeof(staged), UMI_RUN_LOCK_PREFIX
                       "checkpoint_sha256=%s\n"
                       "dataset_snapshot_sha256=%s\n"
                       "run_settings_sha256=%s\n",
                       checkpointHex, snapshotHex, settingsHex);
    if (written <= 0 || (size_t)written >= sizeof(staged))
        return UMI_STATUS_INTERNAL_ERROR;
    if ((size_t)written + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(outRecord, staged, (size_t)written + 1U);
    *outWritten = (size_t)written;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingRunLockVerify(
    const uint8_t *checkpointBytes, size_t checkpointByteCount,
    const UmiAiTrainingRunLockInputs *inputs,
    const char *record, size_t recordLength)
{
    char expected[UMI_AI_TRAINING_RUN_LOCK_RECORD_CAPACITY];
    size_t written = 0U;
    UmiStatus status;
    if (record == NULL || recordLength == 0U ||
        recordLength >= UMI_AI_TRAINING_RUN_LOCK_RECORD_CAPACITY)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingRunLockFormat(
        checkpointBytes, checkpointByteCount, inputs,
        expected, sizeof(expected), &written);
    if (status != UMI_STATUS_OK) return status;
    return recordLength == written &&
           memcmp(record, expected, written) == 0 ?
           UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}

/* Test a path's length without reading indefinitely from a malformed input. */
static bool ValidPath(const char *path)
{
    if (path == NULL) return false;
    for (size_t i = 0U; i < UMI_RUN_LOCK_PATH_MAX; ++i) {
        if (path[i] == '\0') return i != 0U;
    }
    return false;
}

UmiStatus UmiAiTrainingRunLockSaveNewFile(
    const char *recordPath,
    const uint8_t *checkpointBytes, size_t checkpointByteCount,
    const UmiAiTrainingRunLockInputs *inputs)
{
    char record[UMI_AI_TRAINING_RUN_LOCK_RECORD_CAPACITY];
    size_t written = 0U, position = 0U;
    int descriptor;
    UmiStatus status;
    if (!ValidPath(recordPath)) return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingRunLockFormat(
        checkpointBytes, checkpointByteCount, inputs,
        record, sizeof(record), &written);
    if (status != UMI_STATUS_OK) return status;
#if defined(_WIN32)
    descriptor = _open(recordPath, _O_CREAT | _O_EXCL | _O_WRONLY | _O_BINARY,
                       _S_IREAD | _S_IWRITE);
#else
    descriptor = open(recordPath, O_CREAT | O_EXCL | O_WRONLY, 0600);
#endif
    if (descriptor < 0)
        return errno == EEXIST ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_IO_ERROR;
    while (position < written) {
#if defined(_WIN32)
        int amount = _write(descriptor, record + position,
                            (unsigned int)(written - position));
#else
        ssize_t amount = write(descriptor, record + position,
                               written - position);
#endif
        if (amount <= 0) { status = UMI_STATUS_IO_ERROR; goto remove_new; }
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
    goto remove_new;
remove_new:
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

UmiStatus UmiAiTrainingRunLockVerifyFile(
    const char *recordPath,
    const uint8_t *checkpointBytes, size_t checkpointByteCount,
    const UmiAiTrainingRunLockInputs *inputs)
{
    char record[UMI_AI_TRAINING_RUN_LOCK_RECORD_CAPACITY];
    size_t readCount;
    int extra, readError;
    FILE *stream;
    if (!ValidPath(recordPath)) return UMI_STATUS_INVALID_ARGUMENT;
    stream = fopen(recordPath, "rb");
    if (stream == NULL) return UMI_STATUS_IO_ERROR;
    readCount = fread(record, 1U, sizeof(record), stream);
    extra = fgetc(stream);
    readError = ferror(stream);
    if (fclose(stream) != 0) readError = 1;
    if (readError) return UMI_STATUS_IO_ERROR;
    if (readCount == sizeof(record) || extra != EOF)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    return UmiAiTrainingRunLockVerify(
        checkpointBytes, checkpointByteCount, inputs, record, readCount);
}
