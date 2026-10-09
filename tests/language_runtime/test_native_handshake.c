/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_native_handshake.c
 * PURPOSE: Exercise manager selection and LSP initialization through the actual native process transport.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/language_runtime/server_manager.h"
#include "umicom/language_runtime/server_probe.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/clock.h"
#include "umicom/platform/process_search_path.h"

static int ProgramMain(int argc, char **argv)
{
    if (argc != 3)
        return 2;
    const char *mode = argv[1];
    char root[UMI_PATH_CAPACITY], directory[UMI_PATH_CAPACITY], executable[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(directory, root, "caf\xc3\xa9-\xe6\x96\x87");
    CHECK(umi_fs_make_directories(directory) == UMI_STATUS_OK);
    UmiLanguageService *language = NULL;
    UmiLanguageRuntimeServerManager *manager = NULL;
    CHECK(umi_language_service_create(&language) == UMI_STATUS_OK);
    CHECK(umi_language_runtime_server_manager_create(language, &manager) == UMI_STATUS_OK);
    UmiLanguageServerProfile profile = *umi_language_runtime_builtin_profile_for_language("c");
    CHECK(strlen(argv[2]) < sizeof(profile.executable));
    strcpy(profile.executable, argv[2]);
    strcpy(profile.arguments, "lsp");
    profile.enabled = 1;

    /* These cases exercise the real child transport while keeping every selected
     * path and cancellation token owned by this call. No language tool is installed. */
    if (strncmp(mode, "tools-", 6U) == 0)
    {
        char toolFolder[UMI_PATH_CAPACITY];
        CHECK(umi_path_parent(argv[2], toolFolder, sizeof toolFolder) == UMI_STATUS_OK);
        const char *selected = toolFolder;
        UmiCancellationToken *cancel = NULL;
        UmiLanguageRuntimeProbeResult report;
        char *before = NULL, *after = NULL;
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        CHECK(UmiProcessSearchPathRead(&before) == UMI_STATUS_OK);
        UmiStatus expected = UMI_STATUS_OK;
        if (strcmp(mode, "tools-name") == 0)
            strcpy(profile.executable, "umicom-language-process-fixture");
        else if (strcmp(mode, "tools-invalid") == 0)
        {
            selected = "relative/tools";
            expected = UMI_STATUS_INVALID_ARGUMENT;
        }
        else if (strcmp(mode, "tools-cancel") == 0)
        {
            umi_cancellation_token_request(cancel);
            expected = UMI_STATUS_CANCELLED;
        }
        else if (strcmp(mode, "tools-missing") == 0)
        {
            selected = directory;
            strcpy(profile.executable, "missing-language-fixture");
            expected = UMI_STATUS_IO_ERROR;
        }
        else if (strcmp(mode, "tools-absolute") != 0) return 2;
        CHECK(UmiLanguageRuntimeProbeWithToolDirectory(&profile, "file:///project", directory,
            selected, 2000U, cancel, &report) == expected);
        CHECK(report.initializationStatus == expected);
        CHECK(report.launched == (expected == UMI_STATUS_OK));
        CHECK(report.initialized == (expected == UMI_STATUS_OK));
        CHECK(report.shutdownStatus == UMI_STATUS_OK);
        CHECK(UmiProcessSearchPathRead(&after) == UMI_STATUS_OK);
        CHECK(strcmp(before, after) == 0);
        UmiProcessSearchPathFree(before);
        UmiProcessSearchPathFree(after);
        umi_cancellation_token_destroy(cancel);
        umi_language_runtime_server_manager_destroy(manager);
        umi_language_service_destroy(language);
        return 0;
    }
    if (strncmp(mode, "probe", 5U) == 0)
    {
        UmiLanguageRuntimeProbeResult report;
        UmiCancellationToken *cancel = NULL;
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        UmiStatus expectedProbe = UMI_STATUS_OK;
        if (strcmp(mode, "probe-error") == 0)
        {
            strcpy(profile.arguments, "lsp-error");
            expectedProbe = UMI_STATUS_UNAVAILABLE;
        }
        else if (strcmp(mode, "probe-timeout") == 0)
        {
            strcpy(profile.arguments, "lsp-timeout");
            expectedProbe = UMI_STATUS_TIMEOUT;
        }
        else if (strcmp(mode, "probe-cancel") == 0)
        {
            umi_cancellation_token_request(cancel);
            expectedProbe = UMI_STATUS_CANCELLED;
        }
        else if (strcmp(mode, "probe-invalid") == 0)
        {
            memset(profile.arguments, 'x', sizeof(profile.arguments));
            expectedProbe = UMI_STATUS_INVALID_ARGUMENT;
        }
        else if (strcmp(mode, "probe") != 0)
            return 2;
        CHECK(UmiLanguageRuntimeProbe(&profile, "file:///project", directory,
                                      strcmp(mode, "probe-timeout") == 0 ? 30U : 2000U, cancel,
                                      &report) == expectedProbe);
        CHECK(report.initializationStatus == expectedProbe);
        CHECK(report.initialized == (expectedProbe == UMI_STATUS_OK));
        CHECK(report.launched ==
              (expectedProbe != UMI_STATUS_CANCELLED && expectedProbe != UMI_STATUS_INVALID_ARGUMENT));
        CHECK(report.shutdownStatus == UMI_STATUS_OK);
        if (report.initialized)
            CHECK(report.capabilities.completion && report.capabilities.hover);
        umi_cancellation_token_destroy(cancel);
        umi_language_runtime_server_manager_destroy(manager);
        umi_language_service_destroy(language);
        return 0;
    }
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "timeout") == 0)
    {
        strcpy(profile.arguments, "lsp-timeout");
        expected = UMI_STATUS_TIMEOUT;
    }
    else if (strcmp(mode, "error") == 0)
    {
        strcpy(profile.arguments, "lsp-error");
        expected = UMI_STATUS_UNAVAILABLE;
    }
    else if (strcmp(mode, "invalid") == 0)
    {
        strcpy(profile.arguments, "lsp-invalid");
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "unicode") == 0)
    {
#ifdef _WIN32
        FixturePath(executable, directory, "language peer.exe");
#else
        FixturePath(executable, directory, "language peer");
#endif
        CHECK(umi_fs_copy_file(argv[2], executable) == UMI_STATUS_OK);
#ifndef _WIN32
        CHECK(chmod(executable, 0700) == 0);
#endif
        CHECK(strlen(executable) < sizeof(profile.executable));
        strcpy(profile.executable, executable);
    }
    else if (strcmp(mode, "valid") != 0 && strcmp(mode, "reuse") != 0 && strcmp(mode, "configured") != 0 &&
             strcmp(mode, "stopped") != 0)
        return 2;
    UmiLanguageRuntimeServer *server = NULL;
    UmiClock clock = umi_clock_system();
    uint64_t start = clock.monotonic_nanoseconds(&clock);
    UmiStatus status;
    if (strcmp(mode, "configured") == 0)
    {
        CHECK(umi_language_server_profile_registry_upsert(umi_language_service_server_profiles(language),
                                                          &profile) == UMI_STATUS_OK);
        status = umi_language_runtime_server_manager_start_for_language(manager, "c", "file:///project",
                                                                        directory, 2000U, &server);
    }
    else
        status = UmiLanguageRuntimeServerManagerStartProfile(
            manager, "c", &profile, "file:///project", directory, strcmp(mode, "timeout") == 0 ? 30U : 2000U,
            NULL, &server);
    CHECK(status == expected);
    if (status == UMI_STATUS_OK)
    {
        UmiLanguageRuntimeServerSnapshot snapshot;
        CHECK(server != NULL && umi_language_runtime_server_manager_count(manager) == 1U);
        CHECK(umi_language_runtime_server_snapshot(server, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.state == UMI_LANGUAGE_RUNTIME_SERVER_READY && snapshot.pending_requests == 0U);
        CHECK(snapshot.messages_sent == 2U && snapshot.messages_received == 1U);
        if (strcmp(mode, "reuse") == 0)
        {
            UmiLanguageRuntimeServer *again = NULL;
            CHECK(umi_language_runtime_server_manager_start_for_language(
                      manager, "c", "file:///project", directory, 2000U, &again) == UMI_STATUS_OK &&
                  again == server);
            CHECK(UmiLanguageRuntimeServerManagerStartProfile(manager, "c", &profile, "file:///project",
                                                              directory, 2000U, NULL,
                                                              &again) == UMI_STATUS_ALREADY_EXISTS &&
                  again == NULL);
        }
        CHECK(umi_language_runtime_server_manager_stop_all(manager, 2000U) == UMI_STATUS_OK);
        CHECK(!umi_language_runtime_server_is_running(server));
        if (strcmp(mode, "stopped") == 0)
        {
            UmiLanguageRuntimeServer *again = server;
            CHECK(umi_language_runtime_server_manager_start_for_language(manager, "c", "file:///project",
                                                                         directory, 2000U, &again) ==
                      UMI_STATUS_INVALID_STATE &&
                  again == NULL);
        }
    }
    else
    {
        CHECK(server == NULL && umi_language_runtime_server_manager_count(manager) == 0U);
        if (strcmp(mode, "timeout") == 0)
        {
            /* This loose bound catches the former 64 full timeout attempts
             * without assuming a precise scheduler wake-up time. */
            CHECK((clock.monotonic_nanoseconds(&clock) - start) / 1000000U < 1800U);
        }
    }
    umi_language_runtime_server_manager_destroy(manager);
    umi_language_service_destroy(language);
    return 0;
}
#include "../native_process/utf8_entry.inc"
