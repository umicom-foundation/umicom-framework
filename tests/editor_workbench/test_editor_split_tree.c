/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_split_tree.c
 *
 * PURPOSE:
 *   Implement the test editor split tree behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_split_tree.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbEditorSplitTree t; umi_editor_wb_editor_split_tree_init(&t); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_split_tree_add(&t,"root","")!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_split_tree_add(&t,"child","root")!=UMI_STATUS_OK)return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_wb_editor_split_tree_acyclic(&t))return 3; return 0; }
