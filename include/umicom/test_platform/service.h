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

/*
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

/**
 * Represent the test platform service data shared with callers of this public contract.
 */
typedef struct UmiTestPlatformService UmiTestPlatformService;
/**
 * Represent the test platform service snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Initialise test platform service from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_test_platform_service_create(UmiTestPlatformService **out_owner);
/**
 * Release or reset state held by test platform service so the same storage can be reused
 * safely.
 */
void umi_test_platform_service_destroy(UmiTestPlatformService *owner);
/**
 * Provide the test platform service snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_platform_service_snapshot(const UmiTestPlatformService *owner, UmiTestPlatformServiceSnapshot *out_snapshot);
/**
 * Provide the test platform service item operation used by this module and its client
 * applications.
 */
UmiTestPlatformItemRegistry *umi_test_platform_service_item(UmiTestPlatformService *owner);
/**
 * Provide the test platform service suite operation used by this module and its client
 * applications.
 */
UmiTestPlatformSuiteRegistry *umi_test_platform_service_suite(UmiTestPlatformService *owner);
/**
 * Provide the test platform service run profile operation used by this module and its
 * client applications.
 */
UmiTestPlatformRunProfileRegistry *umi_test_platform_service_run_profile(UmiTestPlatformService *owner);
/**
 * Provide the test platform service run session operation used by this module and its
 * client applications.
 */
UmiTestPlatformRunSessionRegistry *umi_test_platform_service_run_session(UmiTestPlatformService *owner);
/**
 * Provide the test platform service result operation used by this module and its client
 * applications.
 */
UmiTestPlatformResultRegistry *umi_test_platform_service_result(UmiTestPlatformService *owner);
/**
 * Provide the test platform service output operation used by this module and its client
 * applications.
 */
UmiTestPlatformOutputRegistry *umi_test_platform_service_output(UmiTestPlatformService *owner);
/**
 * Provide the test platform service coverage operation used by this module and its client
 * applications.
 */
UmiTestPlatformCoverageRegistry *umi_test_platform_service_coverage(UmiTestPlatformService *owner);
/**
 * Provide the test platform service discovery operation used by this module and its client
 * applications.
 */
UmiTestPlatformDiscoveryRegistry *umi_test_platform_service_discovery(UmiTestPlatformService *owner);
/**
 * Provide the test platform service attachment operation used by this module and its
 * client applications.
 */
UmiTestPlatformAttachmentRegistry *umi_test_platform_service_attachment(UmiTestPlatformService *owner);
/**
 * Provide the test platform service benchmark operation used by this module and its client
 * applications.
 */
UmiTestPlatformBenchmarkRegistry *umi_test_platform_service_benchmark(UmiTestPlatformService *owner);
/**
 * Provide the test platform service operation operation used by this module and its client
 * applications.
 */
UmiTestPlatformOperationController *umi_test_platform_service_operation(
    UmiTestPlatformService *owner
);
/**
 * Provide the test platform service import ctest json operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_platform_service_import_ctest_json(
    UmiTestPlatformService *owner,
    const char *json,
    const UmiTestPlatformCtestImportOptions *options,
    UmiTestPlatformCtestImportSummary *out_summary
);
/**
 * Provide the test platform service discover ctest operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_platform_service_discover_ctest(
    UmiTestPlatformService *owner,
    const UmiTestPlatformCtestImportOptions *options,
    UmiTestPlatformCtestImportSummary *out_summary,
    char *out_diagnostics,
    size_t diagnostics_capacity
);
/**
 * Provide the test platform service select operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_platform_service_select(
    UmiTestPlatformService *owner,
    const UmiTestPlatformFilter *filter,
    UmiTestPlatformSelection *out_selection
);
/**
 * Provide the test platform service hierarchy operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_platform_service_hierarchy(
    UmiTestPlatformService *owner,
    UmiTestPlatformHierarchyNode *nodes,
    size_t capacity,
    size_t *out_count
);
/**
 * Provide the test platform service begin operation operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_platform_service_begin_operation(
    UmiTestPlatformService *owner,
    const UmiTestPlatformOperationPlan *plan
);
/**
 * Provide the test platform service request stop operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_platform_service_request_stop(UmiTestPlatformService *owner);
/**
 * Provide the test platform service finish operation operation used by this module and its
 * client applications.
 */
void umi_test_platform_service_finish_operation(UmiTestPlatformService *owner);

#ifdef __cplusplus
}
#endif
#endif
