/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/training_publication.c
 *
 * PURPOSE:
 *   Stream SHA-256 over a completed native training checkpoint bundle and
 *   publish an opt-in, verify-on-read completion marker without scripts.
 *
 * SAFETY AND OWNERSHIP:
 *   File identities are computed in bounded stack storage; no unbounded
 *   allocation and no caller data are retained. The checkpoint and sidecars
 *   are NEVER modified. Publication creates a fresh same-directory staging
 *   file and links/moves it to an unoccupied final path. Incomplete staging
 *   may remain after an unexpected process crash, requiring human review.
 *   The checksum is NOT a signature or an authenticity guarantee.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/training_publication.h"
#include "umicom/ai/training/optimizer_checkpoint.h"

#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

#define UMI_PUBLICATION_MAGIC "UMICOM-AI-TRAINING-PUBLICATION-1\n"
#define UMI_PUBLICATION_SUFFIX ".publication"
#define UMI_PUBLICATION_STAGE_SUFFIX ".partial"
#define UMI_PUBLICATION_CLIP_SUFFIX ".clip-policy"
#define UMI_PUBLICATION_RUN_SUFFIX ".training-lock"
#define UMI_PUBLICATION_SOURCE_SUFFIX ".source-lock"

/* Size bound leaves room for the longest fixed companion and staging suffix.
 * The path is caller-approved, NOT checked for filesystem traversal. */
