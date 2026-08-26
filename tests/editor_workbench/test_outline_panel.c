#include "umicom/editor/workbench/outline_panel.h"
int main(void){ UmiEditorWbOutlinePanel s; umi_editor_wb_outline_panel_init(&s); if(umi_editor_wb_outline_panel_append(&s,"one",1U)!=UMI_STATUS_OK)return 1; if(umi_editor_wb_outline_panel_append(&s,"two",2U)!=UMI_STATUS_OK)return 2; if(umi_editor_wb_outline_panel_activate(&s,1U)!=UMI_STATUS_OK||s.active_index!=1U)return 3; return 0; }
