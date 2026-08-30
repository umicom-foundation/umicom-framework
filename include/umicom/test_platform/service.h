/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/service.h
 *
 * PURPOSE:
 *   Define the reusable test platform aggregating discovery, hierarchy, run profiles, sessions, results, output, coverage and benchmarks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef FRAMEWORK_INCLUDE_UMICOM_TEST_PLATFORM_SERVICE_H
#define FRAMEWORK_INCLUDE_UMICOM_TEST_PLATFORM_SERVICE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/test_platform/item.h"
#include "umicom/test_platform/suite.h"
#include "umicom/test_platform/run_profile.h"
#include "umicom/test_platform/run_session.h"
#include "umicom/test_platform/result.h"
#include "umicom/test_platform/output.h"
#include "umicom/test_platform/coverage.h"
#include "umicom/test_platform/discovery.h"
#include "umicom/test_platform/attachment.h"
#include "umicom/test_platform/benchmark.h"
#include "umicom/test_platform/filter.h"
#include "umicom/test_platform/hierarchy.h"
#include "umicom/test_platform/history.h"
#include "umicom/test_platform/operation.h"
#include "umicom/test_platform/artifact_contract.h"
#include "umicom/test_platform/ctest.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestPlatformService UmiTestPlatformService;
typedef struct UmiTestPlatformServiceSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t revision;
    size_t total_count;
    size_t item_count;
    size_t suite_count;
    size_t run_profile_count;
    size_t run_session_count;
    size_t result_count;
    size_t output_count;
    size_t coverage_count;
    size_t discovery_count;
    size_t attachment_count;
    size_t benchmark_count;
    size_t selected_count;
    int operation_running;
    int stop_requested;
} UmiTestPlatformServiceSnapshot;

UmiStatus umi_test_platform_service_create(UmiTestPlatformService **out_owner);
void umi_test_platform_service_destroy(UmiTestPlatformService *owner);
UmiStatus umi_test_platform_service_snapshot(const UmiTestPlatformService *owner, UmiTestPlatformServiceSnapshot *out_snapshot);
UmiTestPlatformItemRegistry *umi_test_platform_service_item(UmiTestPlatformService *owner);
UmiTestPlatformSuiteRegistry *umi_test_platform_service_suite(UmiTestPlatformService *owner);
UmiTestPlatformRunProfileRegistry *umi_test_platform_service_run_profile(UmiTestPlatformService *owner);
UmiTestPlatformRunSessionRegistry *umi_test_platform_service_run_session(UmiTestPlatformService *owner);
UmiTestPlatformResultRegistry *umi_test_platform_service_result(UmiTestPlatformService *owner);
UmiTestPlatformOutputRegistry *umi_test_platform_service_output(UmiTestPlatformService *owner);
UmiTestPlatformCoverageRegistry *umi_test_platform_service_coverage(UmiTestPlatformService *owner);
UmiTestPlatformDiscoveryRegistry *umi_test_platform_service_discovery(UmiTestPlatformService *owner);
UmiTestPlatformAttachmentRegistry *umi_test_platform_service_attachment(UmiTestPlatformService *owner);
UmiTestPlatformBenchmarkRegistry *umi_test_platform_service_benchmark(UmiTestPlatformService *owner);
UmiTestPlatformOperationController *umi_test_platform_service_operation(
    UmiTestPlatformService *owner
);
UmiStatus umi_test_platform_service_import_ctest_json(
    UmiTestPlatformService *owner,
    const char *json,
    const UmiTestPlatformCtestImportOptions *options,
    UmiTestPlatformCtestImportSummary *out_summary
);
UmiStatus umi_test_platform_service_discover_ctest(
    UmiTestPlatformService *owner,
    const UmiTestPlatformCtestImportOptions *options,
    UmiTestPlatformCtestImportSummary *out_summary,
    char *out_diagnostics,
    size_t diagnostics_capacity
);
UmiStatus umi_test_platform_service_select(
    UmiTestPlatformService *owner,
    const UmiTestPlatformFilter *filter,
    UmiTestPlatformSelection *out_selection
);
UmiStatus umi_test_platform_service_hierarchy(
    UmiTestPlatformService *owner,
    UmiTestPlatformHierarchyNode *nodes,
    size_t capacity,
    size_t *out_count
);
UmiStatus umi_test_platform_service_begin_operation(
    UmiTestPlatformService *owner,
    const UmiTestPlatformOperationPlan *plan
);
UmiStatus umi_test_platform_service_request_stop(UmiTestPlatformService *owner);
void umi_test_platform_service_finish_operation(UmiTestPlatformService *owner);

#ifdef __cplusplus
}
#endif
#endif
