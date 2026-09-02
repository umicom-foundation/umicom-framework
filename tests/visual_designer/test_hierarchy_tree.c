/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_hierarchy_tree.c
 *
 * PURPOSE:
 *   Validate maintain a bounded component hierarchy with deterministic lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/hierarchy_tree.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadHierarchyTree t;UmiRadHierarchyNode n;CHECK(umi_rad_hierarchy_tree_init(&t)==UMI_STATUS_OK);CHECK(umi_rad_hierarchy_node_init(&n)==UMI_STATUS_OK);CHECK(umi_rad_hierarchy_tree_add(&t,&n)==UMI_STATUS_OK);CHECK(umi_rad_hierarchy_tree_find(&t,n.node_id)!=0);return 0;}
