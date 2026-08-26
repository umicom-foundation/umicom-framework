#include "umicom/editor/workbench/editor_appearance.h"
int main(void){ UmiEditorWbEditorAppearance s; if(umi_editor_wb_editor_appearance_init(&s,"id","text")!=UMI_STATUS_OK)return 1; if(umi_editor_wb_editor_appearance_set_values(&s,3U,4U,true)!=UMI_STATUS_OK)return 2; if(!umi_editor_wb_editor_appearance_valid(&s)||s.primary!=3U)return 3; return 0; }
