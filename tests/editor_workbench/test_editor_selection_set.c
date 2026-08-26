#include "umicom/editor/workbench/editor_selection_set.h"
int main(void){ UmiEditorWbEditorSelectionSet s; UmiEditorWbRange r={{4U,1U},{2U,1U}}; umi_editor_wb_editor_selection_set_init(&s); if(umi_editor_wb_editor_selection_set_add(&s,r,true)!=UMI_STATUS_OK)return 1; if(s.ranges[0].start.line!=2U||s.primary_index!=0U)return 2; return 0; }
