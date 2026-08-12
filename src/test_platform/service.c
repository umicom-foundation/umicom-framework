/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/service.c
 *
 * PURPOSE:
 *   Implement the reusable test platform aggregating discovery, hierarchy, run profiles, sessions, results, output, coverage and benchmarks.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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
    uint64_t revision;
};

UmiStatus umi_test_platform_service_create(UmiTestPlatformService **out_owner)
{
    UmiTestPlatformService *owner; UmiStatus status = UMI_STATUS_OK;
    if (out_owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_owner = NULL; owner = (UmiTestPlatformService *)calloc(1U,sizeof(*owner));
    if (owner == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    owner->revision = 1U;
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
