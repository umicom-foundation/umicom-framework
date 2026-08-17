/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_incremental.c
 *
 * PURPOSE:
 *   Prove direct revision changes invalidate dependency consumers in stable
 *   dependency-first order and malformed cycles are rejected.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/build/incremental.h"

int main(void)
{
    UmiBuildIncrementalPlanner *planner = NULL;
    UmiBuildIncrementalNodeEvidence configure;
    UmiBuildIncrementalNodeEvidence build;
    UmiBuildIncrementalNodeEvidence test;
    UmiBuildIncrementalPlanItem item;
    UmiBuildIncrementalSnapshot snapshot;

    assert(umi_build_incremental_planner_create(&planner) == UMI_STATUS_OK);
    umi_build_incremental_evidence_init(
        &configure, "configure", "Configure", UMI_BUILD_PHASE_CONFIGURE);
    umi_build_incremental_evidence_init(
        &build, "build", "Build", UMI_BUILD_PHASE_BUILD);
    umi_build_incremental_evidence_init(
        &test, "test", "Test", UMI_BUILD_PHASE_TEST);
    configure.input_revision = 8U;
    configure.completed_input_revision = 7U;
    configure.estimated_duration_ms = 100U;
    build.input_revision = build.completed_input_revision = 4U;
    build.estimated_duration_ms = 200U;
    test.input_revision = test.completed_input_revision = 2U;
    test.estimated_duration_ms = 300U;

    assert(umi_build_incremental_planner_upsert(planner, &configure) ==
           UMI_STATUS_OK);
    assert(umi_build_incremental_planner_upsert(planner, &build) ==
           UMI_STATUS_OK);
    assert(umi_build_incremental_planner_upsert(planner, &test) ==
           UMI_STATUS_OK);
    assert(umi_build_incremental_planner_add_dependency(
               planner, "build", "configure") == UMI_STATUS_OK);
    assert(umi_build_incremental_planner_add_dependency(
               planner, "test", "build") == UMI_STATUS_OK);
    assert(umi_build_incremental_planner_evaluate(planner, 44U) ==
           UMI_STATUS_OK);
    assert(umi_build_incremental_planner_count(planner) == 3U);
    assert(umi_build_incremental_planner_item_at(planner, 0U, &item) ==
           UMI_STATUS_OK);
    assert(strcmp(item.node_id, "configure") == 0);
    assert(item.directly_invalidated != 0);
    assert(umi_build_incremental_planner_item_at(planner, 1U, &item) ==
           UMI_STATUS_OK);
    assert(strcmp(item.node_id, "build") == 0);
    assert(item.inherited_invalidation != 0);
    assert(umi_build_incremental_planner_item_at(planner, 2U, &item) ==
           UMI_STATUS_OK);
    assert(strcmp(item.node_id, "test") == 0);
    assert(umi_build_incremental_planner_snapshot(planner, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.direct_count == 1U);
    assert(snapshot.inherited_count == 2U);
    assert(snapshot.estimated_duration_ms == 600U);

    assert(umi_build_incremental_planner_add_dependency(
               planner, "configure", "test") == UMI_STATUS_OK);
    assert(umi_build_incremental_planner_evaluate(planner, 45U) ==
           UMI_STATUS_INVALID_STATE);
    assert(umi_build_incremental_planner_snapshot(planner, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.cyclic != 0);

    umi_build_incremental_planner_destroy(planner);
    return 0;
}
