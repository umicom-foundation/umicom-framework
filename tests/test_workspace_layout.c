/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_layout.c
 *
 * PURPOSE:
 *   Implement the test workspace layout behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Professional layout tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/ui/workspace_layout.h"
int main(void)
{
    UmiUiWorkspaceLayout layout;
    UmiUiWorkspaceWindow editor = {"editor-1","Editor","editor","",0.2,0.0,0.8,0.75,true,false,false,true,1};
    char reason[192U];
    assert(umi_ui_workspace_layout_init(&layout,"develop","Develop") == UMI_STATUS_OK);
    assert(layout.locked);
    assert(umi_ui_workspace_layout_add_window(&layout,&editor) == UMI_STATUS_PERMISSION_DENIED);
    assert(umi_ui_workspace_layout_set_locked(&layout,false) == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_add_window(&layout,&editor) == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_place_window(&layout,"editor-1",0.0,0.0,1.0,1.0) == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_set_locked(&layout,true) == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_set_maximised(&layout,"editor-1",true) == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_validate(&layout,reason,sizeof(reason)) == UMI_STATUS_OK);
    assert(strcmp(reason,"Layout is valid") == 0);
    return 0;
}
