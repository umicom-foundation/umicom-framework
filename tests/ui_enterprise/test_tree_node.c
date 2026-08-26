/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_tree_node.c
 *
 * PURPOSE:
 *   Exercise the tree node enterprise UI capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/tree_node.h"
#include <stdio.h>
int main(void){UmiUiEntTreeNode v;if(umi_ui_ent_tree_node_init(&v)!=UMI_STATUS_OK)return 1;if(umi_ui_ent_copy_text(v.node_id,sizeof v.node_id,"id")!=UMI_STATUS_OK)return 2;if(!umi_ui_ent_tree_node_validate(&v))return 9;puts("ok");return 0;}
