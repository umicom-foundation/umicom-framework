/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/diagnostic_session/test_monitor.c
 * PURPOSE: Check source coalescing, immutable caller inputs and native diagnostic worker lifetime.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/diagnostic_monitor.h"
#include "umicom/platform/path.h"
#include "umicom/platform/threading.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (!(value))                                                                              \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                            \
            failed = 1;                                                                            \
            goto cleanup;                                                                          \
        }                                                                                          \
    } while (0)
static int Run(void *context) { return (int)UmiLanguageDiagnosticMonitorRun(context); }
static int Wait(UmiLanguageDiagnosticMonitor *monitor, uint64_t sequence,
                UmiLanguageDiagnosticCatalogue **out)
{
    for (unsigned attempt = 0U; attempt < 1000U; ++attempt)
    {
        uint64_t received = 0U;
        UmiStatus status = UmiLanguageDiagnosticMonitorTake(monitor, &received, out);
        if (status == UMI_STATUS_OK)
        {
            if (received == sequence)
                return 1;
            UmiLanguageDiagnosticCatalogueDestroy(*out);
            *out = NULL;
        }
        UmiLanguageDiagnosticMonitorSnapshot snapshot;
        if (UmiLanguageDiagnosticMonitorRead(monitor, &snapshot) != UMI_STATUS_OK ||
            snapshot.completed)
            return 0;
        umi_thread_sleep_ms(10U);
    }
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 3)
        return 2;
    int failed = 0;
    UmiLanguageDiagnosticMonitor *monitor = NULL;
    UmiLanguageDiagnosticCatalogue *catalogue = NULL;
    UmiThread *thread = NULL;
    UmiLanguageDiagnosticMonitorConfig config = {0};
    strcpy(config.profile.id, "native-diagnostic-fixture");
    if (strlen(argv[2]) >= sizeof config.profile.executable)
        return 2;
    strcpy(config.profile.executable, argv[2]);
    config.profile.enabled = 1;
    char directory[UMI_PATH_CAPACITY];
    CHECK(umi_path_parent(argv[2], directory, sizeof directory) == UMI_STATUS_OK);
    config.working_directory = directory;
    if (strcmp(argv[1], "delayed") == 0)
        strcpy(config.profile.arguments, "delay");
    if (strcmp(argv[1], "unversioned") == 0)
        strcpy(config.profile.arguments, "unversioned");
    if (strcmp(argv[1], "request") == 0)
        strcpy(config.profile.arguments, "request");
    config.document = (UmiLanguageDiagnosticRequest){
        "file:///workspace", "file:///workspace/main.c", "c", "initial", 7U, 5000U};
    CHECK(UmiLanguageDiagnosticMonitorCreate(&config, &monitor) == UMI_STATUS_OK);
    uint64_t sequence = 0U;
    if (strcmp(argv[1], "coalesce") == 0)
    {
        CHECK(UmiLanguageDiagnosticMonitorSubmit(monitor, "middle", 6U, &sequence) ==
                  UMI_STATUS_OK &&
              sequence == 2U);
        CHECK(UmiLanguageDiagnosticMonitorSubmit(monitor, "latest", 6U, &sequence) ==
                  UMI_STATUS_OK &&
              sequence == 3U);
        UmiLanguageDiagnosticMonitorSnapshot snapshot;
        CHECK(UmiLanguageDiagnosticMonitorRead(monitor, &snapshot) == UMI_STATUS_OK &&
              snapshot.coalesced_updates == 2U);
    }
    if (strcmp(argv[1], "prestop") == 0)
    {
        CHECK(UmiLanguageDiagnosticMonitorStop(monitor) == UMI_STATUS_OK);
        CHECK(UmiLanguageDiagnosticMonitorRun(monitor) == UMI_STATUS_CANCELLED);
        CHECK(UmiLanguageDiagnosticMonitorRun(monitor) == UMI_STATUS_INVALID_STATE);
        goto cleanup;
    }
    CHECK(umi_thread_start(Run, monitor, &thread) == UMI_STATUS_OK);
    if (strcmp(argv[1], "request") == 0)
    {
        UmiLanguageDiagnosticMonitorSnapshot snapshot = {0};
        for (unsigned attempt = 0U; attempt < 1000U && !snapshot.completed; ++attempt)
        {
            CHECK(UmiLanguageDiagnosticMonitorRead(monitor, &snapshot) == UMI_STATUS_OK);
            umi_thread_sleep_ms(10U);
        }
        CHECK(snapshot.completed && snapshot.status == UMI_STATUS_NOT_IMPLEMENTED);
        goto cleanup;
    }
    uint64_t first = strcmp(argv[1], "coalesce") == 0 ? 3U : 1U;
    CHECK(Wait(monitor, first, &catalogue));
    UmiLanguageDiagnostic row;
    CHECK(UmiLanguageDiagnosticCatalogueAt(catalogue, 0U, &row) == UMI_STATUS_OK);
    CHECK(strcmp(row.message, first == 3U ? "latest" : "initial") == 0);
    UmiLanguageDiagnosticCatalogueDestroy(catalogue);
    catalogue = NULL;
    if (strcmp(argv[1], "invalid") == 0)
    {
        sequence = 77U;
        CHECK(UmiLanguageDiagnosticMonitorSubmit(monitor, "\xff", 1U, &sequence) != UMI_STATUS_OK &&
              sequence == 77U);
    }
    else if (strcmp(argv[1], "same") == 0)
    {
        CHECK(UmiLanguageDiagnosticMonitorSubmit(monitor, "initial", 7U, &sequence) ==
                  UMI_STATUS_OK &&
              sequence == 1U);
    }
    else
    {
        CHECK(strcmp(argv[1], "normal") == 0 || strcmp(argv[1], "delayed") == 0 ||
              strcmp(argv[1], "unversioned") == 0 || strcmp(argv[1], "coalesce") == 0);
        char text[] = "changed \xf0\x9f\x98\x80";
        CHECK(UmiLanguageDiagnosticMonitorSubmit(monitor, text, strlen(text), &sequence) ==
              UMI_STATUS_OK);
        text[0] = 'X'; /* The worker must own its copy before the caller resumes. */
        if (strcmp(argv[1], "unversioned") == 0)
        {
            UmiLanguageDiagnosticMonitorSnapshot snapshot = {0};
            for (unsigned attempt = 0U;
                 attempt < 1000U && snapshot.session.ignored_publications == 0U; ++attempt)
            {
                CHECK(UmiLanguageDiagnosticMonitorRead(monitor, &snapshot) == UMI_STATUS_OK);
                umi_thread_sleep_ms(10U);
            }
            CHECK(snapshot.session.ignored_publications != 0U && !snapshot.completed);
            uint64_t old_sequence = 77U;
            CHECK(UmiLanguageDiagnosticMonitorTake(monitor, &old_sequence, &catalogue) ==
                      UMI_STATUS_NOT_FOUND &&
                  old_sequence == 77U);
        }
        else
        {
            CHECK(Wait(monitor, sequence, &catalogue));
            CHECK(UmiLanguageDiagnosticCatalogueAt(catalogue, 0U, &row) == UMI_STATUS_OK);
            CHECK(strcmp(row.message, "changed \xf0\x9f\x98\x80") == 0);
        }
    }
cleanup:
    if (monitor != NULL)
        (void)UmiLanguageDiagnosticMonitorStop(monitor);
    if (thread != NULL)
    {
        if (umi_thread_join(thread, NULL) != UMI_STATUS_OK)
            failed = 1;
        umi_thread_destroy(thread);
    }
    UmiLanguageDiagnosticCatalogueDestroy(catalogue);
    if (UmiLanguageDiagnosticMonitorDestroy(&monitor) != UMI_STATUS_OK)
        failed = 1;
    return failed;
}
