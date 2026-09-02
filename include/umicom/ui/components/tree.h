/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/tree.h
 *
 * PURPOSE:
 *   Represent parent/child UI composition without storing toolkit widget pointers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */

#ifndef UMICOM_UI_COMPONENTS_TREE_H
#define UMICOM_UI_COMPONENTS_TREE_H
#include "umicom/ui/components/component.h"
#define UMI_UI_COMPONENT_TREE_CAPACITY 512U
/**
 * Represent the ui component node data shared with callers of this public contract.
 */
typedef struct UmiUiComponentNode { UmiUiComponentSpec spec; int32_t parent_index; } UmiUiComponentNode;
/**
 * Represent the ui component tree data shared with callers of this public contract.
 */
typedef struct UmiUiComponentTree { UmiUiComponentNode nodes[UMI_UI_COMPONENT_TREE_CAPACITY]; size_t count; } UmiUiComponentTree;
/**
 * Initialise ui component tree from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_component_tree_init(UmiUiComponentTree *tree);
/**
 * Add ui component tree only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ui_component_tree_add(UmiUiComponentTree *tree,const UmiUiComponentSpec *spec,int32_t parent_index,size_t *out_index);
/**
 * Return the number of records represented by ui component tree child without changing
 * their state.
 */
size_t umi_ui_component_tree_child_count(const UmiUiComponentTree *tree,size_t parent_index);
#endif
