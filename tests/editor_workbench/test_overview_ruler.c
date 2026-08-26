#include "umicom/editor/workbench/overview_ruler.h"
int main(void){ UmiEditorWbOverviewRuler s; umi_editor_wb_overview_ruler_init(&s); if(umi_editor_wb_overview_ruler_add(&s,9U,UMI_EDITOR_WB_ERROR)!=UMI_STATUS_OK||s.count!=1U)return 1; return 0; }
