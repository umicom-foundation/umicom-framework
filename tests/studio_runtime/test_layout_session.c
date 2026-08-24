/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_layout_session.c
 *
 * PURPOSE:
 *   Behavioral regression coverage for Studio runtime layout session.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/studio_runtime/layout_session.h"
int main(void)
{
    UmiSessionStore *store=NULL;
    UmiApplicationShellState source_state, restored_state;
    UmiApplicationShellLayout source_layout, restored_layout;
    char preset[192];
    int restored=0;
    assert(umi_session_store_create(&store)==UMI_STATUS_OK);
    umi_application_shell_state_init(&source_state);
    umi_application_shell_state_init(&restored_state);
    umi_application_shell_layout_init(&source_layout,"layout","Layout");
    umi_application_shell_layout_init(&restored_layout,"restore","Restore");
    assert(umi_application_shell_state_set_perspective(
        &source_state,"umicom.perspective.integrated-development")==UMI_STATUS_OK);
    assert(umi_application_shell_layout_place(
        &source_layout,"editor","studio.editor",
        UMI_APPLICATION_SHELL_REGION_EDITOR,1U,1)==UMI_STATUS_OK);
    assert(umi_studio_layout_session_save(
        store,"studio","umicom.studio.layout.default",
        &source_state,&source_layout)==UMI_STATUS_OK);
    assert(umi_studio_layout_session_restore(
        store,"studio",preset,sizeof(preset),
        &restored_state,&restored_layout,&restored)==UMI_STATUS_OK);
    assert(restored);
    assert(strcmp(preset,"umicom.studio.layout.default")==0);
    assert(restored_layout.placement_count==1U);
    umi_session_store_destroy(store);
    return 0;
}

