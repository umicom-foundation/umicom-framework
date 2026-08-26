/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/tree_sort.h
 *
 * PURPOSE:
 *   Compare tree nodes using deterministic label or identity order.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_TREE_SORT_H
#define UMICOM_UI_ENTERPRISE_TREE_SORT_H

#include "umicom/ui/enterprise/tree_node.h"
int umi_ui_ent_tree_sort_compare(const UmiUiEntTreeNode *a,const UmiUiEntTreeNode *b,UmiUiEntSortDirection direction,int by_label);

#endif
