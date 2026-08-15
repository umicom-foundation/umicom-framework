/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_ui/test_ui.h
 *
 * PURPOSE:
 *   Define toolkit-neutral Test Explorer, Results, Failures, Output, Coverage
 *   and Runs view factories shared by every Umicom application and frontend.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_UI_TEST_UI_H
#define UMICOM_TEST_UI_TEST_UI_H

#include "umicom/test_platform/workspace.h"
#include "umicom/ui/view_model.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_UI_VISIBLE_ROWS 20U
#define UMI_TEST_UI_VIEW_EXPLORER "umicom.test.workspace.explorer"
#define UMI_TEST_UI_VIEW_RESULTS "umicom.test.workspace.results"
#define UMI_TEST_UI_VIEW_FAILURES "umicom.test.workspace.failures"
#define UMI_TEST_UI_VIEW_OUTPUT "umicom.test.workspace.output"
#define UMI_TEST_UI_VIEW_COVERAGE "umicom.test.workspace.coverage"
#define UMI_TEST_UI_VIEW_RUNS "umicom.test.workspace.runs"

UmiStatus umi_test_ui_explorer_view_create(
    const char *view_id,
    UmiTestWorkspace *workspace,
    UmiUiViewModel **out_view
);
UmiStatus umi_test_ui_results_view_create(
    const char *view_id,
    UmiTestWorkspace *workspace,
    UmiUiViewModel **out_view
);
UmiStatus umi_test_ui_failures_view_create(
    const char *view_id,
    UmiTestWorkspace *workspace,
    UmiUiViewModel **out_view
);
UmiStatus umi_test_ui_output_view_create(
    const char *view_id,
    UmiTestWorkspace *workspace,
    UmiUiViewModel **out_view
);
UmiStatus umi_test_ui_coverage_view_create(
    const char *view_id,
    UmiTestWorkspace *workspace,
    UmiUiViewModel **out_view
);
UmiStatus umi_test_ui_runs_view_create(
    const char *view_id,
    UmiTestWorkspace *workspace,
    UmiUiViewModel **out_view
);

#ifdef __cplusplus
}
#endif

#endif
