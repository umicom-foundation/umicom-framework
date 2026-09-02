/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/explorer_session.h
 *
 * PURPOSE:
 *   Coordinate the complete professional Test Explorer experience over the
 *   canonical test-platform service and workspace: providers, explorer tree,
 *   rerun planning, coverage gates, benchmarks and retained history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_EXPLORER_SESSION_H
#define UMICOM_TEST_PLATFORM_EXPLORER_SESSION_H

#include "umicom/test_platform/benchmark_analysis.h"
#include "umicom/test_platform/coverage_analysis.h"
#include "umicom/test_platform/explorer_model.h"
#include "umicom/test_platform/history_analysis.h"
#include "umicom/test_platform/provider_registry.h"
#include "umicom/test_platform/rerun_policy.h"
#include "umicom/test_platform/workspace.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_EXPLORER_SESSION_API_VERSION 1U

/**
 * List the named test explorer session state values accepted by this public contract.
 */
typedef enum UmiTestExplorerSessionState {
    UMI_TEST_EXPLORER_SESSION_EMPTY = 0,
    UMI_TEST_EXPLORER_SESSION_READY = 1,
    UMI_TEST_EXPLORER_SESSION_DISCOVERING = 2,
    UMI_TEST_EXPLORER_SESSION_RUNNING = 3,
    UMI_TEST_EXPLORER_SESSION_STOPPING = 4,
    UMI_TEST_EXPLORER_SESSION_COMPLETED = 5,
    UMI_TEST_EXPLORER_SESSION_CANCELLED = 6,
    UMI_TEST_EXPLORER_SESSION_FAILED = 7
} UmiTestExplorerSessionState;

/**
 * Represent the test explorer session snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiTestExplorerSessionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiTestExplorerSessionState state;
    UmiTestPlatformServiceSnapshot service;
    UmiTestWorkspaceSnapshot workspace;
    UmiTestExplorerModelSnapshot explorer;
    UmiTestRerunPlanSnapshot rerun;
    UmiTestCoverageAnalysisSnapshot coverage;
    UmiTestBenchmarkAnalysisSnapshot benchmarks;
    UmiTestHistoryAnalysisSnapshot history;
    size_t provider_count;
    uint64_t refresh_generation;
    uint64_t revision;
    int owns_service;
    int owns_workspace;
    int quality_gate_passed;
    int can_refresh;
    int can_run;
    int can_stop;
} UmiTestExplorerSessionSnapshot;

/**
 * Represent the test explorer session data shared with callers of this public contract.
 */
typedef struct UmiTestExplorerSession UmiTestExplorerSession;

/**
 * Initialise test explorer session from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_test_explorer_session_create(
    UmiTestExplorerSession **out_session);
/**
 * Provide the test explorer session create bound operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_explorer_session_create_bound(
    UmiTestPlatformService *service,
    UmiTestWorkspace *workspace,
    UmiTestExplorerSession **out_session);
/**
 * Release or reset state held by test explorer session so the same storage can be reused
 * safely.
 */
void umi_test_explorer_session_destroy(UmiTestExplorerSession *session);
/**
 * Provide the test explorer session refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_explorer_session_refresh(
    UmiTestExplorerSession *session,
    const char *coverage_session_id);
/**
 * Provide the test explorer session set filter operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_explorer_session_set_filter(
    UmiTestExplorerSession *session,
    const UmiTestPlatformFilter *filter);
/**
 * Provide the test explorer session set sort operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_explorer_session_set_sort(
    UmiTestExplorerSession *session,
    UmiTestExplorerSortMode sort_mode);
/**
 * Provide the test explorer session set group operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_explorer_session_set_group(
    UmiTestExplorerSession *session,
    UmiTestExplorerGroupMode group_mode);
/**
 * Provide the test explorer session select operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_explorer_session_select(
    UmiTestExplorerSession *session,
    const char *item_id,
    int selected,
    int exclusive);
/**
 * Provide the test explorer session build rerun operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_explorer_session_build_rerun(
    UmiTestExplorerSession *session,
    const UmiTestRerunPolicy *policy);
/**
 * Provide the test explorer session begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_explorer_session_begin(
    UmiTestExplorerSession *session,
    const UmiTestPlatformOperationPlan *plan);
/**
 * Provide the test explorer session request stop operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_explorer_session_request_stop(
    UmiTestExplorerSession *session);
/**
 * Provide the test explorer session finish operation used by this module and its client
 * applications.
 */
void umi_test_explorer_session_finish(
    UmiTestExplorerSession *session,
    int cancelled,
    int failed);
/**
 * Provide the test explorer session snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_explorer_session_snapshot(
    UmiTestExplorerSession *session,
    UmiTestExplorerSessionSnapshot *out_snapshot);
/**
 * Provide the test explorer session providers operation used by this module and its client
 * applications.
 */
UmiTestPlatformProviderRegistry *umi_test_explorer_session_providers(
    UmiTestExplorerSession *session);
/**
 * Provide the test explorer session model operation used by this module and its client
 * applications.
 */
UmiTestExplorerModel *umi_test_explorer_session_model(
    UmiTestExplorerSession *session);
/**
 * Provide the test explorer session rerun operation used by this module and its client
 * applications.
 */
UmiTestRerunPlan *umi_test_explorer_session_rerun(
    UmiTestExplorerSession *session);
/**
 * Provide the test explorer session coverage operation used by this module and its client
 * applications.
 */
UmiTestCoverageAnalysis *umi_test_explorer_session_coverage(
    UmiTestExplorerSession *session);
/**
 * Provide the test explorer session benchmarks operation used by this module and its
 * client applications.
 */
UmiTestBenchmarkAnalysis *umi_test_explorer_session_benchmarks(
    UmiTestExplorerSession *session);
/**
 * Provide the test explorer session history operation used by this module and its client
 * applications.
 */
UmiTestHistoryAnalysis *umi_test_explorer_session_history(
    UmiTestExplorerSession *session);
/**
 * Provide the test explorer session state text operation used by this module and its
 * client applications.
 */
const char *umi_test_explorer_session_state_text(
    UmiTestExplorerSessionState state);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_TEST_PLATFORM_EXPLORER_SESSION_H */
