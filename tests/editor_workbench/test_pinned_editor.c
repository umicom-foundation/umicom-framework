#include "umicom/editor/workbench/pinned_editor.h"
int main(void){ UmiEditorWbPinnedEditor s; if(umi_editor_wb_pinned_editor_init(&s,"item",false)!=UMI_STATUS_OK)return 1; if(umi_editor_wb_pinned_editor_set(&s,true)!=UMI_STATUS_OK)return 2; if(!umi_editor_wb_pinned_editor_valid(&s)||!s.enabled)return 3; return 0; }
