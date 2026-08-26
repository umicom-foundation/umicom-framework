#include "umicom/editor/workbench/readonly_document_state.h"
int main(void){ UmiEditorWbReadonlyDocumentState s; if(umi_editor_wb_readonly_document_state_init(&s,"id","text")!=UMI_STATUS_OK)return 1; if(umi_editor_wb_readonly_document_state_set_values(&s,3U,4U,true)!=UMI_STATUS_OK)return 2; if(!umi_editor_wb_readonly_document_state_valid(&s)||s.primary!=3U)return 3; return 0; }
