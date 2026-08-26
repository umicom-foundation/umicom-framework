#include "umicom/editor/workbench/types.h"
int main(void){ char b[8]; UmiEditorWbRange r={ {4U,2U},{2U,1U} }; r=umi_editor_wb_range_normalize(r); if(umi_editor_wb_copy_text(b,sizeof b,"editor")!=UMI_STATUS_OK)return 1; if(r.start.line!=2U||r.end.line!=4U)return 2; if(!umi_editor_wb_id_valid("main"))return 3; return 0; }
