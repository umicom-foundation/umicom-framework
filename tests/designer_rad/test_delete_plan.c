/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_delete_plan.c
 *
 * PURPOSE:
 *   Validate describe a bounded set of component identifiers scheduled for deletion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/delete_plan.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadDeletePlan p;CHECK(umi_rad_delete_plan_init(&p)==UMI_STATUS_OK);CHECK(umi_rad_delete_plan_add(&p,"button") == UMI_STATUS_OK);CHECK(umi_rad_delete_plan_contains(&p,"button"));return 0;}
