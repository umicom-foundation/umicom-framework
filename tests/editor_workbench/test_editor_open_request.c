#include "umicom/editor/workbench/editor_open_request.h"
int main(void){ UmiEditorWbEditorOpenRequest s; if(umi_editor_wb_editor_open_request_init(&s,"a.c","main")!=UMI_STATUS_OK)return 1; if(!umi_editor_wb_editor_open_request_valid(&s))return 2; return 0; }
