/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/tree.h
 *
 * PURPOSE:
 *   Represent parent/child UI composition without storing toolkit widget pointers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */

#ifndef UMICOM_UI_COMPONENTS_TREE_H
#define UMICOM_UI_COMPONENTS_TREE_H
#include "umicom/ui/components/component.h"
#define UMI_UI_COMPONENT_TREE_CAPACITY 512U
typedef struct UmiUiComponentNode { UmiUiComponentSpec spec; int32_t parent_index; } UmiUiComponentNode;
typedef struct UmiUiComponentTree { UmiUiComponentNode nodes[UMI_UI_COMPONENT_TREE_CAPACITY]; size_t count; } UmiUiComponentTree;
void umi_ui_component_tree_init(UmiUiComponentTree *tree);
UmiStatus umi_ui_component_tree_add(UmiUiComponentTree *tree,const UmiUiComponentSpec *spec,int32_t parent_index,size_t *out_index);
size_t umi_ui_component_tree_child_count(const UmiUiComponentTree *tree,size_t parent_index);
#endif
