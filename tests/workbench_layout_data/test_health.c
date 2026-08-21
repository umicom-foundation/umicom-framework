/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_health.c
 *
 * PURPOSE:
 *   Verify readiness and liveness derivation from Data Server availability, queue pressure, conflicts and recent failures.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

static int test_healthy_snapshot(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataMetrics metrics;
    UmiWorkbenchLayoutHealthPolicy policy =
        umi_workbench_layout_health_policy_default();
    UmiWorkbenchLayoutDataHealthSnapshot snapshot;
    TEST_REQUIRE(server != NULL, "create server");
    (void)memset(&metrics, 0, sizeof(metrics));
    metrics.structure_size = sizeof(metrics);
    TEST_STATUS_OK(umi_workbench_layout_health_evaluate(
        server, &metrics, 0U, 0U, 0U, 1U,
        5000U, 4900U, 0U, &policy, &snapshot));
    TEST_REQUIRE(snapshot.state ==
        UMI_WORKBENCH_LAYOUT_DATA_HEALTH_HEALTHY,
        "healthy state");
    TEST_REQUIRE(umi_workbench_layout_health_ready(&snapshot),
                 "healthy service ready");
    TEST_REQUIRE(umi_workbench_layout_health_live(&snapshot),
                 "healthy service live");
    umi_data_server_destroy(server);
    return 0;
}

static int test_degraded_pressure(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataMetrics metrics;
    UmiWorkbenchLayoutHealthPolicy policy =
        umi_workbench_layout_health_policy_default();
    UmiWorkbenchLayoutDataHealthSnapshot snapshot;
    TEST_REQUIRE(server != NULL, "create server");
    (void)memset(&metrics, 0, sizeof(metrics));
    metrics.structure_size = sizeof(metrics);
    TEST_STATUS_OK(umi_workbench_layout_health_evaluate(
        server, &metrics,
        policy.maximum_pending_outbox + 1U,
        0U, 0U, 0U,
        5000U, 4900U, 0U, &policy, &snapshot));
    TEST_REQUIRE(snapshot.state ==
        UMI_WORKBENCH_LAYOUT_DATA_HEALTH_DEGRADED,
        "queue pressure degrades state");
    TEST_REQUIRE(umi_workbench_layout_health_ready(&snapshot),
                 "degraded service remains ready");
    TEST_REQUIRE(umi_workbench_layout_health_live(&snapshot),
                 "degraded service remains live");
    umi_data_server_destroy(server);
    return 0;
}

static int test_unhealthy_conflicts(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataMetrics metrics;
    UmiWorkbenchLayoutHealthPolicy policy =
        umi_workbench_layout_health_policy_default();
    UmiWorkbenchLayoutDataHealthSnapshot snapshot;
    TEST_REQUIRE(server != NULL, "create server");
    (void)memset(&metrics, 0, sizeof(metrics));
    metrics.structure_size = sizeof(metrics);
    TEST_STATUS_OK(umi_workbench_layout_health_evaluate(
        server, &metrics, 0U,
        policy.maximum_open_conflicts + 1U,
        0U, 0U, 5000U, 4900U, 0U,
        &policy, &snapshot));
    TEST_REQUIRE(snapshot.state ==
        UMI_WORKBENCH_LAYOUT_DATA_HEALTH_UNHEALTHY,
        "conflict pressure unhealthy");
    TEST_REQUIRE(!umi_workbench_layout_health_ready(&snapshot),
                 "unhealthy service not ready");
    TEST_REQUIRE(umi_workbench_layout_health_live(&snapshot),
                 "unhealthy service still live");
    umi_data_server_destroy(server);
    return 0;
}

static int test_production_backend_policy(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataMetrics metrics;
    UmiWorkbenchLayoutHealthPolicy policy =
        umi_workbench_layout_health_policy_default();
    UmiWorkbenchLayoutDataHealthSnapshot snapshot;
    TEST_REQUIRE(server != NULL, "create server");
    (void)memset(&metrics, 0, sizeof(metrics));
    metrics.structure_size = sizeof(metrics);
    policy.require_sqlite_for_production = true;
    TEST_STATUS_OK(umi_workbench_layout_health_evaluate(
        server, &metrics, 0U, 0U, 0U, 0U,
        5000U, 4900U, 0U, &policy, &snapshot));
    TEST_REQUIRE(snapshot.state !=
        UMI_WORKBENCH_LAYOUT_DATA_HEALTH_HEALTHY,
        "memory backend not production healthy");
    umi_data_server_destroy(server);
    return 0;
}

static int test_invalid_health_arguments(void)
{
    UmiWorkbenchLayoutDataMetrics metrics;
    UmiWorkbenchLayoutHealthPolicy policy =
        umi_workbench_layout_health_policy_default();
    UmiWorkbenchLayoutDataHealthSnapshot snapshot;
    (void)memset(&metrics, 0, sizeof(metrics));
    metrics.structure_size = sizeof(metrics);
    TEST_REQUIRE(umi_workbench_layout_health_evaluate(
        NULL, &metrics, 0U, 0U, 0U, 0U,
        0U, 0U, 0U, &policy, &snapshot) ==
        UMI_STATUS_INVALID_ARGUMENT, "null server rejected");
    return 0;
}

int main(void)
{
    TEST_REQUIRE(test_healthy_snapshot() == 0, "healthy snapshot");
    TEST_REQUIRE(test_degraded_pressure() == 0,
                 "degraded pressure");
    TEST_REQUIRE(test_unhealthy_conflicts() == 0,
                 "unhealthy conflicts");
    TEST_REQUIRE(test_production_backend_policy() == 0,
                 "production backend");
    TEST_REQUIRE(test_invalid_health_arguments() == 0,
                 "invalid health");
    return 0;
}
