/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/tree_sort.h
 *
 * PURPOSE:
 *   Compare tree nodes using deterministic label or identity order.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_TREE_SORT_H
#define UMICOM_UI_ENTERPRISE_TREE_SORT_H

#include "umicom/ui/enterprise/tree_node.h"

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ui ent tree sort compare operation used by this module and its client
 * applications.
 */
int umi_ui_ent_tree_sort_compare(const UmiUiEntTreeNode *a,const UmiUiEntTreeNode *b,UmiUiEntSortDirection direction,int by_label);


#ifdef __cplusplus
}
#endif
#endif
