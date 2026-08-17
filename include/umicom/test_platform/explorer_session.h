/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/explorer_session.h
 *
 * PURPOSE:
 *   Coordinate the complete professional Test Explorer experience over the
 *   canonical test-platform service and workspace: providers, explorer tree,
 *   rerun planning, coverage gates, benchmarks and retained history.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiTestExplorerSession UmiTestExplorerSession;

UmiStatus umi_test_explorer_session_create(
    UmiTestExplorerSession **out_session);
UmiStatus umi_test_explorer_session_create_bound(
    UmiTestPlatformService *service,
    UmiTestWorkspace *workspace,
    UmiTestExplorerSession **out_session);
void umi_test_explorer_session_destroy(UmiTestExplorerSession *session);
UmiStatus umi_test_explorer_session_refresh(
    UmiTestExplorerSession *session,
    const char *coverage_session_id);
UmiStatus umi_test_explorer_session_set_filter(
    UmiTestExplorerSession *session,
    const UmiTestPlatformFilter *filter);
UmiStatus umi_test_explorer_session_set_sort(
    UmiTestExplorerSession *session,
    UmiTestExplorerSortMode sort_mode);
UmiStatus umi_test_explorer_session_set_group(
    UmiTestExplorerSession *session,
    UmiTestExplorerGroupMode group_mode);
UmiStatus umi_test_explorer_session_select(
    UmiTestExplorerSession *session,
    const char *item_id,
    int selected,
    int exclusive);
UmiStatus umi_test_explorer_session_build_rerun(
    UmiTestExplorerSession *session,
    const UmiTestRerunPolicy *policy);
UmiStatus umi_test_explorer_session_begin(
    UmiTestExplorerSession *session,
    const UmiTestPlatformOperationPlan *plan);
UmiStatus umi_test_explorer_session_request_stop(
    UmiTestExplorerSession *session);
void umi_test_explorer_session_finish(
    UmiTestExplorerSession *session,
    int cancelled,
    int failed);
UmiStatus umi_test_explorer_session_snapshot(
    UmiTestExplorerSession *session,
    UmiTestExplorerSessionSnapshot *out_snapshot);
UmiTestPlatformProviderRegistry *umi_test_explorer_session_providers(
    UmiTestExplorerSession *session);
UmiTestExplorerModel *umi_test_explorer_session_model(
    UmiTestExplorerSession *session);
UmiTestRerunPlan *umi_test_explorer_session_rerun(
    UmiTestExplorerSession *session);
UmiTestCoverageAnalysis *umi_test_explorer_session_coverage(
    UmiTestExplorerSession *session);
UmiTestBenchmarkAnalysis *umi_test_explorer_session_benchmarks(
    UmiTestExplorerSession *session);
UmiTestHistoryAnalysis *umi_test_explorer_session_history(
    UmiTestExplorerSession *session);
const char *umi_test_explorer_session_state_text(
    UmiTestExplorerSessionState state);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_TEST_PLATFORM_EXPLORER_SESSION_H */
