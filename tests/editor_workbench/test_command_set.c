#include "umicom/editor/workbench/command_set.h"
int main(void){ UmiEditorWbCommandSet s; umi_editor_wb_command_set_init(&s); if(umi_editor_wb_command_set_add(&s,"one")!=UMI_STATUS_OK)return 1; if(!umi_editor_wb_command_set_contains(&s,"one"))return 2; if(umi_editor_wb_command_set_remove(&s,"one")!=UMI_STATUS_OK||s.count!=0U)return 3; return 0; }
