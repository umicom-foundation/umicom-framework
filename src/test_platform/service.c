/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/service.c
 *
 * PURPOSE:
 *   Implement the reusable test platform aggregating discovery, hierarchy, run profiles, sessions, results, output, coverage and benchmarks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#include "umicom/test_platform/service.h"

#include <stdlib.h>
#include <string.h>

struct UmiTestPlatformService {
    UmiTestPlatformItemRegistry *item;
    UmiTestPlatformSuiteRegistry *suite;
    UmiTestPlatformRunProfileRegistry *run_profile;
    UmiTestPlatformRunSessionRegistry *run_session;
    UmiTestPlatformResultRegistry *result;
    UmiTestPlatformOutputRegistry *output;
    UmiTestPlatformCoverageRegistry *coverage;
    UmiTestPlatformDiscoveryRegistry *discovery;
    UmiTestPlatformAttachmentRegistry *attachment;
    UmiTestPlatformBenchmarkRegistry *benchmark;
    UmiTestPlatformOperationController operation;
    size_t selected_count;
    uint64_t revision;
};

UmiStatus umi_test_platform_service_create(UmiTestPlatformService **out_owner)
{
    UmiTestPlatformService *owner; UmiStatus status = UMI_STATUS_OK;
    if (out_owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_owner = NULL; owner = (UmiTestPlatformService *)calloc(1U,sizeof(*owner));
    if (owner == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    owner->revision = 1U;
    umi_test_platform_operation_controller_init(&owner->operation);
    if (status == UMI_STATUS_OK) status = umi_test_platform_item_registry_create(&owner->item);
    if (status == UMI_STATUS_OK) status = umi_test_platform_suite_registry_create(&owner->suite);
    if (status == UMI_STATUS_OK) status = umi_test_platform_run_profile_registry_create(&owner->run_profile);
    if (status == UMI_STATUS_OK) status = umi_test_platform_run_session_registry_create(&owner->run_session);
    if (status == UMI_STATUS_OK) status = umi_test_platform_result_registry_create(&owner->result);
    if (status == UMI_STATUS_OK) status = umi_test_platform_output_registry_create(&owner->output);
    if (status == UMI_STATUS_OK) status = umi_test_platform_coverage_registry_create(&owner->coverage);
    if (status == UMI_STATUS_OK) status = umi_test_platform_discovery_registry_create(&owner->discovery);
    if (status == UMI_STATUS_OK) status = umi_test_platform_attachment_registry_create(&owner->attachment);
    if (status == UMI_STATUS_OK) status = umi_test_platform_benchmark_registry_create(&owner->benchmark);
    if (status != UMI_STATUS_OK) { umi_test_platform_service_destroy(owner); return status; }
    *out_owner = owner; return UMI_STATUS_OK;
}

void umi_test_platform_service_destroy(UmiTestPlatformService *owner)
{
    if (owner == NULL) return;
    umi_test_platform_benchmark_registry_destroy(owner->benchmark);
    umi_test_platform_attachment_registry_destroy(owner->attachment);
    umi_test_platform_discovery_registry_destroy(owner->discovery);
    umi_test_platform_coverage_registry_destroy(owner->coverage);
    umi_test_platform_output_registry_destroy(owner->output);
    umi_test_platform_result_registry_destroy(owner->result);
    umi_test_platform_run_session_registry_destroy(owner->run_session);
    umi_test_platform_run_profile_registry_destroy(owner->run_profile);
    umi_test_platform_suite_registry_destroy(owner->suite);
    umi_test_platform_item_registry_destroy(owner->item);
    free(owner);
}

UmiStatus umi_test_platform_service_snapshot(const UmiTestPlatformService *owner, UmiTestPlatformServiceSnapshot *out_snapshot)
{
    if (owner == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_snapshot,0,sizeof(*out_snapshot));
    out_snapshot->struct_size=(uint32_t)sizeof(*out_snapshot); out_snapshot->api_version=1U;
    out_snapshot->revision=owner->revision;
    out_snapshot->item_count = umi_test_platform_item_registry_count(owner->item);
    out_snapshot->suite_count = umi_test_platform_suite_registry_count(owner->suite);
    out_snapshot->run_profile_count = umi_test_platform_run_profile_registry_count(owner->run_profile);
    out_snapshot->run_session_count = umi_test_platform_run_session_registry_count(owner->run_session);
    out_snapshot->result_count = umi_test_platform_result_registry_count(owner->result);
    out_snapshot->output_count = umi_test_platform_output_registry_count(owner->output);
    out_snapshot->coverage_count = umi_test_platform_coverage_registry_count(owner->coverage);
    out_snapshot->discovery_count = umi_test_platform_discovery_registry_count(owner->discovery);
    out_snapshot->attachment_count = umi_test_platform_attachment_registry_count(owner->attachment);
    out_snapshot->benchmark_count = umi_test_platform_benchmark_registry_count(owner->benchmark);
    out_snapshot->selected_count = owner->selected_count;
    out_snapshot->operation_running = owner->operation.running;
    out_snapshot->stop_requested = owner->operation.stop_requested;
    out_snapshot->total_count = out_snapshot->item_count + out_snapshot->suite_count + out_snapshot->run_profile_count + out_snapshot->run_session_count + out_snapshot->result_count + out_snapshot->output_count + out_snapshot->coverage_count + out_snapshot->discovery_count + out_snapshot->attachment_count + out_snapshot->benchmark_count;
    return UMI_STATUS_OK;
}

UmiTestPlatformItemRegistry *umi_test_platform_service_item(UmiTestPlatformService *owner) { return owner != NULL ? owner->item : NULL; }
UmiTestPlatformSuiteRegistry *umi_test_platform_service_suite(UmiTestPlatformService *owner) { return owner != NULL ? owner->suite : NULL; }
UmiTestPlatformRunProfileRegistry *umi_test_platform_service_run_profile(UmiTestPlatformService *owner) { return owner != NULL ? owner->run_profile : NULL; }
UmiTestPlatformRunSessionRegistry *umi_test_platform_service_run_session(UmiTestPlatformService *owner) { return owner != NULL ? owner->run_session : NULL; }
UmiTestPlatformResultRegistry *umi_test_platform_service_result(UmiTestPlatformService *owner) { return owner != NULL ? owner->result : NULL; }
UmiTestPlatformOutputRegistry *umi_test_platform_service_output(UmiTestPlatformService *owner) { return owner != NULL ? owner->output : NULL; }
UmiTestPlatformCoverageRegistry *umi_test_platform_service_coverage(UmiTestPlatformService *owner) { return owner != NULL ? owner->coverage : NULL; }
UmiTestPlatformDiscoveryRegistry *umi_test_platform_service_discovery(UmiTestPlatformService *owner) { return owner != NULL ? owner->discovery : NULL; }
UmiTestPlatformAttachmentRegistry *umi_test_platform_service_attachment(UmiTestPlatformService *owner) { return owner != NULL ? owner->attachment : NULL; }
UmiTestPlatformBenchmarkRegistry *umi_test_platform_service_benchmark(UmiTestPlatformService *owner) { return owner != NULL ? owner->benchmark : NULL; }
UmiTestPlatformOperationController *umi_test_platform_service_operation(
    UmiTestPlatformService *owner)
{
    return owner != NULL ? &owner->operation : NULL;
}

UmiStatus umi_test_platform_service_import_ctest_json(
    UmiTestPlatformService *owner,
    const char *json,
    const UmiTestPlatformCtestImportOptions *options,
    UmiTestPlatformCtestImportSummary *out_summary)
{
    UmiStatus status;
    if (owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_test_platform_ctest_parse_json(
        json, options, owner->item, owner->suite, owner->discovery,
        out_summary);
    if (status == UMI_STATUS_OK) owner->revision += 1U;
    return status;
}

UmiStatus umi_test_platform_service_discover_ctest(
    UmiTestPlatformService *owner,
    const UmiTestPlatformCtestImportOptions *options,
    UmiTestPlatformCtestImportSummary *out_summary,
    char *out_diagnostics,
    size_t diagnostics_capacity)
{
    UmiStatus status;
    if (owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_test_platform_ctest_discover(
        options, owner->item, owner->suite, owner->discovery, out_summary,
        out_diagnostics, diagnostics_capacity);
    if (status == UMI_STATUS_OK) owner->revision += 1U;
    return status;
}

UmiStatus umi_test_platform_service_select(
    UmiTestPlatformService *owner,
    const UmiTestPlatformFilter *filter,
    UmiTestPlatformSelection *out_selection)
{
    UmiStatus status;
    if (owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_test_platform_filter_select(owner->item, owner->result,
                                             filter, out_selection);
    if (status == UMI_STATUS_OK) {
        owner->selected_count = out_selection->count;
        owner->revision += 1U;
    }
    return status;
}

UmiStatus umi_test_platform_service_hierarchy(
    UmiTestPlatformService *owner,
    UmiTestPlatformHierarchyNode *nodes,
    size_t capacity,
    size_t *out_count)
{
    if (owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_test_platform_hierarchy_build(owner->item, owner->result,
                                             nodes, capacity, out_count);
}

UmiStatus umi_test_platform_service_begin_operation(
    UmiTestPlatformService *owner,
    const UmiTestPlatformOperationPlan *plan)
{
    UmiStatus status;
    if (owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_test_platform_operation_begin(&owner->operation, plan);
    if (status == UMI_STATUS_OK) {
        owner->selected_count = plan->selection.count;
        owner->revision += 1U;
    }
    return status;
}

UmiStatus umi_test_platform_service_request_stop(UmiTestPlatformService *owner)
{
    UmiStatus status;
    if (owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_test_platform_operation_request_stop(&owner->operation);
    if (status == UMI_STATUS_OK) owner->revision += 1U;
    return status;
}

void umi_test_platform_service_finish_operation(UmiTestPlatformService *owner)
{
    if (owner == NULL) return;
    umi_test_platform_operation_finish(&owner->operation);
    owner->revision += 1U;
}
