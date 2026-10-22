/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_source_lock.c
 *
 * PURPOSE:
 *   Verify canonical cryptographic source locks, checkpoint sensitivity,
 *   ordered datasets, exclusive file publication and fail-closed reads.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/training_source_lock.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

#ifndef UMI_AI_TEST_MANIFEST_PATH
#error CMake must supply UMI_AI_TEST_MANIFEST_PATH
#endif
#ifndef UMI_AI_TEST_REORDERED_MANIFEST_PATH
#error CMake must supply UMI_AI_TEST_REORDERED_MANIFEST_PATH
#endif
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

int main(void)
{
#if defined(_WIN32)
    unsigned long pid = (unsigned long)GetCurrentProcessId();
#else
    unsigned long pid = (unsigned long)getpid();
#endif
    uint8_t checkpoint[] = {0x55U, 0x4DU, 0x49U, 0x42U, 0U, 9U, 17U};
    UmiAiTrainingDataset *dataset = NULL, *reordered = NULL;
    char record[UMI_AI_TRAINING_SOURCE_LOCK_RECORD_CAPACITY];
    char small[8] = "SAVED";
    char path[160];
    char modified[UMI_AI_TRAINING_SOURCE_LOCK_RECORD_CAPACITY];
    size_t recordLength = 0U, previousLength = 905U;
    int length;
    FILE *stream;

    CHECK(UmiAiTrainingDatasetLoadManifest(UMI_AI_TEST_MANIFEST_PATH,
          &dataset) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDatasetLoadManifest(UMI_AI_TEST_REORDERED_MANIFEST_PATH,
          &reordered) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingSourceLockFormat(checkpoint, sizeof(checkpoint), dataset,
          record, sizeof(record), &recordLength) == UMI_STATUS_OK);
    CHECK(recordLength > 0U && recordLength < sizeof(record));
    CHECK(strncmp(record, "UMICOM-AI-SOURCE-LOCK-1\n", 24U) == 0);
    CHECK(strstr(record, "checkpoint_sha256=") != NULL);
    CHECK(strstr(record, "dataset_source_sha256=") != NULL);
    CHECK(UmiAiTrainingSourceLockVerify(checkpoint, sizeof(checkpoint), dataset,
          record, recordLength) == UMI_STATUS_OK);

    CHECK(UmiAiTrainingSourceLockFormat(checkpoint, sizeof(checkpoint), dataset,
          small, sizeof(small), &previousLength) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(strcmp(small, "SAVED") == 0 && previousLength == 905U);
    CHECK(UmiAiTrainingSourceLockFormat(NULL, sizeof(checkpoint), dataset,
          small, sizeof(small), &previousLength) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(strcmp(small, "SAVED") == 0 && previousLength == 905U);
    checkpoint[0] ^= 1U;
    CHECK(UmiAiTrainingSourceLockVerify(checkpoint, sizeof(checkpoint), dataset,
          record, recordLength) == UMI_STATUS_PARSE_ERROR);
    checkpoint[0] ^= 1U;
    CHECK(UmiAiTrainingSourceLockVerify(checkpoint, sizeof(checkpoint), reordered,
          record, recordLength) == UMI_STATUS_PARSE_ERROR);

    memcpy(modified, record, recordLength);
    modified[recordLength] = '\0';
    modified[2] = 'x';
    CHECK(UmiAiTrainingSourceLockVerify(checkpoint, sizeof(checkpoint), dataset,
          modified, recordLength) == UMI_STATUS_PARSE_ERROR);
    memcpy(modified, record, recordLength);
    modified[recordLength] = '\0';
    CHECK(UmiAiTrainingSourceLockVerify(checkpoint, sizeof(checkpoint), dataset,
          modified, recordLength - 1U) == UMI_STATUS_PARSE_ERROR);
    CHECK(UmiAiTrainingSourceLockVerify(checkpoint, sizeof(checkpoint), dataset,
          modified, recordLength + 1U) == UMI_STATUS_PARSE_ERROR);

    length = snprintf(path, sizeof(path), "umicom-source-lock-%lu.txt", pid);
    CHECK(length > 0 && (size_t)length < sizeof(path));
    CHECK(UmiAiTrainingSourceLockVerifyFile(path, checkpoint,
          sizeof(checkpoint), dataset) == UMI_STATUS_IO_ERROR);
    CHECK(UmiAiTrainingSourceLockSaveNewFile(path, checkpoint,
          sizeof(checkpoint), dataset) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingSourceLockSaveNewFile(path, checkpoint,
          sizeof(checkpoint), dataset) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(UmiAiTrainingSourceLockVerifyFile(path, checkpoint,
          sizeof(checkpoint), dataset) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingSourceLockVerifyFile(path, checkpoint,
          sizeof(checkpoint), reordered) == UMI_STATUS_PARSE_ERROR);

    /* Replace the test-owned temporary file with corrupt bytes. This test
     * does not alter an existing reviewed checkpoint or a repository file. */
    stream = fopen(path, "wb");
    CHECK(stream != NULL);
    CHECK(fwrite("x", 1U, 1U, stream) == 1U);
    CHECK(fclose(stream) == 0);
    CHECK(UmiAiTrainingSourceLockVerifyFile(path, checkpoint,
          sizeof(checkpoint), dataset) == UMI_STATUS_PARSE_ERROR);
    CHECK(remove(path) == 0);
    CHECK(UmiAiTrainingSourceLockVerifyFile(path, checkpoint,
          sizeof(checkpoint), dataset) == UMI_STATUS_IO_ERROR);

    UmiAiTrainingDatasetDestroy(reordered);
    UmiAiTrainingDatasetDestroy(dataset);
    puts("source lock: format, snapshot, integrity, no-clobber, failure PASS");
    return 0;
}
