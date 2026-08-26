#include "umicom/editor/workbench/editor_split_tree.h"
int main(void){ UmiEditorWbEditorSplitTree t; umi_editor_wb_editor_split_tree_init(&t); if(umi_editor_wb_editor_split_tree_add(&t,"root","")!=UMI_STATUS_OK)return 1; if(umi_editor_wb_editor_split_tree_add(&t,"child","root")!=UMI_STATUS_OK)return 2; if(!umi_editor_wb_editor_split_tree_acyclic(&t))return 3; return 0; }
