/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_release_approval_file.c
 * PURPOSE: Verify bounded local approval loading and failure-atomic outputs.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/training_release_approval.h"

#include <stdio.h>
#include <string.h>
#if defined(_WIN32)
#include <process.h>
#define TEST_PID _getpid
#else
#include <unistd.h>
#define TEST_PID getpid
#endif

#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); return 1; \
} } while (0)

#define APPROVED_SHA \
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"

static int WriteBytes(const char *path, const void *data, size_t length)
{
    FILE *output = fopen(path, "wb");
    int okay;
    if (output == NULL) return 0;
    okay = fwrite(data, 1U, length, output) == length;
    if (fclose(output) != 0) okay = 0;
    return okay;
}

int main(void)
{
    const char source[] =
        "UMICOM-AI-RELEASE-APPROVAL-1\n"
        "publication_count=1\n"
        "evaluated_at_unix_seconds=150\n"
        "batch_sha256=" APPROVED_SHA "\n";
    char path[128];
    char huge[UMI_AI_TRAINING_RELEASE_APPROVAL_MAX_BYTES + 1U];
    UmiAiTrainingReleaseApproval parsed;
    UmiAiTrainingReleaseApproval previous;
    int n = snprintf(path, sizeof(path), "umicom-approval-file-%ld.txt",
                     (long)TEST_PID());
    CHECK(n > 0 && (size_t)n < sizeof(path));
    CHECK(WriteBytes(path, source, sizeof(source) - 1U));
    CHECK(UmiAiTrainingReleaseApprovalLoadFile(path, &parsed) == UMI_STATUS_OK);
    CHECK(parsed.publicationCount == 1U);
    CHECK(parsed.evaluatedAtUnixSeconds == 150U);
    CHECK(strcmp(parsed.batchSha256, APPROVED_SHA) == 0);

    memset(&parsed, 0x4E, sizeof(parsed));
    previous = parsed;
    memset(huge, 'A', sizeof(huge));
    CHECK(WriteBytes(path, huge, sizeof(huge)));
    CHECK(UmiAiTrainingReleaseApprovalLoadFile(path, &parsed) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&parsed, &previous, sizeof(parsed)) == 0);

    CHECK(WriteBytes(path, "UMICOM-AI-RELEASE-APPROVAL-1\n", 30U));
    CHECK(UmiAiTrainingReleaseApprovalLoadFile(path, &parsed) != UMI_STATUS_OK);
    CHECK(memcmp(&parsed, &previous, sizeof(parsed)) == 0);
    CHECK(remove(path) == 0);
    CHECK(UmiAiTrainingReleaseApprovalLoadFile(path, &parsed) == UMI_STATUS_IO_ERROR);
    CHECK(memcmp(&parsed, &previous, sizeof(parsed)) == 0);
    puts("Approval file: bounded loading and no partial output PASS");
    return 0;
}
