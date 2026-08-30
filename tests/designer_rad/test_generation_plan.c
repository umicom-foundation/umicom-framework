/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_generation_plan.c
 *
 * PURPOSE:
 *   Validate describe generated declarative/source artifacts before filesystem writes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/generation_plan.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadGenerationPlan item;CHECK(umi_rad_generation_plan_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_generation_plan_is_valid(&item));return 0;}
