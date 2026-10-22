/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/training_source_lock.c
 *
 * PURPOSE:
 *   Compare cryptographic identity of already-imported source snapshots and
 *   canonical combined AdamW checkpoint bytes, without a second data reader.
 *
 * OWNERSHIP AND TRUST:
 *   The corpus and SHA-256 algorithm belong to their existing Framework
 *   services. This component neither owns the dataset nor trusts the host.
 *   Its one-file publication is exclusive; the multi-file checkpoint bundle
 *   still requires independently verified companions on every strict resume.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/training_source_lock.h"
#include "umicom/ai/training/optimizer_checkpoint.h"
#include "umicom/base/sha256.h"

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
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

#define UMI_SOURCE_LOCK_MAGIC "UMICOM-AI-SOURCE-LOCK-1\n"
#define UMI_SOURCE_LOCK_PATH_MAX 4096U

static bool ValidPath(const char *path)
{
    if (path == NULL) return false;
    for (size_t i = 0U; i < UMI_SOURCE_LOCK_PATH_MAX; ++i) {
        if (path[i] == '\0') return i != 0U;
    }
    return false;
}

UmiStatus UmiAiTrainingSourceLockFormat(
    const uint8_t *checkpointBytes, size_t checkpointByteCount,
    const UmiAiTrainingDataset *dataset,
    char *outRecord, size_t capacity, size_t *outWritten)
{
    unsigned char digest[UMI_AI_DATASET_SOURCE_SHA256_BYTES];
    char datasetHex[UMI_SHA256_HEX_CAPACITY];
    char checkpointHex[UMI_SHA256_HEX_CAPACITY];
    char staged[UMI_AI_TRAINING_SOURCE_LOCK_RECORD_CAPACITY];
    UmiStatus status;
    int count;
    if (checkpointBytes == NULL || checkpointByteCount == 0U ||
        checkpointByteCount > UMI_AI_ADAMW_MAX_CHECKPOINT_BYTES ||
        dataset == NULL || outRecord == NULL || outWritten == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingDatasetSourceSha256(dataset, digest);
    if (status != UMI_STATUS_OK) return status;
    UmiSha256Hex(digest, datasetHex);
    status = UmiSha256Buffer(checkpointBytes, checkpointByteCount,
                             checkpointHex);
    if (status != UMI_STATUS_OK) return status;
    count = snprintf(staged, sizeof(staged), UMI_SOURCE_LOCK_MAGIC
                     "checkpoint_sha256=%s\n"
                     "dataset_source_sha256=%s\n", checkpointHex, datasetHex);
    if (count <= 0 || (size_t)count >= sizeof(staged))
        return UMI_STATUS_INTERNAL_ERROR;
    if ((size_t)count + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(outRecord, staged, (size_t)count + 1U);
    *outWritten = (size_t)count;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingSourceLockVerify(
    const uint8_t *checkpointBytes, size_t checkpointByteCount,
    const UmiAiTrainingDataset *dataset,
    const char *record, size_t recordLength)
{
    char expected[UMI_AI_TRAINING_SOURCE_LOCK_RECORD_CAPACITY];
    size_t expectedCount = 0U;
    UmiStatus status;
    if (record == NULL || recordLength == 0U ||
        recordLength >= UMI_AI_TRAINING_SOURCE_LOCK_RECORD_CAPACITY)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingSourceLockFormat(
        checkpointBytes, checkpointByteCount, dataset,
        expected, sizeof(expected), &expectedCount);
    if (status != UMI_STATUS_OK) return status;
    return recordLength == expectedCount &&
        memcmp(record, expected, expectedCount) == 0 ?
        UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}

UmiStatus UmiAiTrainingSourceLockSaveNewFile(
    const char *recordPath,
    const uint8_t *checkpointBytes, size_t checkpointByteCount,
    const UmiAiTrainingDataset *dataset)
{
    char record[UMI_AI_TRAINING_SOURCE_LOCK_RECORD_CAPACITY];
    size_t written = 0U, position = 0U;
    int descriptor;
    UmiStatus status;
    if (!ValidPath(recordPath)) return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingSourceLockFormat(checkpointBytes, checkpointByteCount,
        dataset, record, sizeof(record), &written);
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
        ssize_t amount = write(descriptor, record + position, written - position);
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

UmiStatus UmiAiTrainingSourceLockVerifyFile(
    const char *recordPath,
    const uint8_t *checkpointBytes, size_t checkpointByteCount,
    const UmiAiTrainingDataset *dataset)
{
    char record[UMI_AI_TRAINING_SOURCE_LOCK_RECORD_CAPACITY];
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
    return UmiAiTrainingSourceLockVerify(
        checkpointBytes, checkpointByteCount, dataset, record, readCount);
}
