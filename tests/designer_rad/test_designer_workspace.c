/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_designer_workspace.c
 *
 * PURPOSE:
 *   Validate represent the complete visual designer workspace selection and dirty state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/designer_workspace.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadDesignerWorkspace item;CHECK(umi_rad_designer_workspace_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_designer_workspace_is_valid(&item));return 0;}
