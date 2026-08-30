/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_tree_sort.c
 *
 * PURPOSE:
 *   Exercise the tree sort enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/tree_sort.h"
#include <stdio.h>
int main(void){UmiUiEntTreeNode a,b;umi_ui_ent_tree_node_init(&a);umi_ui_ent_tree_node_init(&b);umi_ui_ent_copy_text(a.node_id,sizeof a.node_id,"a");umi_ui_ent_copy_text(b.node_id,sizeof b.node_id,"b");umi_ui_ent_copy_text(a.label,sizeof a.label,"Alpha");umi_ui_ent_copy_text(b.label,sizeof b.label,"Beta");if(umi_ui_ent_tree_sort_compare(&a,&b,UMI_UI_ENT_SORT_ASCENDING,1)>=0)return 1;puts("ok");return 0;}
