/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_duplicate_plan.c
 *
 * PURPOSE:
 *   Validate describe deterministic component duplication before it is committed.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/duplicate_plan.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadDuplicatePlan item;CHECK(umi_rad_duplicate_plan_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_duplicate_plan_is_valid(&item));return 0;}
