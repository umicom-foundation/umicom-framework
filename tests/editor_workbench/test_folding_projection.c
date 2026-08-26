#include "umicom/editor/workbench/folding_projection.h"
int main(void){ UmiEditorWbFoldingProjection p; umi_editor_wb_folding_projection_init(&p); if(umi_editor_wb_folding_projection_add(&p,3U,8U)!=UMI_STATUS_OK)return 1; if(!umi_editor_wb_folding_projection_line_hidden(&p,4U)||umi_editor_wb_folding_projection_line_hidden(&p,3U))return 2; return 0; }
