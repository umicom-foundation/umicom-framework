/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_backup.c
 *
 * PURPOSE:
 *   Verify deterministic policy-filtered backup streams, manifest integrity, size negotiation and tamper detection.
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
 * Exercise seed backup server and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int seed_backup_server(UmiDataServer *server)
{
    TEST_STATUS_OK(umi_data_server_set(
        server, "workbench-layout/layout-manifest/layout.a",
        "schema=manifest\\naggregate_id=layout.a\\n"));
    TEST_STATUS_OK(umi_data_server_set(
        server, "workbench-layout/layout-chunk/layout.a/0",
        "layout-payload"));
    TEST_STATUS_OK(umi_data_server_set(
        server, "workbench-layout/session-manifest/session.a",
        "session-manifest"));
    TEST_STATUS_OK(umi_data_server_set(
        server, "workbench-layout/session-chunk/session.a/0",
        "session-payload"));
    TEST_STATUS_OK(umi_data_server_set(
        server, "workbench-layout/outbox/00000000000000000001/outbox-1",
        "outbox-payload"));
    TEST_STATUS_OK(umi_data_server_set(
        server, "workbench-layout/presence/layout.a/presence-1",
        "presence-payload"));
    TEST_STATUS_OK(umi_data_server_set(
        server, "foreign/application/value", "not-owned"));
    return 0;
}

/*
 * Exercise test backup size negotiation and return a clear result when the behaviour no
 * longer matches its contract.
 */
static int test_backup_size_negotiation(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutBackupOptions options =
        umi_workbench_layout_backup_options_default();
    UmiWorkbenchLayoutBackupResult result;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_REQUIRE(seed_backup_server(server) == 0, "seed server");
    TEST_REQUIRE(umi_workbench_layout_backup_create(
        server, "backup-one", "replica-local", 5000U,
        &options, NULL, 0U, &result) ==
        UMI_STATUS_CAPACITY_EXCEEDED,
        "null buffer returns required capacity");
    TEST_REQUIRE(result.bytes_required > 0U,
                 "required capacity returned");
    TEST_REQUIRE(result.manifest.record_count > 0U,
                 "manifest record count calculated");
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test backup round trip validation and return a clear result when the behaviour
 * no longer matches its contract.
 */
static int test_backup_round_trip_validation(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutBackupOptions options =
        umi_workbench_layout_backup_options_default();
    UmiWorkbenchLayoutBackupResult sizing;
    UmiWorkbenchLayoutBackupResult written;
    UmiWorkbenchLayoutBackupManifest manifest;
    char *buffer;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_REQUIRE(seed_backup_server(server) == 0, "seed server");
    (void)umi_workbench_layout_backup_create(
        server, "backup-two", "replica-local", 5000U,
        &options, NULL, 0U, &sizing);
    buffer = (char *)calloc(sizing.bytes_required + 1U, sizeof(char));
    TEST_REQUIRE(buffer != NULL, "allocate backup buffer");
    TEST_STATUS_OK(umi_workbench_layout_backup_create(
        server, "backup-two", "replica-local", 5000U,
        &options, buffer, sizing.bytes_required + 1U, &written));
    TEST_REQUIRE(written.bytes_written > 0U,
                 "backup bytes written");
    TEST_REQUIRE(strstr(buffer, "UMICOM-WORKBENCH-LAYOUT-BACKUP") != NULL,
                 "backup signature present");
    TEST_STATUS_OK(umi_workbench_layout_backup_validate(
        buffer, written.bytes_written, &manifest));
    TEST_REQUIRE(strcmp(manifest.backup_id, "backup-two") == 0,
                 "backup id validated");
    TEST_REQUIRE(manifest.content_hash != 0U,
                 "backup integrity hash retained");
    free(buffer);
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test backup filtering and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_backup_filtering(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutBackupOptions options =
        umi_workbench_layout_backup_options_default();
    UmiWorkbenchLayoutBackupResult sizing;
    UmiWorkbenchLayoutBackupResult written;
    char *buffer;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_REQUIRE(seed_backup_server(server) == 0, "seed server");
    options.include_sessions = false;
    options.include_outbox = false;
    options.include_presence = false;
    (void)umi_workbench_layout_backup_create(
        server, "backup-filtered", "replica-local", 5000U,
        &options, NULL, 0U, &sizing);
    buffer = (char *)calloc(sizing.bytes_required + 1U, sizeof(char));
    TEST_REQUIRE(buffer != NULL, "allocate filtered backup");
    TEST_STATUS_OK(umi_workbench_layout_backup_create(
        server, "backup-filtered", "replica-local", 5000U,
        &options, buffer, sizing.bytes_required + 1U, &written));
    TEST_REQUIRE(strstr(buffer, "session-manifest") == NULL,
                 "sessions excluded");
    TEST_REQUIRE(strstr(buffer, "outbox/") == NULL,
                 "outbox excluded");
    TEST_REQUIRE(strstr(buffer, "presence/") == NULL,
                 "presence excluded");
    TEST_REQUIRE(strstr(buffer, "foreign/application") == NULL,
                 "foreign namespace excluded");
    free(buffer);
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test backup tamper detection and return a clear result when the behaviour no
 * longer matches its contract.
 */
static int test_backup_tamper_detection(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutBackupOptions options =
        umi_workbench_layout_backup_options_default();
    UmiWorkbenchLayoutBackupResult sizing;
    UmiWorkbenchLayoutBackupResult written;
    UmiWorkbenchLayoutBackupManifest manifest;
    char *buffer;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_REQUIRE(seed_backup_server(server) == 0, "seed server");
    (void)umi_workbench_layout_backup_create(
        server, "backup-tamper", "replica-local", 5000U,
        &options, NULL, 0U, &sizing);
    buffer = (char *)calloc(sizing.bytes_required + 1U, sizeof(char));
    TEST_REQUIRE(buffer != NULL, "allocate backup");
    TEST_STATUS_OK(umi_workbench_layout_backup_create(
        server, "backup-tamper", "replica-local", 5000U,
        &options, buffer, sizing.bytes_required + 1U, &written));
    buffer[written.bytes_written / 2U] =
        buffer[written.bytes_written / 2U] == 'x' ? 'y' : 'x';
    TEST_REQUIRE(umi_workbench_layout_backup_validate(
        buffer, written.bytes_written, &manifest) ==
        UMI_STATUS_PARSE_ERROR, "tampering detected");
    free(buffer);
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    TEST_REQUIRE(test_backup_size_negotiation() == 0,
                 "backup sizing");
    TEST_REQUIRE(test_backup_round_trip_validation() == 0,
                 "backup validation");
    TEST_REQUIRE(test_backup_filtering() == 0,
                 "backup filtering");
    TEST_REQUIRE(test_backup_tamper_detection() == 0,
                 "backup tamper");
    return 0;
}
