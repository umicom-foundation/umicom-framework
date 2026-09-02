/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_hierarchy_move.c
 *
 * PURPOSE:
 *   Validate describe a reviewable hierarchy reparent/reorder operation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/hierarchy_move.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadHierarchyMove item;CHECK(umi_rad_hierarchy_move_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_hierarchy_move_is_valid(&item));return 0;}
