/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_session_recovery.c
 *
 * PURPOSE:
 *   Verify deterministic session persistence, unsaved-document recovery and
 *   per-user application directory resolution.  Writable application state
 *   must have an explicit root and must never depend on the caller's current
 *   working directory.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Verify the reusable application-path contract with an isolated temporary root. */
static void TestApplicationPaths(const char *testRoot)
{
    UmiApplicationPathsConfig config =
        UmiApplicationPathsConfigDefault("Studio");
    UmiApplicationPaths paths;
    UmiApplicationPathsConfig invalidConfig;
    char expected[UMI_PATH_CAPACITY];

    /* The normal operating-system location is absolute and independent from
     * the directory used to launch the test process. */
    assert(UmiApplicationPathsResolve(&config, &paths) == UMI_STATUS_OK);
    assert(umi_path_is_absolute(paths.root));

    config.baseOverride = testRoot;
    assert(UmiApplicationPathsResolve(&config, &paths) == UMI_STATUS_OK);
    assert(paths.structSize == (uint32_t)sizeof(paths));
    assert(paths.apiVersion == UMI_APPLICATION_PATHS_API_VERSION);

    assert(umi_path_join(testRoot, "Umicom", expected, sizeof(expected)) ==
           UMI_STATUS_OK);
    assert(umi_path_join(expected, "Studio", expected, sizeof(expected)) ==
           UMI_STATUS_OK);
    assert(umi_path_equal(paths.root, expected));
    assert(umi_path_is_within(paths.root, paths.config));
    assert(umi_path_is_within(paths.root, paths.state));
    assert(umi_path_is_within(paths.root, paths.cache));
    assert(umi_path_is_within(paths.root, paths.data));
    assert(umi_path_is_within(paths.root, paths.logs));
    assert(umi_path_is_within(paths.root, paths.recovery));

    assert(UmiApplicationPathsPrepare(&paths) == UMI_STATUS_OK);
    assert(umi_fs_is_directory(paths.root));
    assert(umi_fs_is_directory(paths.config));
    assert(umi_fs_is_directory(paths.state));
    assert(umi_fs_is_directory(paths.cache));
    assert(umi_fs_is_directory(paths.data));
    assert(umi_fs_is_directory(paths.logs));
    assert(umi_fs_is_directory(paths.recovery));

    invalidConfig = UmiApplicationPathsConfigDefault("../Studio");
    invalidConfig.baseOverride = testRoot;
    assert(UmiApplicationPathsResolve(&invalidConfig, &paths) ==
           UMI_STATUS_INVALID_ARGUMENT);

    invalidConfig = UmiApplicationPathsConfigDefault("Studio");
    invalidConfig.baseOverride = "relative-state-root";
    assert(UmiApplicationPathsResolve(&invalidConfig, &paths) ==
           UMI_STATUS_INVALID_ARGUMENT);
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    char temp_root[UMI_PATH_CAPACITY];
    char test_root[UMI_PATH_CAPACITY];
    char session_path[UMI_PATH_CAPACITY];
    char recovery_root[UMI_PATH_CAPACITY];
    char value[UMI_SESSION_VALUE_CAPACITY];
    UmiSessionStore *session = NULL;
    UmiSessionStore *loaded = NULL;
    UmiRecoveryManager *recovery = NULL;
    UmiRecoveryRecord record;
    int was_loaded = 0;

    assert(umi_fs_temp_directory(temp_root, sizeof(temp_root)) == UMI_STATUS_OK);
    assert(umi_fs_join(test_root,
                       sizeof(test_root),
                       temp_root,
                       "umicom-session-recovery-test") == UMI_STATUS_OK);
    assert(umi_fs_remove_tree(test_root) == UMI_STATUS_OK);
    assert(umi_fs_make_directories(test_root) == UMI_STATUS_OK);

    TestApplicationPaths(test_root);

    assert(umi_fs_join(session_path,
                       sizeof(session_path),
                       test_root,
                       "studio.session") == UMI_STATUS_OK);
    assert(umi_fs_join(recovery_root,
                       sizeof(recovery_root),
                       test_root,
                       "recovery") == UMI_STATUS_OK);

    assert(umi_session_store_create(&session) == UMI_STATUS_OK);
    assert(umi_session_store_set(session,
                                 "workspace.root",
                                 "C:/Dev/umicom") == UMI_STATUS_OK);
    assert(umi_session_store_set(session,
                                 "editor.active",
                                 "example.c") == UMI_STATUS_OK);
    assert(umi_session_store_save(session,
                                  session_path) == UMI_STATUS_OK);

    assert(umi_session_store_create(&loaded) == UMI_STATUS_OK);
    assert(umi_session_store_load(loaded,
                                  session_path,
                                  &was_loaded) == UMI_STATUS_OK);
    assert(was_loaded);
    assert(umi_session_store_get(loaded,
                                 "workspace.root",
                                 value,
                                 sizeof(value)) == UMI_STATUS_OK);
    assert(strcmp(value, "C:/Dev/umicom") == 0);

    assert(umi_recovery_manager_create(recovery_root,
                                       &recovery) == UMI_STATUS_OK);
    assert(umi_recovery_manager_save(recovery,
                                     77U,
                                     "C:/Dev/umicom/example.c",
                                     9U,
                                     "unsaved text",
                                     12U) == UMI_STATUS_OK);
    assert(umi_recovery_manager_exists(recovery, 77U));
    assert(umi_recovery_manager_load(recovery,
                                     77U,
                                     &record) == UMI_STATUS_OK);
    assert(record.document_id == 77U);
    assert(record.revision == 9U);
    assert(strcmp(record.source_path,
                  "C:/Dev/umicom/example.c") == 0);
    assert(strcmp(record.text, "unsaved text") == 0);
    umi_recovery_record_dispose(&record);

    assert(umi_recovery_manager_remove(recovery, 77U) == UMI_STATUS_OK);
    assert(!umi_recovery_manager_exists(recovery, 77U));

    umi_recovery_manager_destroy(recovery);
    umi_session_store_destroy(loaded);
    umi_session_store_destroy(session);
    assert(umi_fs_remove_tree(test_root) == UMI_STATUS_OK);
    (void)printf("session and recovery contracts passed\n");
    return 0;
}
