/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_restore.c
 *
 * PURPOSE:
 *   Verify transactional backup restoration, validation-only mode, replacement policy and selective transient-record recovery.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

static int create_backup(
    UmiDataServer *source,
    char **out_backup,
    size_t *out_length)
{
    UmiWorkbenchLayoutBackupOptions options =
        umi_workbench_layout_backup_options_default();
    UmiWorkbenchLayoutBackupResult sizing;
    UmiWorkbenchLayoutBackupResult written;
    char *buffer;
    TEST_STATUS_OK(umi_data_server_set(
        source, "workbench-layout/layout-manifest/layout.restore",
        "manifest-value"));
    TEST_STATUS_OK(umi_data_server_set(
        source, "workbench-layout/layout-chunk/layout.restore/0",
        "layout-value"));
    TEST_STATUS_OK(umi_data_server_set(
        source, "workbench-layout/session-manifest/session.restore",
        "session-value"));
    (void)umi_workbench_layout_backup_create(
        source, "restore-backup", "source", 1000U,
        &options, NULL, 0U, &sizing);
    buffer = (char *)calloc(sizing.bytes_required + 1U, sizeof(char));
    if (buffer == NULL) return 1;
    TEST_STATUS_OK(umi_workbench_layout_backup_create(
        source, "restore-backup", "source", 1000U,
        &options, buffer, sizing.bytes_required + 1U, &written));
    *out_backup = buffer;
    *out_length = written.bytes_written;
    return 0;
}

static int test_validation_only(void)
{
    UmiDataServer *source = test_create_data_server();
    UmiDataServer *destination = test_create_data_server();
    UmiWorkbenchLayoutRestoreOptions options =
        umi_workbench_layout_restore_options_default();
    UmiWorkbenchLayoutRestoreResult result;
    char *backup = NULL;
    size_t length = 0U;
    char value[64];
    TEST_REQUIRE(source != NULL && destination != NULL,
                 "create servers");
    TEST_REQUIRE(create_backup(source, &backup, &length) == 0,
                 "create backup");
    options.validate_only = true;
    TEST_STATUS_OK(umi_workbench_layout_restore_apply(
        destination, backup, length, &options, &result));
    TEST_REQUIRE(result.parsed_count > 0U,
                 "validation parsed records");
    TEST_REQUIRE(result.restored_count == 0U,
                 "validation writes nothing");
    TEST_REQUIRE(umi_data_server_get(
        destination,
        "workbench-layout/layout-manifest/layout.restore",
        value, sizeof(value)) == UMI_STATUS_NOT_FOUND,
        "destination unchanged");
    free(backup);
    umi_data_server_destroy(source);
    umi_data_server_destroy(destination);
    return 0;
}

static int test_restore_new_records(void)
{
    UmiDataServer *source = test_create_data_server();
    UmiDataServer *destination = test_create_data_server();
    UmiWorkbenchLayoutRestoreOptions options =
        umi_workbench_layout_restore_options_default();
    UmiWorkbenchLayoutRestoreResult result;
    char *backup = NULL;
    size_t length = 0U;
    char value[64];
    TEST_REQUIRE(source != NULL && destination != NULL,
                 "create servers");
    TEST_REQUIRE(create_backup(source, &backup, &length) == 0,
                 "create backup");
    TEST_STATUS_OK(umi_workbench_layout_restore_apply(
        destination, backup, length, &options, &result));
    TEST_REQUIRE(result.restored_count > 0U,
                 "records restored");
    TEST_STATUS_OK(umi_data_server_get(
        destination,
        "workbench-layout/layout-chunk/layout.restore/0",
        value, sizeof(value)));
    TEST_REQUIRE(strcmp(value, "layout-value") == 0,
                 "layout payload restored");
    free(backup);
    umi_data_server_destroy(source);
    umi_data_server_destroy(destination);
    return 0;
}

static int test_existing_record_policy(void)
{
    UmiDataServer *source = test_create_data_server();
    UmiDataServer *destination = test_create_data_server();
    UmiWorkbenchLayoutRestoreOptions options =
        umi_workbench_layout_restore_options_default();
    UmiWorkbenchLayoutRestoreResult result;
    char *backup = NULL;
    size_t length = 0U;
    char value[64];
    TEST_REQUIRE(source != NULL && destination != NULL,
                 "create servers");
    TEST_REQUIRE(create_backup(source, &backup, &length) == 0,
                 "create backup");
    TEST_STATUS_OK(umi_data_server_set(
        destination,
        "workbench-layout/layout-chunk/layout.restore/0",
        "existing"));
    options.replace_existing = false;
    TEST_STATUS_OK(umi_workbench_layout_restore_apply(
        destination, backup, length, &options, &result));
    TEST_STATUS_OK(umi_data_server_get(
        destination,
        "workbench-layout/layout-chunk/layout.restore/0",
        value, sizeof(value)));
    TEST_REQUIRE(strcmp(value, "existing") == 0,
                 "existing record preserved");
    TEST_REQUIRE(result.skipped_count > 0U,
                 "existing record counted as skipped");
    options.replace_existing = true;
    TEST_STATUS_OK(umi_workbench_layout_restore_apply(
        destination, backup, length, &options, &result));
    TEST_STATUS_OK(umi_data_server_get(
        destination,
        "workbench-layout/layout-chunk/layout.restore/0",
        value, sizeof(value)));
    TEST_REQUIRE(strcmp(value, "layout-value") == 0,
                 "existing record replaced");
    free(backup);
    umi_data_server_destroy(source);
    umi_data_server_destroy(destination);
    return 0;
}

static int test_invalid_backup_rollback(void)
{
    UmiDataServer *destination = test_create_data_server();
    UmiWorkbenchLayoutRestoreOptions options =
        umi_workbench_layout_restore_options_default();
    UmiWorkbenchLayoutRestoreResult result;
    TEST_REQUIRE(destination != NULL, "create server");
    TEST_REQUIRE(umi_workbench_layout_restore_apply(
        destination, "invalid-backup", strlen("invalid-backup"),
        &options, &result) == UMI_STATUS_PARSE_ERROR,
        "invalid backup rejected");
    TEST_REQUIRE(umi_data_server_count(destination) == 0U,
                 "failed restore writes nothing");
    umi_data_server_destroy(destination);
    return 0;
}

int main(void)
{
    TEST_REQUIRE(test_validation_only() == 0, "validation only");
    TEST_REQUIRE(test_restore_new_records() == 0, "restore records");
    TEST_REQUIRE(test_existing_record_policy() == 0,
                 "existing policy");
    TEST_REQUIRE(test_invalid_backup_rollback() == 0,
                 "invalid rollback");
    return 0;
}
