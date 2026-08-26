#include "umicom/editor/workbench/transient_editor.h"
int main(void){ UmiEditorWbTransientEditor s; if(umi_editor_wb_transient_editor_init(&s,"id","text")!=UMI_STATUS_OK)return 1; if(umi_editor_wb_transient_editor_set_values(&s,3U,4U,true)!=UMI_STATUS_OK)return 2; if(!umi_editor_wb_transient_editor_valid(&s)||s.primary!=3U)return 3; return 0; }
