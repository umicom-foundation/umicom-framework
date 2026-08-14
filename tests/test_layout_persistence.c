/* Umicom Framework | Layout persistence tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/ui/layout_persistence.h"
int main(void)
{
    UmiUiLayoutPersistenceRecord record = {0};
    UmiUiLayoutPersistenceRecord restored;
    UmiUiWorkspaceWindow explorer = {"explorer","Explorer","explorer","project-blue",0.0,0.0,0.2,1.0,true,false,false,false,1};
    UmiUiWorkspaceWindow editor = {"editor","Editor","editor","project-blue",0.2,0.0,0.8,1.0,true,false,false,true,2};
    char encoded[UMI_UI_LAYOUT_ENCODED_CAPACITY];
    record.schema_version = 2U;
    record.saved_at_ns = 100U;
    assert(umi_ui_workspace_layout_init(&record.layout,"develop","Develop") == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_set_locked(&record.layout,false) == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_add_window(&record.layout,&explorer) == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_add_window(&record.layout,&editor) == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_set_locked(&record.layout,true) == UMI_STATUS_OK);
    assert(umi_ui_layout_persistence_encode(&record,encoded,sizeof(encoded)) == UMI_STATUS_OK);
    assert(umi_ui_layout_persistence_decode(encoded,&restored) == UMI_STATUS_OK);
    assert(restored.layout.window_count == 2U);
    assert(strcmp(restored.layout.windows[1].window_id,"editor") == 0);
    assert(restored.layout.locked);
    return 0;
}
