#include "umicom/editor/workbench/bookmark_panel.h"
int main(void){ UmiEditorWbBookmarkPanel s; umi_editor_wb_bookmark_panel_init(&s); if(umi_editor_wb_bookmark_panel_add(&s,"one")!=UMI_STATUS_OK)return 1; if(!umi_editor_wb_bookmark_panel_contains(&s,"one"))return 2; if(umi_editor_wb_bookmark_panel_remove(&s,"one")!=UMI_STATUS_OK||s.count!=0U)return 3; return 0; }
