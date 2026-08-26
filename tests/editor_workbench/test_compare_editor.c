#include "umicom/editor/workbench/compare_editor.h"
int main(void){ UmiEditorWbCompareEditor s; if(umi_editor_wb_compare_editor_init(&s,"left","right")!=UMI_STATUS_OK)return 1; if(!s.sync_scroll)return 2; return 0; }
