/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_tree_projection.c
 *
 * PURPOSE:
 *   Exercise the tree projection enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/tree_projection.h"
#include <stdio.h>
int main(void){UmiUiEntTreeProjection p;UmiUiEntTreeNode n;umi_ui_ent_tree_projection_init(&p);umi_ui_ent_tree_node_init(&n);umi_ui_ent_copy_text(n.node_id,sizeof n.node_id,"n");if(umi_ui_ent_tree_projection_append(&p,&n)!=UMI_STATUS_OK||p.count!=1U)return 1;puts("ok");return 0;}
