#include "umicom/editor/workbench/editor_location.h"
int main(void){ UmiEditorWbEditorLocation l; if(umi_editor_wb_editor_location_init(&l,"a.c",3U,4U)!=UMI_STATUS_OK)return 1; if(!umi_editor_wb_editor_location_valid(&l))return 2; if(umi_editor_wb_editor_location_init(&l,"a.c",0U,1U)!=UMI_STATUS_INVALID_ARGUMENT)return 3; return 0; }
