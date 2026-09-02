/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_backup_restore_atomicity.c
 *
 * PURPOSE:
 *   Verify backup policy, validation-only restore, integrity rejection and
 *   atomic preservation of destination state when a backup is invalid.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

/*
 * Exercise seed source and return a clear result when the behaviour no longer matches its
 * contract.
 */
static int seed_source(UmiDataServer *server)
{
    TEST_STATUS_OK(umi_data_server_set(
        server,
        "workbench-layout/layout-manifest/layout.atomic",
        "manifest-layout-atomic"));
    TEST_STATUS_OK(umi_data_server_set(
        server,
        "workbench-layout/layout-chunk/layout.atomic/0",
        "layout-document-chunk"));
    TEST_STATUS_OK(umi_data_server_set(
        server,
        "workbench-layout/session-manifest/session.atomic",
        "session-manifest"));
    TEST_STATUS_OK(umi_data_server_set(
        server,
        "workbench-layout/session-chunk/session.atomic/0",
        "session-document-chunk"));
    TEST_STATUS_OK(umi_data_server_set(
        server,
        "workbench-layout/outbox/00000000000000000001/outbox.atomic",
        "outbox-entry"));
    return 0;
}

/*
 * Exercise create backup and return a clear result when the behaviour no longer matches
 * its contract.
 */
static int create_backup(
    UmiDataServer *source,
    const UmiWorkbenchLayoutBackupOptions *options,
    char **out_backup,
    size_t *out_length,
    UmiWorkbenchLayoutBackupManifest *out_manifest)
{
    UmiWorkbenchLayoutBackupResult sizing;
    UmiWorkbenchLayoutBackupResult written;
    char *buffer;

    TEST_REQUIRE(umi_workbench_layout_backup_create(
        source, "backup-atomic", "replica-source", 5000U,
        options, NULL, 0U, &sizing) == UMI_STATUS_CAPACITY_EXCEEDED,
        "backup reports required capacity");
    buffer = (char *)calloc(sizing.bytes_required + 1U, sizeof(char));
    TEST_REQUIRE(buffer != NULL, "allocate backup buffer");
    TEST_STATUS_OK(umi_workbench_layout_backup_create(
        source, "backup-atomic", "replica-source", 5000U,
        options, buffer, sizing.bytes_required + 1U, &written));
    TEST_STATUS_OK(umi_workbench_layout_backup_validate(
        buffer, written.bytes_written, out_manifest));
    *out_backup = buffer;
    *out_length = written.bytes_written;
    return 0;
}

/*
 * Exercise test validate only and policy filter and return a clear result when the
 * behaviour no longer matches its contract.
 */
static int test_validate_only_and_policy_filter(void)
{
    UmiDataServer *source = test_create_data_server();
    UmiDataServer *destination = test_create_data_server();
    UmiWorkbenchLayoutBackupOptions backup_options =
        umi_workbench_layout_backup_options_default();
    UmiWorkbenchLayoutRestoreOptions restore_options =
        umi_workbench_layout_restore_options_default();
    UmiWorkbenchLayoutRestoreResult result;
    UmiWorkbenchLayoutBackupManifest manifest;
    char *backup = NULL;
    size_t backup_length = 0U;
    char value[128];

    TEST_REQUIRE(source != NULL && destination != NULL,
                 "create source and destination");
    TEST_REQUIRE(seed_source(source) == 0, "seed source records");
    backup_options.include_sessions = false;
    backup_options.include_outbox = false;
    TEST_REQUIRE(create_backup(
        source, &backup_options, &backup, &backup_length, &manifest) == 0,
        "create policy-filtered backup");
    TEST_REQUIRE(!manifest.includes_sessions, "sessions excluded by policy");
    TEST_REQUIRE(!manifest.includes_outbox, "outbox excluded by policy");
    TEST_REQUIRE(strstr(backup, "session.atomic") == NULL,
                 "session records absent from stream");
    TEST_REQUIRE(strstr(backup, "outbox.atomic") == NULL,
                 "outbox records absent from stream");

    restore_options.validate_only = true;
    TEST_STATUS_OK(umi_workbench_layout_restore_apply(
        destination, backup, backup_length, &restore_options, &result));
    TEST_REQUIRE(result.parsed_count == manifest.record_count,
                 "validation parses every selected record");
    TEST_REQUIRE(result.restored_count == 0U,
                 "validation-only mode writes nothing");
    TEST_REQUIRE(umi_data_server_get(
        destination,
        "workbench-layout/layout-manifest/layout.atomic",
        value, sizeof(value)) == UMI_STATUS_NOT_FOUND,
        "validation-only destination remains unchanged");

    free(backup);
    umi_data_server_destroy(source);
    umi_data_server_destroy(destination);
    return 0;
}

