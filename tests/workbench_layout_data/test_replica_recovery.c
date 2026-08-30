/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_replica_recovery.c
 *
 * PURPOSE:
 *   Verify recovery of semantic layouts and active sessions into a replacement replica with health and revision evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

static int test_replacement_replica(void)
{
    UmiDataServer *primary_server = test_create_data_server();
    UmiDataServer *replacement_server = test_create_data_server();
    UmiWorkbenchLayoutDataService *primary = NULL;
    UmiWorkbenchLayoutDataService *replacement = NULL;
    UmiWorkbenchLayoutDataServiceConfig primary_config =
        umi_workbench_layout_data_service_config_default();
    UmiWorkbenchLayoutDataServiceConfig replacement_config =
        umi_workbench_layout_data_service_config_default();
    UmiWorkbenchLayoutDocument *document = test_allocate_layout(
        "layout.recovery");
    UmiWorkbenchLayoutSession session = test_create_session(
        "session.recovery", "layout.recovery");
    UmiWorkbenchLayoutDocument recovered;
    UmiWorkbenchLayoutDataServiceSnapshot snapshot;
    UmiWorkbenchLayoutBackupResult sizing;
    UmiWorkbenchLayoutBackupResult written;
    UmiWorkbenchLayoutRestoreOptions options =
        umi_workbench_layout_restore_options_default();
    UmiWorkbenchLayoutRestoreResult restore_result;
    uint64_t revision = 0U;
    char *backup;
    TEST_REQUIRE(primary_server != NULL && replacement_server != NULL &&
                 document != NULL, "create fixtures");
    test_copy_text(primary_config.replica_id,
                   sizeof(primary_config.replica_id), "primary");
    test_copy_text(primary_config.actor_id,
                   sizeof(primary_config.actor_id), "developer");
    test_copy_text(replacement_config.replica_id,
                   sizeof(replacement_config.replica_id), "replacement");
    test_copy_text(replacement_config.actor_id,
                   sizeof(replacement_config.actor_id), "recovery-service");
    TEST_STATUS_OK(umi_workbench_layout_data_service_create(
        primary_server, &primary_config, &primary));
    TEST_STATUS_OK(umi_workbench_layout_data_service_start(
        primary, 1000U));
    TEST_STATUS_OK(umi_workbench_layout_data_service_save_layout(
        primary, document, 0U, "save", 1100U, &revision));
    TEST_STATUS_OK(umi_workbench_layout_data_service_checkpoint_session(
        primary, &session, 0U, &revision));
    TEST_REQUIRE(umi_workbench_layout_data_service_backup(
        primary, "recovery-backup", 1200U,
        NULL, 0U, &sizing) == UMI_STATUS_CAPACITY_EXCEEDED,
        "recovery backup sizing");
    backup = (char *)calloc(sizing.bytes_required + 1U, sizeof(char));
    TEST_REQUIRE(backup != NULL, "allocate backup");
    TEST_STATUS_OK(umi_workbench_layout_data_service_backup(
        primary, "recovery-backup", 1200U,
        backup, sizing.bytes_required + 1U, &written));
    TEST_STATUS_OK(umi_workbench_layout_data_service_create(
        replacement_server, &replacement_config, &replacement));
    TEST_STATUS_OK(umi_workbench_layout_data_service_start(
        replacement, 1300U));
    TEST_STATUS_OK(umi_workbench_layout_data_service_restore(
        replacement, backup, written.bytes_written,
        &options, &restore_result));
    TEST_STATUS_OK(umi_workbench_layout_data_service_load_layout(
        replacement, "layout.recovery", &recovered));
    TEST_REQUIRE(recovered.content_hash == document->content_hash,
                 "layout content recovered");
    TEST_STATUS_OK(umi_workbench_layout_data_service_snapshot(
        replacement, 1400U, &snapshot));
    TEST_REQUIRE(snapshot.layout_count == 1U,
                 "replacement layout count");
    TEST_REQUIRE(snapshot.session_count == 1U,
                 "replacement session count");
    TEST_REQUIRE(snapshot.health.state !=
        UMI_WORKBENCH_LAYOUT_DATA_HEALTH_UNAVAILABLE,
        "replacement service available");
    free(backup);
    free(document);
    umi_workbench_layout_data_service_destroy(primary);
    umi_workbench_layout_data_service_destroy(replacement);
    umi_data_server_destroy(primary_server);
    umi_data_server_destroy(replacement_server);
    return 0;
}

int main(void)
{
    TEST_REQUIRE(test_replacement_replica() == 0,
                 "replacement replica");
    return 0;
}
