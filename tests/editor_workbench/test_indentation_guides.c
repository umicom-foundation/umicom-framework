#include "umicom/editor/workbench/indentation_guides.h"
int main(void){ if(umi_editor_wb_indentation_guides_depth(12U,4U)!=3U)return 1; if(umi_editor_wb_indentation_guides_depth(4U,0U)!=0U)return 2; return 0; }
