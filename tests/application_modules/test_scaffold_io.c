/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_modules/test_scaffold_io.c
 * PURPOSE: Inject output failures into the actual scaffold publisher and inspect retained files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/scaffold/application_module.h"
#include "umicom/platform/output_file.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/input_file.h"
#include <stdio.h>
#include <string.h>

static size_t write_count, close_count;
static int fail_close;

/* Faults wrap the real file API: the failing write accepts a prefix, and the
 * failing close actually closes its handle. This exercises retained evidence
 * without relying on disk exhaustion or changing host filesystem permissions. */
static UmiStatus failing_write(UmiOutputFile *file, const void *bytes, size_t length)
{
    ++write_count;
    if (!fail_close && write_count == 3U) {
        UmiStatus status = UmiOutputFileWrite(file, bytes, length < 2U ? length : 2U);
        return status == UMI_STATUS_OK ? UMI_STATUS_IO_ERROR : status;
    }
    return UmiOutputFileWrite(file, bytes, length);
}
static UmiStatus failing_close(UmiOutputFile *file)
{
    UmiStatus status = UmiOutputFileClose(file);
    ++close_count;
    return fail_close && close_count == 3U && status == UMI_STATUS_OK
        ? UMI_STATUS_IO_ERROR : status;
}

/* Compile the production publisher with only its write/close boundaries
 * substituted. Its public API definitions satisfy this test executable, so the
 * archive's second copy is not linked. No production fault switch is exposed. */
#define UmiOutputFileWrite failing_write
#define UmiOutputFileClose failing_close
#include "../../src/scaffold/application_module.c"
#undef UmiOutputFileWrite
#undef UmiOutputFileClose

#define CHECK(condition) do { if (!(condition)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; \
} } while (0)

int main(int argc, char **argv)
{
    UmiApplicationScaffoldRequest request = {0};
    UmiApplicationScaffoldPlan *plan = NULL;
    UmiApplicationScaffoldResult result;
    UmiApplicationScaffoldSummary summary;
    UmiApplicationScaffoldFile view;
    char parent[UMI_PATH_CAPACITY], templates[UMI_PATH_CAPACITY], absent[UMI_PATH_CAPACITY];
    unsigned char *bytes = NULL;
    size_t length = 0U;
    CHECK(argc == 3);
    fail_close = strcmp(argv[1], "close_failure") == 0;
    CHECK(umi_path_join(argv[2], "output", parent, sizeof(parent)) == UMI_STATUS_OK);
    CHECK(umi_path_join(argv[2], "templates", templates, sizeof(templates)) == UMI_STATUS_OK);
    request.template_root = templates; request.destination_parent = parent;
    request.slug = "weather"; request.application_name = "Weather";
    request.application_id = "org.example.weather"; request.gtk_frontend = true;
    CHECK(umi_application_scaffold_plan(&request, &plan) == UMI_STATUS_OK);
    CHECK(umi_application_scaffold_apply(plan, &result) == UMI_STATUS_IO_ERROR);
    CHECK(result.created_directories == 4U && result.created_files == 3U &&
        result.completed_files == 2U && result.status == UMI_STATUS_IO_ERROR);
    CHECK(umi_application_scaffold_summary(plan, &summary) == UMI_STATUS_OK);
    CHECK(summary.apply_started);
    CHECK(UmiInputFileRead(result.last_path, UMI_APPLICATION_SCAFFOLD_TEXT_LIMIT,
        &bytes, &length) == UMI_STATUS_OK);
    CHECK(umi_application_scaffold_file(plan, 2U, &view) == UMI_STATUS_OK);
    CHECK(length == (fail_close ? view.length : 2U));
    CHECK(memcmp(bytes, view.text, length) == 0);
    UmiInputFileFree(bytes);
    CHECK(umi_path_join(summary.root, ".gitignore", absent, sizeof(absent)) == UMI_STATUS_OK);
    CHECK(!umi_fs_exists(absent));
    CHECK(umi_application_scaffold_apply(plan, &result) == UMI_STATUS_INVALID_STATE);
    umi_application_scaffold_destroy(plan);
    return 0;
}
