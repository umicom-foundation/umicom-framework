/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_stream/test_pages.c
 * PURPOSE: Verify live diagnostic paging, phase identity and retained-loss accounting.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/live_diagnostics.h"
#include "umicom/build/live_output.h"
#include "umicom/build/parser.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/threading.h"
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (!(value))                                                                              \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                            \
            exit(1);                                                                               \
        }                                                                                          \
    } while (0)
typedef struct PageProbe
{
    atomic_int entered, permit;
    size_t records;
    int damaged;
} PageProbe;
static UmiStatus Execute(const UmiBuildProfile *profile, UmiBuildPhase phase,
                         UmiCancellationToken *cancel, UmiProcessOutputObserver observer,
                         void *owner, UmiBuildResult *result, void *context)
{
    (void)profile;
    (void)result;
    PageProbe *probe = context;
    size_t records = phase == UMI_BUILD_PHASE_BUILD ? 1U : probe->records;
    for (size_t index = 0U; index < records; ++index)
    {
        char line[192];
        int bytes = snprintf(line, sizeof line, "src/example.c:%zu:2: warning: record %zu\n",
                             index + 1U, index);
        CHECK(bytes > 0 && (size_t)bytes < sizeof line);
        observer(line, (size_t)bytes, owner);
    }
    if (probe->damaged)
    {
        const char bytes[] = "broken.c:9: error: prefix\0broken suffix\n";
        observer(bytes, sizeof bytes - 1U, owner);
    }
    atomic_store(&probe->entered, (int)phase + 1);
    while (atomic_load(&probe->permit) < (int)phase + 1)
    {
        if (umi_cancellation_token_is_requested(cancel))
            return UMI_STATUS_CANCELLED;
        umi_thread_sleep_ms(1U);
    }
    return UMI_STATUS_OK;
}
static void Entered(PageProbe *probe, UmiBuildPhase phase)
{
    for (unsigned index = 0U; index < 5000U && atomic_load(&probe->entered) != (int)phase + 1;
         ++index)
        umi_thread_sleep_ms(1U);
    CHECK(atomic_load(&probe->entered) == (int)phase + 1);
}
static void Finished(UmiBuildProjectSession *session)
{
    for (unsigned index = 0U; index < 5000U; ++index)
    {
        UmiBuildProjectSessionSnapshot state;
        CHECK(umi_build_project_session_snapshot(session, &state) == UMI_STATUS_OK);
        if (!state.active)
            return;
        umi_thread_sleep_ms(1U);
    }
    CHECK(0 && "Worker did not finish");
}
static void Projection(void)
{
    UmiCompilerDiagnosticFields *fields = calloc(1U, sizeof *fields);
    CHECK(fields != NULL);
    UmiBuildDiagnostic output, before;
    memset(&output, 0x57, sizeof output);
    before = output;
    memset(fields->path, 'p', sizeof fields->path);
    CHECK(UmiBuildDiagnosticFromCompilerFields(fields, &output) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&output, &before, sizeof output) == 0);
    memset(fields, 0, sizeof *fields);
    memset(fields->code, 'c', sizeof fields->code);
    CHECK(UmiBuildDiagnosticFromCompilerFields(fields, &output) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&output, &before, sizeof output) == 0);
    memset(fields, 0, sizeof *fields);
    memset(fields->message, 'm', sizeof fields->message);
    CHECK(UmiBuildDiagnosticFromCompilerFields(fields, &output) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&output, &before, sizeof output) == 0);
    memset(fields, 0, sizeof *fields);
    fields->severity = (UmiDiagnosticSeverity)99;
    CHECK(UmiBuildDiagnosticFromCompilerFields(fields, &output) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&output, &before, sizeof output) == 0);
    fields->severity = UMI_DIAGNOSTIC_TRACE;
    CHECK(UmiBuildDiagnosticFromCompilerFields(fields, &output) == UMI_STATUS_OK);
    CHECK(output.severity == UMI_BUILD_DIAGNOSTIC_NOTE);
    free(fields);
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    if (strcmp(mode, "projection") == 0)
    {
        Projection();
        return 0;
    }
    PageProbe probe = {0};
    probe.records = strcmp(mode, "capacity") == 0 ? UMI_BUILD_MAX_DIAGNOSTICS + 17U : 19U;
    probe.damaged = strcmp(mode, "damaged") == 0;
    UmiBuildProjectSessionConfig config = {NULL, NULL, &probe};
    UmiBuildProjectSession *session = NULL;
    CHECK(UmiBuildProjectSessionCreateObserved(&config, Execute, &session) == UMI_STATUS_OK);
    UmiBuildDiagnosticPage *page = calloc(1U, sizeof *page), *saved = calloc(1U, sizeof *saved);
    CHECK(page != NULL && saved != NULL);
    CHECK(UmiBuildProjectSessionReadDiagnosticPage(session, 0U, 0U, 0U, page) == UMI_STATUS_OK);
    CHECK(page->count == 0U && page->progress.operation_id == 0U);
    UmiBuildProfile profile;
    umi_build_profile_init(&profile);
    UmiBuildPhase requested =
        strcmp(mode, "phases") == 0 ? UMI_BUILD_PHASE_BUILD : UMI_BUILD_PHASE_CONFIGURE;
    CHECK(umi_build_project_session_submit(session, &profile, requested, true) == UMI_STATUS_OK);
    Entered(&probe, UMI_BUILD_PHASE_CONFIGURE);
    CHECK(UmiBuildProjectSessionReadDiagnosticPage(session, 0U, 0U, 0U, page) == UMI_STATUS_OK);
    CHECK(page->progress.operation_id == 1U && !page->progress.phase_complete);
    CHECK(page->count == UMI_BUILD_DIAGNOSTIC_PAGE_CAPACITY && page->first_index == 0U);
    CHECK(umi_path_is_absolute(page->source_directory));
    CHECK(umi_path_is_absolute(page->build_directory));
    char expected_build[UMI_BUILD_PATH_CAPACITY];
    CHECK(umi_path_absolute(profile.build_directory, page->source_directory,
        expected_build, sizeof expected_build) == UMI_STATUS_OK);
    CHECK(umi_path_equal(expected_build, page->build_directory));
    CHECK(page->items[0].line == 1U && page->items[15].line == 16U);
    *saved = *page;
    if (strcmp(mode, "pages") == 0)
    {
        CHECK(UmiBuildProjectSessionReadDiagnosticPage(session, 1U, 0U, 16U, page) ==
              UMI_STATUS_OK);
        CHECK(page->count == 3U && page->items[0].line == 17U && page->items[2].line == 19U);
        CHECK(UmiBuildProjectSessionReadDiagnosticPage(session, 1U, 0U, 19U, page) ==
              UMI_STATUS_OK);
        CHECK(page->count == 0U && page->retained_count == 19U);
        CHECK(saved->items[0].line == 1U && saved->count == 16U);
    }
    else if (strcmp(mode, "capacity") == 0)
    {
        CHECK(page->retained_count == 256U && page->retention_dropped == 17U);
        CHECK(page->progress.warnings == 273U && page->progress.unrepresented == 0U);
        CHECK(UmiBuildProjectSessionReadDiagnosticPage(session, 1U, 0U, 240U, page) ==
              UMI_STATUS_OK);
        CHECK(page->items[15].line == 256U);
    }
    else if (strcmp(mode, "damaged") == 0)
        CHECK(page->progress.unrepresented == 1U && page->retention_dropped == 0U);
    else if (strcmp(mode, "invalid") == 0)
    {
        CHECK(UmiBuildProjectSessionReadDiagnosticPage(session, 1U, 0U, 20U, page) ==
              UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(page, saved, sizeof *page) == 0);
        CHECK(UmiBuildProjectSessionReadDiagnosticPage(session, 2U, 0U, 0U, page) ==
              UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(page, saved, sizeof *page) == 0);
        CHECK(UmiBuildProjectSessionReadDiagnosticPage(session, 1U, 1U, 0U, page) ==
              UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(page, saved, sizeof *page) == 0);
        CHECK(UmiBuildProjectSessionReadDiagnosticPage(NULL, 0U, 0U, 0U, page) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(page, saved, sizeof *page) == 0);
    }
    else
        CHECK(strcmp(mode, "phases") == 0 || strcmp(mode, "cancel") == 0 ||
              strcmp(mode, "repeat") == 0);
    if (strcmp(mode, "cancel") == 0)
        umi_build_project_session_cancel(session);
    else
        atomic_store(&probe.permit, 1);
    if (requested == UMI_BUILD_PHASE_BUILD)
    {
        Entered(&probe, UMI_BUILD_PHASE_BUILD);
        CHECK(UmiBuildProjectSessionReadDiagnosticPage(session, 1U, 0U, 0U, page) ==
              UMI_STATUS_INVALID_STATE);
        CHECK(UmiBuildProjectSessionReadDiagnosticPage(session, 0U, 0U, 0U, page) == UMI_STATUS_OK);
        CHECK(page->progress.phase_index == 1U && page->count == 1U &&
              page->retention_dropped == 0U);
        atomic_store(&probe.permit, 2);
    }
    Finished(session);
    CHECK(UmiBuildProjectSessionReadDiagnosticPage(session, 0U, 0U, 0U, page) == UMI_STATUS_OK);
    CHECK(page->progress.phase_complete && !saved->progress.phase_complete);
    if (strcmp(mode, "repeat") == 0)
    {
        atomic_store(&probe.permit, 0);
        atomic_store(&probe.entered, 0);
        probe.records = 1U;
        CHECK(umi_build_project_session_submit(session, &profile, requested, true) ==
              UMI_STATUS_OK);
        Entered(&probe, UMI_BUILD_PHASE_CONFIGURE);
        CHECK(UmiBuildProjectSessionReadDiagnosticPage(session, 1U, 0U, 0U, page) ==
              UMI_STATUS_INVALID_STATE);
        CHECK(UmiBuildProjectSessionReadDiagnosticPage(session, 0U, 0U, 0U, page) == UMI_STATUS_OK);
        CHECK(page->progress.operation_id == 2U && page->count == 1U);
        atomic_store(&probe.permit, 1);
        Finished(session);
    }
    umi_build_project_session_destroy(session);
    free(page);
    free(saved);
    return 0;
}
