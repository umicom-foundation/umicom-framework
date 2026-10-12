/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_evidence/test_export.c
 * PURPOSE: Verify report immutability, exact bytes and refusal to overwrite existing evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/ai_workspace/evidence_export.h"
#include "umicom/platform/filesystem.h"

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *path = argv[1];
    /* CTest supplies an isolated absolute filename. Never remove a file left
     * by somebody else; an earlier interrupted test remains inspectable. */
    CHECK(!umi_fs_exists(path));
    Fixture fixture;
    CHECK(OpenFixture(&fixture, NULL) == 0);
    CHECK(RunFixture(&fixture) == 0);
    UmiAiEvidenceReview *review = NULL;
    OK(UmiAiEvidenceCapture(fixture.workspace, "job", &review));
    char *expected = malloc(UMI_AI_EVIDENCE_REPORT_CAPACITY);
    CHECK(expected != NULL);
    size_t length = 0U;
    OK(UmiAiEvidenceFormat(review, expected, UMI_AI_EVIDENCE_REPORT_CAPACITY, &length));
    /* Later corpus edits and destruction must not alter the captured report. */
    OK(UmiAiWorkspacePutSource(fixture.workspace, "save", "notes", "Saving", "Later text", 10U));
    CloseFixture(&fixture);
    UmiOutputFileSnapshot output;
    memset(&output, 0x5a, sizeof(output));
    UmiOutputFileSnapshot unchanged = output;
    CHECK(UmiAiEvidenceWriteNew(NULL, path, &output) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&output, &unchanged, sizeof(output)) == 0 && !umi_fs_exists(path));
    CHECK(UmiAiEvidenceWriteNew(review, "relative.txt", &output) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&output, &unchanged, sizeof(output)) == 0);
    OK(UmiAiEvidenceWriteNew(review, path, &output));
    CHECK(output.closed && output.status == UMI_STATUS_OK && output.bytes_written == length);
    CHECK(strcmp(output.path, path) == 0);
    char *actual = NULL;
    size_t readLength = 0U;
    OK(umi_fs_read_text(path, &actual, &readLength));
    CHECK(readLength == length && memcmp(actual, expected, length) == 0);
    umi_fs_free_text(actual);
    output = unchanged;
    CHECK(UmiAiEvidenceWriteNew(review, path, &output) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(memcmp(&output, &unchanged, sizeof(output)) == 0);
    OK(umi_fs_read_text(path, &actual, &readLength));
    CHECK(readLength == length && memcmp(actual, expected, length) == 0);
    umi_fs_free_text(actual);
    /* Only the file this successful test created is removed. */
    OK(umi_fs_remove_tree(path));
    UmiAiEvidenceDestroy(review);
    free(expected);
    return 0;
}
