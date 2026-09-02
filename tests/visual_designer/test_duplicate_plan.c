/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_duplicate_plan.c
 *
 * PURPOSE:
 *   Validate describe deterministic component duplication before it is committed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/duplicate_plan.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadDuplicatePlan item;CHECK(umi_rad_duplicate_plan_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_duplicate_plan_is_valid(&item));return 0;}
