#include "umicom/editor/workbench/editor_layout.h"
int main(void){ UmiEditorWbEditorLayout s; if(umi_editor_wb_editor_layout_init(&s,"id","")!=UMI_STATUS_OK)return 1; if(umi_editor_wb_editor_layout_set_count(&s,3U,1U)!=UMI_STATUS_OK)return 2; if(!umi_editor_wb_editor_layout_valid(&s)||s.active_index!=1U)return 3; return 0; }