/*
 * Exercise test restore and integrity failure preserves destination and return a clear
 * result when the behaviour no longer matches its contract.
 */
static int test_restore_and_integrity_failure_preserves_destination(void)
{
    UmiDataServer *source = test_create_data_server();
    UmiDataServer *destination = test_create_data_server();
    UmiWorkbenchLayoutBackupOptions backup_options =
        umi_workbench_layout_backup_options_default();
    UmiWorkbenchLayoutRestoreOptions restore_options =
        umi_workbench_layout_restore_options_default();
    UmiWorkbenchLayoutRestoreResult result;
    UmiWorkbenchLayoutBackupManifest manifest;
    char *backup = NULL;
    char *tampered = NULL;
    size_t backup_length = 0U;
    size_t tamper_index;
    size_t before_count;
    char value[128];

    TEST_REQUIRE(source != NULL && destination != NULL,
                 "create source and destination");
    TEST_REQUIRE(seed_source(source) == 0, "seed source records");
    TEST_REQUIRE(create_backup(
        source, &backup_options, &backup, &backup_length, &manifest) == 0,
        "create complete backup");

    TEST_STATUS_OK(umi_data_server_set(
        destination, "workbench-layout/local-marker", "preserve-me"));
    TEST_STATUS_OK(umi_workbench_layout_restore_apply(
        destination, backup, backup_length, &restore_options, &result));
    TEST_REQUIRE(result.restored_count == manifest.record_count,
                 "complete backup restored");
    TEST_STATUS_OK(umi_data_server_get(
        destination,
        "workbench-layout/layout-chunk/layout.atomic/0",
        value, sizeof(value)));
    TEST_REQUIRE(strcmp(value, "layout-document-chunk") == 0,
                 "layout payload restored");
    TEST_STATUS_OK(umi_data_server_get(
        destination, "workbench-layout/local-marker",
        value, sizeof(value)));
    TEST_REQUIRE(strcmp(value, "preserve-me") == 0,
                 "unrelated destination state preserved");

    before_count = umi_data_server_count(destination);
    tampered = (char *)calloc(backup_length + 1U, sizeof(char));
    TEST_REQUIRE(tampered != NULL, "allocate tampered backup");
    (void)memcpy(tampered, backup, backup_length);
    tamper_index = backup_length / 2U;
    tampered[tamper_index] = tampered[tamper_index] == 'x' ? 'y' : 'x';
    TEST_REQUIRE(umi_workbench_layout_restore_apply(
        destination, tampered, backup_length,
        &restore_options, &result) == UMI_STATUS_PARSE_ERROR,
        "integrity failure rejects restore");
    TEST_REQUIRE(umi_data_server_count(destination) == before_count,
                 "failed restore does not change record count");
    TEST_STATUS_OK(umi_data_server_get(
        destination, "workbench-layout/local-marker",
        value, sizeof(value)));
    TEST_REQUIRE(strcmp(value, "preserve-me") == 0,
                 "failed restore preserves pre-existing value");

    free(tampered);
    free(backup);
    umi_data_server_destroy(source);
    umi_data_server_destroy(destination);
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    TEST_REQUIRE(test_validate_only_and_policy_filter() == 0,
                 "validation and policy filtering");
    TEST_REQUIRE(
        test_restore_and_integrity_failure_preserves_destination() == 0,
        "restore integrity and atomic preservation");
    return 0;
}
