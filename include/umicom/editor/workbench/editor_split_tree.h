/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_split_tree.h
 *
 * PURPOSE:
 *   Maintain an acyclic parent-linked split tree for editor groups.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_SPLIT_TREE_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_SPLIT_TREE_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbSplitNode { char node_id[UMI_EDITOR_WB_ID_CAPACITY]; char parent_id[UMI_EDITOR_WB_ID_CAPACITY]; } UmiEditorWbSplitNode;
typedef struct UmiEditorWbEditorSplitTree { UmiEditorWbSplitNode nodes[UMI_EDITOR_WB_MAX_GROUPS]; size_t count; } UmiEditorWbEditorSplitTree;
void umi_editor_wb_editor_split_tree_init(UmiEditorWbEditorSplitTree *tree);
UmiStatus umi_editor_wb_editor_split_tree_add(UmiEditorWbEditorSplitTree *tree,const char *node_id,const char *parent_id);
int umi_editor_wb_editor_split_tree_acyclic(const UmiEditorWbEditorSplitTree *tree);

#ifdef __cplusplus
}
#endif
#endif