static UmiStatus MakePath(const char *checkpointPath, const char *suffix,
                          char outPath[UMI_AI_TRAINING_PUBLICATION_MAX_PATH])
{
    size_t prefixLength, suffixLength;
    if (checkpointPath == NULL || suffix == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    prefixLength = strlen(checkpointPath);
    suffixLength = strlen(suffix);
    if (prefixLength == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (prefixLength >= UMI_AI_TRAINING_PUBLICATION_MAX_PATH ||
        suffixLength >= UMI_AI_TRAINING_PUBLICATION_MAX_PATH - prefixLength)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(outPath, checkpointPath, prefixLength);
    memcpy(outPath + prefixLength, suffix, suffixLength + 1U);
    return UMI_STATUS_OK;
}

/* Exactly one forward pass through each selected file. No mmap, no second
 * source-dataset reader and no memory proportional to the checkpoint size. */
static UmiStatus HashFile(const char *path, uint64_t maximumBytes,
                          UmiAiTrainingPublicationFileIdentity *outIdentity)
{
    unsigned char buffer[4096U], digest[UMI_SHA256_BYTES];
    UmiAiTrainingPublicationFileIdentity candidate = {0};
    UmiSha256 state;
    FILE *file;
    UmiStatus status = UMI_STATUS_OK;
    if (path == NULL || outIdentity == NULL || maximumBytes == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    file = fopen(path, "rb");
    if (file == NULL) return UMI_STATUS_IO_ERROR;
    UmiSha256Init(&state);
    for (;;) {
        size_t readCount = fread(buffer, 1U, sizeof(buffer), file);
        if ((uint64_t)readCount > maximumBytes - candidate.byteCount) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        if (readCount > 0U) {
            status = UmiSha256Update(&state, buffer, readCount);
            if (status != UMI_STATUS_OK) break;
            candidate.byteCount += (uint64_t)readCount;
        }
        if (readCount < sizeof(buffer)) {
            if (ferror(file) != 0) status = UMI_STATUS_IO_ERROR;
            break;
        }
    }
    if (fclose(file) != 0 && status == UMI_STATUS_OK)
        status = UMI_STATUS_IO_ERROR;
    if (status != UMI_STATUS_OK) return status;
    if (candidate.byteCount == 0U) return UMI_STATUS_PARSE_ERROR;
    status = UmiSha256Final(&state, digest);
    if (status != UMI_STATUS_OK) return status;
    UmiSha256Hex(digest, candidate.sha256);
    *outIdentity = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingPublicationInspect(
    const char *checkpointPath,
    UmiAiTrainingPublicationEvidence *outEvidence)
{
    UmiAiTrainingPublicationEvidence candidate = {0};
    char path[UMI_AI_TRAINING_PUBLICATION_MAX_PATH];
    UmiStatus status;
    if (checkpointPath == NULL || outEvidence == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = MakePath(checkpointPath, "", path);
    if (status != UMI_STATUS_OK) return status;
    status = HashFile(path, (uint64_t)UMI_AI_ADAMW_MAX_CHECKPOINT_BYTES,
                      &candidate.checkpoint);
    if (status != UMI_STATUS_OK) return status;
    status = MakePath(checkpointPath, UMI_PUBLICATION_CLIP_SUFFIX, path);
    if (status != UMI_STATUS_OK) return status;
    status = HashFile(path, UMI_AI_TRAINING_PUBLICATION_MAX_COMPANION_BYTES,
                      &candidate.clipPolicy);
    if (status != UMI_STATUS_OK) return status;
    status = MakePath(checkpointPath, UMI_PUBLICATION_RUN_SUFFIX, path);
    if (status != UMI_STATUS_OK) return status;
    status = HashFile(path, UMI_AI_TRAINING_PUBLICATION_MAX_COMPANION_BYTES,
                      &candidate.trainingLock);
    if (status != UMI_STATUS_OK) return status;
    status = MakePath(checkpointPath, UMI_PUBLICATION_SOURCE_SUFFIX, path);
    if (status != UMI_STATUS_OK) return status;
    status = HashFile(path, UMI_AI_TRAINING_PUBLICATION_MAX_COMPANION_BYTES,
                      &candidate.sourceLock);
    if (status != UMI_STATUS_OK) return status;
    *outEvidence = candidate;
    return UMI_STATUS_OK;
}

static UmiStatus FormatRecord(
    const UmiAiTrainingPublicationEvidence *evidence,
    char output[UMI_AI_TRAINING_PUBLICATION_RECORD_CAPACITY],
    size_t *outLength)
{
    int length = snprintf(output, UMI_AI_TRAINING_PUBLICATION_RECORD_CAPACITY,
        UMI_PUBLICATION_MAGIC
        "checkpoint_bytes=%" PRIu64 "\ncheckpoint_sha256=%s\n"
        "clip_policy_bytes=%" PRIu64 "\nclip_policy_sha256=%s\n"
        "training_lock_bytes=%" PRIu64 "\ntraining_lock_sha256=%s\n"
        "source_lock_bytes=%" PRIu64 "\nsource_lock_sha256=%s\n",
        evidence->checkpoint.byteCount, evidence->checkpoint.sha256,
        evidence->clipPolicy.byteCount, evidence->clipPolicy.sha256,
        evidence->trainingLock.byteCount, evidence->trainingLock.sha256,
        evidence->sourceLock.byteCount, evidence->sourceLock.sha256);
    if (length <= 0 || length >= (int)UMI_AI_TRAINING_PUBLICATION_RECORD_CAPACITY)
        return UMI_STATUS_INTERNAL_ERROR;
    *outLength = (size_t)length;
    return UMI_STATUS_OK;
}

#if defined(_WIN32)
static int CloseFile(int descriptor) { return _close(descriptor); }
static int SyncFile(int descriptor) { return _commit(descriptor); }
#else
static int CloseFile(int descriptor) { return close(descriptor); }
static int SyncFile(int descriptor) { return fsync(descriptor); }
#endif

UmiStatus UmiAiTrainingPublicationSaveNewFile(const char *checkpointPath)
{
    UmiAiTrainingPublicationEvidence evidence;
    char record[UMI_AI_TRAINING_PUBLICATION_RECORD_CAPACITY];
    char marker[UMI_AI_TRAINING_PUBLICATION_MAX_PATH];
    char staging[UMI_AI_TRAINING_PUBLICATION_MAX_PATH];
    size_t length = 0U, written = 0U;
    UmiStatus status;
    int descriptor = -1;

    status = MakePath(checkpointPath, UMI_PUBLICATION_SUFFIX, marker);
    if (status != UMI_STATUS_OK) return status;
    status = MakePath(checkpointPath,
                      UMI_PUBLICATION_SUFFIX UMI_PUBLICATION_STAGE_SUFFIX, staging);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingPublicationInspect(checkpointPath, &evidence);
    if (status != UMI_STATUS_OK) return status;
    status = FormatRecord(&evidence, record, &length);
    if (status != UMI_STATUS_OK) return status;

    /* Neither the marker nor a crash-leftover partial file may be clobbered. */
#if defined(_WIN32)
    descriptor = _open(staging, _O_CREAT | _O_EXCL | _O_WRONLY | _O_BINARY,
                       _S_IREAD | _S_IWRITE);
#else
    descriptor = open(staging, O_CREAT | O_EXCL | O_WRONLY, 0600);
#endif
    if (descriptor < 0)
        return errno == EEXIST ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_IO_ERROR;
    while (written < length) {
#if defined(_WIN32)
        int amount = _write(descriptor, record + written, (unsigned int)(length - written));
#else
        ssize_t amount = write(descriptor, record + written, length - written);
#endif
        if (amount <= 0) { status = UMI_STATUS_IO_ERROR; goto reject_staging; }
        written += (size_t)amount;
    }
    /* Always close, even when flush fails. The stage remains private until
     * both operations have finished successfully. */
    int syncResult = SyncFile(descriptor);
    int closeResult = CloseFile(descriptor);
    descriptor = -1;
    if (syncResult != 0 || closeResult != 0) {
        status = UMI_STATUS_IO_ERROR;
        goto reject_staging;
    }

#if defined(_WIN32)
    if (!MoveFileExA(staging, marker, MOVEFILE_WRITE_THROUGH)) {
        DWORD error = GetLastError();
        status = error == ERROR_ALREADY_EXISTS || error == ERROR_FILE_EXISTS ?
                 UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_IO_ERROR;
        goto reject_staging;
    }
#else
    /* link() publishes a complete inode without replacing an existing name;
     * rename() would silently overwrite an existing POSIX marker. */
    if (link(staging, marker) != 0) {
        status = errno == EEXIST ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_IO_ERROR;
        goto reject_staging;
    }
    (void)remove(staging);
#endif
    return UMI_STATUS_OK;
reject_staging:
    if (descriptor >= 0) (void)CloseFile(descriptor);
    (void)remove(staging);
    return status;
}

UmiStatus UmiAiTrainingPublicationVerifyAndInspectFile(
    const char *checkpointPath,
    UmiAiTrainingPublicationEvidence *outEvidence)
{
    UmiAiTrainingPublicationEvidence evidence;
    char marker[UMI_AI_TRAINING_PUBLICATION_MAX_PATH];
    char expected[UMI_AI_TRAINING_PUBLICATION_RECORD_CAPACITY];
    char actual[UMI_AI_TRAINING_PUBLICATION_RECORD_CAPACITY];
    size_t expectedLength = 0U, actualLength;
    UmiStatus status;
    int extra, readError;
    FILE *file;

    if (outEvidence == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = MakePath(checkpointPath, UMI_PUBLICATION_SUFFIX, marker);
    if (status != UMI_STATUS_OK) return status;
    file = fopen(marker, "rb");
    if (file == NULL) return UMI_STATUS_IO_ERROR;
    actualLength = fread(actual, 1U, sizeof(actual), file);
    extra = fgetc(file);
    readError = ferror(file);
    if (fclose(file) != 0) readError = 1;
    if (readError != 0) return UMI_STATUS_IO_ERROR;
    if (actualLength == sizeof(actual) || extra != EOF)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    status = UmiAiTrainingPublicationInspect(checkpointPath, &evidence);
    if (status != UMI_STATUS_OK) return status;
    status = FormatRecord(&evidence, expected, &expectedLength);
    if (status != UMI_STATUS_OK) return status;
    if (actualLength != expectedLength ||
        memcmp(actual, expected, expectedLength) != 0)
        return UMI_STATUS_PARSE_ERROR;
    *outEvidence = evidence;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingPublicationVerifyFile(const char *checkpointPath)
{
    UmiAiTrainingPublicationEvidence ignored;
    return UmiAiTrainingPublicationVerifyAndInspectFile(checkpointPath,
                                                        &ignored);
}
