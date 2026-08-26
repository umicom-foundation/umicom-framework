#include "umicom/editor/workbench/session_restore.h"
int main(void){ UmiEditorWbSessionRestore s; if(umi_editor_wb_session_restore_init(&s,"id","text")!=UMI_STATUS_OK)return 1; if(umi_editor_wb_session_restore_set_values(&s,3U,4U,true)!=UMI_STATUS_OK)return 2; if(!umi_editor_wb_session_restore_valid(&s)||s.primary!=3U)return 3; return 0; }
