#include "umicom/editor/workbench/editor_reopen.h"
int main(void){ UmiEditorWbEditorReopen s; if(umi_editor_wb_editor_reopen_init(&s,"a.c","main")!=UMI_STATUS_OK)return 1; if(!umi_editor_wb_editor_reopen_valid(&s))return 2; return 0; }
