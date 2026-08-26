#include "umicom/editor/workbench/minimap_model.h"
int main(void){ UmiEditorWbMinimapModel m; if(umi_editor_wb_minimap_model_init(&m,100U,200)!=UMI_STATUS_OK)return 1; if(umi_editor_wb_minimap_model_y_for_line(&m,51U)!=100)return 2; return 0; }
