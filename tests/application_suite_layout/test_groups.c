/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_suite_layout/test_groups.c
 *
 * PURPOSE:
 *   Exercise Framework-owned Application Suite layout materialisation and coverage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/application/suite_layout/suite_layout.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiUiWorkspaceLayout l;
    assert(umi_application_suite_layout_project_default("org.umicom.trader",&l)==UMI_STATUS_OK);
    assert(strcmp(umi_ui_workspace_layout_find_window(&l,"watchlist")->group_id,"left")==0);
    assert(strcmp(umi_ui_workspace_layout_find_window(&l,"chart")->group_id,"centre")==0);
    assert(strcmp(umi_ui_workspace_layout_find_window(&l,"depth")->group_id,"right")==0);
    assert(strcmp(umi_ui_workspace_layout_find_window(&l,"blotter")->group_id,"bottom")==0);
    return 0;
}
