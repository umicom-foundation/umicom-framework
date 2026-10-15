/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_optimization_pass.c
 *
 * PURPOSE:
 *   Regression coverage for describe deterministic optimization pass metadata and minimum optimization levels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/optimization_pass.h"
#include "../value_archive/construction_cases.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted optimization pass value. */
static int UmiNativeOptimizationPassConstructionCases(void)
{
    UmiNativeOptimizationPass *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_nc_optimization_pass_init(value, "fold", UMI_NC_PASS_CONSTANT_FOLD, UMI_NC_OPT_O1, true) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_optimization_pass_init(value, oversized, UMI_NC_PASS_CONSTANT_FOLD, UMI_NC_OPT_O1, true) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_optimization_pass_init(value, value->id, UMI_NC_PASS_CONSTANT_FOLD, UMI_NC_OPT_O1, true) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->id, "fold") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void){
    if (UmiNativeOptimizationPassConstructionCases() != 0) return 1;
UmiNativeOptimizationPass p;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_optimization_pass_init(&p,"fold",UMI_NC_PASS_CONSTANT_FOLD,UMI_NC_OPT_O1,true)!=UMI_STATUS_OK)return 1;UmiNativeOptimizationProfile o=umi_nc_optimization_profile_default(UMI_NC_OPT_O2);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_nc_optimization_pass_enabled(&p,&o))return 2;return 0;}
