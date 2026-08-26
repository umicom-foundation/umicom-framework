/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_hierarchy_tree.c
 *
 * PURPOSE:
 *   Validate maintain a bounded component hierarchy with deterministic lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/hierarchy_tree.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadHierarchyTree t;UmiRadHierarchyNode n;CHECK(umi_rad_hierarchy_tree_init(&t)==UMI_STATUS_OK);CHECK(umi_rad_hierarchy_node_init(&n)==UMI_STATUS_OK);CHECK(umi_rad_hierarchy_tree_add(&t,&n)==UMI_STATUS_OK);CHECK(umi_rad_hierarchy_tree_find(&t,n.node_id)!=0);return 0;}
