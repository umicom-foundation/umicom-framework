/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_evidence/test_export_failures.c
 * PURPOSE: Inject report write and close failures while exercising the real evidence formatter.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/ai_workspace/evidence_export.h"
#include "umicom/platform/filesystem.h"

struct UmiOutputFile { UmiOutputFileSnapshot snapshot; };
static unsigned creates, writes, closes;
static unsigned failure;
static UmiStatus ExportTestCreateFile(const char *path, UmiOutputFile **out)
{
    ++creates;
    *out = NULL;
    if (failure == 1U) return UMI_STATUS_PERMISSION_DENIED;
    *out = calloc(1U, sizeof(**out));
    if (*out == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (void)snprintf((*out)->snapshot.path, sizeof((*out)->snapshot.path), "%s", path);
    return UMI_STATUS_OK;
}
static UmiStatus ExportTestWriteFile(UmiOutputFile *file, const void *bytes, size_t length)
{
    (void)bytes;
    ++writes;
    file->snapshot.bytes_written = failure == 2U ? 5U : length;
    file->snapshot.status = failure == 2U ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
    return file->snapshot.status;
}
static UmiStatus ExportTestCloseFile(UmiOutputFile *file)
{
    ++closes;
    file->snapshot.closed = true;
    if (failure == 3U) file->snapshot.status = UMI_STATUS_IO_ERROR;
    return file->snapshot.status;
}
static UmiStatus ExportTestReadFile(const UmiOutputFile *file, UmiOutputFileSnapshot *out)
{
    *out = file->snapshot;
    return UMI_STATUS_OK;
}
static void ExportTestDestroyFile(UmiOutputFile *file) { free(file); }
/* Only the output-file boundary is substituted. Native path validation and
 * rendering still run, and the separate filesystem test checks actual bytes. */
#define UmiAiEvidenceWriteNew TestEvidenceWriteNew
#define UmiOutputFileCreate ExportTestCreateFile
#define UmiOutputFileWrite ExportTestWriteFile
#define UmiOutputFileClose ExportTestCloseFile
#define UmiOutputFileRead ExportTestReadFile
#define UmiOutputFileDestroy ExportTestDestroyFile
#include "../../src/ai_workspace/evidence_export.c"
#undef UmiAiEvidenceWriteNew
#undef UmiOutputFileCreate
#undef UmiOutputFileWrite
#undef UmiOutputFileClose
#undef UmiOutputFileRead
#undef UmiOutputFileDestroy

int main(void)
{
    Fixture fixture;
    CHECK(OpenFixture(&fixture, NULL) == 0);
    CHECK(RunFixture(&fixture) == 0);
    UmiAiEvidenceReview *review = NULL;
    OK(UmiAiEvidenceCapture(fixture.workspace, "job", &review));
    char current[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    OK(umi_fs_current_directory(current, sizeof(current)));
    OK(umi_path_join(current, "injected-inspection.txt", path, sizeof(path)));
    for (failure = 1U; failure <= 3U; ++failure) {
        creates = writes = closes = 0U;
        UmiOutputFileSnapshot output;
        memset(&output, 0x6b, sizeof(output));
        UmiOutputFileSnapshot before = output;
        UmiStatus status = TestEvidenceWriteNew(review, path, &output);
        CHECK(creates == 1U);
        if (failure == 1U) {
            CHECK(status == UMI_STATUS_PERMISSION_DENIED && writes == 0U && closes == 0U);
            CHECK(memcmp(&output, &before, sizeof(output)) == 0);
        } else {
            CHECK(status == UMI_STATUS_IO_ERROR && writes == 1U && closes == 1U);
            CHECK(output.closed && output.status == UMI_STATUS_IO_ERROR);
            CHECK(failure == 2U ? output.bytes_written == 5U : output.bytes_written > 5U);
        }
    }
    UmiAiEvidenceDestroy(review);
    CloseFixture(&fixture);
    return 0;
}
