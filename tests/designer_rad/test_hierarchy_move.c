/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_hierarchy_move.c
 *
 * PURPOSE:
 *   Validate describe a reviewable hierarchy reparent/reorder operation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/hierarchy_move.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadHierarchyMove item;CHECK(umi_rad_hierarchy_move_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_hierarchy_move_is_valid(&item));return 0;}
