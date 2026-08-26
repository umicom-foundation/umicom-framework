/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_tree_filter.c
 *
 * PURPOSE:
 *   Exercise the tree filter enterprise UI capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/tree_filter.h"
#include <stdio.h>
int main(void){UmiUiEntTreeFilter f;UmiUiEntTreeNode n;umi_ui_ent_tree_node_init(&n);umi_ui_ent_copy_text(n.node_id,sizeof n.node_id,"n");umi_ui_ent_copy_text(n.label,sizeof n.label,"Risk Limits");umi_ui_ent_tree_filter_set(&f,"risk",0,1);if(!umi_ui_ent_tree_filter_matches(&f,&n))return 1;puts("ok");return 0;}
