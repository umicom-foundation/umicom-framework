/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/tree_sort.c
 *
 * PURPOSE:
 *   Implement deterministic tree-node comparison.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/tree_sort.h"
#include <string.h>
int umi_ui_ent_tree_sort_compare(const UmiUiEntTreeNode *a,const UmiUiEntTreeNode *b,UmiUiEntSortDirection d,int label){int c;if(!a||!b)return 0;c=strcmp(label?a->label:a->node_id,label?b->label:b->node_id);return d==UMI_UI_ENT_SORT_DESCENDING?-c:c;}
