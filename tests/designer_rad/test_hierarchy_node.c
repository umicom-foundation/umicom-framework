/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_hierarchy_node.c
 *
 * PURPOSE:
 *   Validate represent one node in the designer object hierarchy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/hierarchy_node.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadHierarchyNode item;CHECK(umi_rad_hierarchy_node_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_hierarchy_node_is_valid(&item));return 0;}
