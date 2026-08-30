/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/workspace.h
 *
 * PURPOSE:
 *   Coordinate the selection, filtering, run mode and command availability
 *   required by a professional Test Explorer workspace.  Test records remain
 *   owned by UmiTestPlatformService; this layer adds reusable presentation
 *   state without copying the canonical registries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_PLATFORM_WORKSPACE_H
#define UMICOM_TEST_PLATFORM_WORKSPACE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/test_platform/service.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_WORKSPACE_API_VERSION 1U

typedef enum UmiTestWorkspaceRunMode {
    UMI_TEST_WORKSPACE_RUN = 0,
    UMI_TEST_WORKSPACE_DEBUG = 1,
    UMI_TEST_WORKSPACE_COVERAGE = 2
} UmiTestWorkspaceRunMode;

typedef struct UmiTestWorkspace UmiTestWorkspace;

typedef struct UmiTestWorkspaceSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t revision;
    UmiTestPlatformServiceSnapshot service;
    UmiTestPlatformFilter filter;
    UmiTestWorkspaceRunMode run_mode;
    char run_mode_label[64];
    char selected_item_id[128];
    char selected_session_id[128];
    size_t visible_item_count;
    size_t passed_count;
    size_t failed_count;
    size_t skipped_count;
    size_t cancelled_count;
    size_t timed_out_count;
    size_t not_run_count;
    size_t failure_result_count;
    uint64_t coverage_lines_total;
    uint64_t coverage_lines_covered;
    uint64_t coverage_branches_total;
    uint64_t coverage_branches_covered;
    uint32_t line_coverage_basis_points;
    uint32_t branch_coverage_basis_points;
    int can_discover;
    int can_run_all;
    int can_run_selected;
    int can_debug_selected;
    int can_run_with_coverage;
    int can_rerun_failed;
    int can_stop;
    int can_clear_results;
    int can_clear_output;
    int can_clear_coverage;
} UmiTestWorkspaceSnapshot;

UmiStatus umi_test_workspace_create(
    UmiTestPlatformService *service,
    UmiTestWorkspace **out_workspace
);
void umi_test_workspace_destroy(UmiTestWorkspace *workspace);
UmiStatus umi_test_workspace_refresh(UmiTestWorkspace *workspace);
UmiStatus umi_test_workspace_snapshot(
    UmiTestWorkspace *workspace,
    UmiTestWorkspaceSnapshot *out_snapshot
);

UmiStatus umi_test_workspace_set_filter(
    UmiTestWorkspace *workspace,
    const char *text,
    const char *suite_id,
    const char *label,
    int outcome,
    int include_disabled,
    int failed_only
);
UmiStatus umi_test_workspace_select_item(
    UmiTestWorkspace *workspace,
    const char *item_id
);
UmiStatus umi_test_workspace_select_session(
    UmiTestWorkspace *workspace,
    const char *session_id
);
UmiStatus umi_test_workspace_set_run_mode(
    UmiTestWorkspace *workspace,
    UmiTestWorkspaceRunMode mode
);

UmiStatus umi_test_workspace_plan_all(
    UmiTestWorkspace *workspace,
    UmiTestPlatformOperationPlan *out_plan
);
UmiStatus umi_test_workspace_plan_selected(
    UmiTestWorkspace *workspace,
    UmiTestPlatformOperationPlan *out_plan
);
UmiStatus umi_test_workspace_plan_failed(
    UmiTestWorkspace *workspace,
    UmiTestPlatformOperationPlan *out_plan
);
UmiStatus umi_test_workspace_plan_repeat_selected(
    UmiTestWorkspace *workspace,
    uint32_t repeat_count,
    int stop_on_failure,
    UmiTestPlatformOperationPlan *out_plan
);
UmiStatus umi_test_workspace_begin(
    UmiTestWorkspace *workspace,
    const UmiTestPlatformOperationPlan *plan
);
UmiStatus umi_test_workspace_request_stop(UmiTestWorkspace *workspace);
void umi_test_workspace_finish(UmiTestWorkspace *workspace);

void umi_test_workspace_clear_results(UmiTestWorkspace *workspace);
void umi_test_workspace_clear_output(UmiTestWorkspace *workspace);
void umi_test_workspace_clear_coverage(UmiTestWorkspace *workspace);

UmiStatus umi_test_workspace_visible_item_at(
    UmiTestWorkspace *workspace,
    size_t visible_index,
    UmiTestPlatformItemSnapshot *out_item,
    UmiTestPlatformResultSnapshot *out_latest_result
);
UmiStatus umi_test_workspace_result_at(
    UmiTestWorkspace *workspace,
    size_t newest_index,
    UmiTestPlatformResultSnapshot *out_result
);
UmiStatus umi_test_workspace_failure_at(
    UmiTestWorkspace *workspace,
    size_t newest_index,
    UmiTestPlatformResultSnapshot *out_result
);
UmiStatus umi_test_workspace_output_at(
    UmiTestWorkspace *workspace,
    size_t newest_index,
    UmiTestPlatformOutputSnapshot *out_output
);
UmiStatus umi_test_workspace_coverage_at(
    UmiTestWorkspace *workspace,
    size_t index,
    UmiTestPlatformCoverageSnapshot *out_coverage
);
UmiStatus umi_test_workspace_session_at(
    UmiTestWorkspace *workspace,
    size_t newest_index,
    UmiTestPlatformRunSessionSnapshot *out_session
);

const char *umi_test_workspace_run_mode_text(UmiTestWorkspaceRunMode mode);

#ifdef __cplusplus
}
#endif

#endif
