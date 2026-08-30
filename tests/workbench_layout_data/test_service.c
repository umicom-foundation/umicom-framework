/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_service.c
 *
 * PURPOSE:
 *   Verify the authoritative Framework service lifecycle, layout revisions, sharing, leases, sessions, backups and health snapshots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

static UmiWorkbenchLayoutDataService *create_started_service(
    UmiDataServer *server)
{
    UmiWorkbenchLayoutDataService *service = NULL;
    UmiWorkbenchLayoutDataServiceConfig config =
        umi_workbench_layout_data_service_config_default();
    test_copy_text(config.replica_id, sizeof(config.replica_id),
                   "replica-test");
    test_copy_text(config.actor_id, sizeof(config.actor_id),
                   "developer");
    if (umi_workbench_layout_data_service_create(
            server, &config, &service) != UMI_STATUS_OK) {
        return NULL;
    }
    if (umi_workbench_layout_data_service_start(service, 1000U) !=
        UMI_STATUS_OK) {
        umi_workbench_layout_data_service_destroy(service);
        return NULL;
    }
    return service;
}

static int test_service_lifecycle(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataService *service;
    UmiWorkbenchLayoutDataServiceSnapshot snapshot;
    TEST_REQUIRE(server != NULL, "create server");
    service = create_started_service(server);
    TEST_REQUIRE(service != NULL, "create service");
    TEST_STATUS_OK(umi_workbench_layout_data_service_snapshot(
        service, 1100U, &snapshot));
    TEST_REQUIRE(snapshot.started, "service started");
    TEST_REQUIRE(snapshot.accepting_writes,
                 "service accepts writes");
    TEST_REQUIRE(strcmp(snapshot.replica_id, "replica-test") == 0,
                 "replica identity retained");
    TEST_STATUS_OK(umi_workbench_layout_data_service_quiesce(service));
    TEST_STATUS_OK(umi_workbench_layout_data_service_snapshot(
        service, 1200U, &snapshot));
    TEST_REQUIRE(!snapshot.accepting_writes,
                 "quiesced service rejects writes");
    TEST_STATUS_OK(umi_workbench_layout_data_service_stop(
        service, 1300U));
    TEST_STATUS_OK(umi_workbench_layout_data_service_snapshot(
        service, 1400U, &snapshot));
    TEST_REQUIRE(!snapshot.started, "service stopped");
    umi_workbench_layout_data_service_destroy(service);
    umi_data_server_destroy(server);
    return 0;
}

static int test_layout_save_load_delete(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataService *service;
    UmiWorkbenchLayoutDocument *document = test_allocate_layout(
        "layout.service");
    UmiWorkbenchLayoutDocument loaded;
    UmiWorkbenchLayoutDataServiceSnapshot snapshot;
    uint64_t revision = 0U;
    TEST_REQUIRE(server != NULL && document != NULL,
                 "create fixture");
    service = create_started_service(server);
    TEST_REQUIRE(service != NULL, "create service");
    TEST_STATUS_OK(umi_workbench_layout_data_service_save_layout(
        service, document, 0U, "save-one", 2000U, &revision));
    TEST_REQUIRE(revision == 1U, "service revision assigned");
    TEST_STATUS_OK(umi_workbench_layout_data_service_load_layout(
        service, "layout.service", &loaded));
    TEST_REQUIRE(loaded.version.revision == 1U,
                 "service load revision");
    TEST_STATUS_OK(umi_workbench_layout_data_service_snapshot(
        service, 2100U, &snapshot));
    TEST_REQUIRE(snapshot.layout_count == 1U,
                 "layout count updated");
    TEST_REQUIRE(snapshot.metrics.save_count >= 1U,
                 "save metric updated");
    TEST_REQUIRE(snapshot.metrics.load_count >= 1U,
                 "load metric updated");
    TEST_STATUS_OK(umi_workbench_layout_data_service_delete_layout(
        service, "layout.service", revision,
        "delete-one", 2200U));
    TEST_REQUIRE(umi_workbench_layout_data_service_load_layout(
        service, "layout.service", &loaded) == UMI_STATUS_NOT_FOUND,
        "deleted layout absent");
    free(document);
    umi_workbench_layout_data_service_destroy(service);
    umi_data_server_destroy(server);
    return 0;
}

static int test_sharing_and_lease(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataService *service;
    UmiWorkbenchLayoutGrant grant = test_create_grant(
        "grant-service", "layout.service", "reviewer",
        (1U << UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_VIEW));
    UmiWorkbenchLayoutLease lease;
    TEST_REQUIRE(server != NULL, "create server");
    service = create_started_service(server);
    TEST_REQUIRE(service != NULL, "create service");
    TEST_STATUS_OK(umi_workbench_layout_data_service_share_layout(
        service, &grant, "share-one", 2000U));
    TEST_STATUS_OK(umi_workbench_layout_data_service_acquire_lease(
        service, "layout.service", "developer", "client-a",
        2100U, &lease));
    TEST_REQUIRE(lease.fencing_token > 0U,
                 "lease fencing token assigned");
    TEST_REQUIRE(lease.state ==
        UMI_WORKBENCH_LAYOUT_DATA_LEASE_HELD,
        "lease held");
    umi_workbench_layout_data_service_destroy(service);
    umi_data_server_destroy(server);
    return 0;
}

static int test_session_checkpoint(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataService *service;
    UmiWorkbenchLayoutSession session = test_create_session(
        "session.service", "layout.service");
    UmiWorkbenchLayoutDataServiceSnapshot snapshot;
    uint64_t revision = 0U;
    TEST_REQUIRE(server != NULL, "create server");
    service = create_started_service(server);
    TEST_REQUIRE(service != NULL, "create service");
    TEST_STATUS_OK(umi_workbench_layout_data_service_checkpoint_session(
        service, &session, 0U, &revision));
    TEST_REQUIRE(revision == 1U, "session checkpoint revision");
    TEST_STATUS_OK(umi_workbench_layout_data_service_snapshot(
        service, 2000U, &snapshot));
    TEST_REQUIRE(snapshot.session_count == 1U,
                 "session count updated");
    umi_workbench_layout_data_service_destroy(service);
    umi_data_server_destroy(server);
    return 0;
}

static int test_quiesced_write_rejected(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataService *service;
    UmiWorkbenchLayoutDocument *document = test_allocate_layout(
        "layout.quiesced");
    uint64_t revision = 0U;
    TEST_REQUIRE(server != NULL && document != NULL,
                 "create fixture");
    service = create_started_service(server);
    TEST_REQUIRE(service != NULL, "create service");
    TEST_STATUS_OK(umi_workbench_layout_data_service_quiesce(service));
    TEST_REQUIRE(umi_workbench_layout_data_service_save_layout(
        service, document, 0U, "save", 2000U, &revision) ==
        UMI_STATUS_INVALID_STATE, "quiesced write rejected");
    free(document);
    umi_workbench_layout_data_service_destroy(service);
    umi_data_server_destroy(server);
    return 0;
}

int main(void)
{
    TEST_REQUIRE(test_service_lifecycle() == 0,
                 "service lifecycle");
    TEST_REQUIRE(test_layout_save_load_delete() == 0,
                 "layout lifecycle");
    TEST_REQUIRE(test_sharing_and_lease() == 0,
                 "sharing lease");
    TEST_REQUIRE(test_session_checkpoint() == 0,
                 "session checkpoint");
    TEST_REQUIRE(test_quiesced_write_rejected() == 0,
                 "quiesced write");
    return 0;
}
