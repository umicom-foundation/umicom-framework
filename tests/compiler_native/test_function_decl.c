/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_function_decl.c
 *
 * PURPOSE:
 *   Regression coverage for model native function signatures, parameter type ids, variadic state and abi-relevant flags.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/function_decl.h"
#include "../value_archive/construction_cases.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted function decl value. */
static int UmiNativeFunctionDeclConstructionCases(void)
{
    UmiNativeFunctionDecl *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_nc_function_decl_init(value, "sum", 1U) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->name) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_function_decl_init(value, oversized, 1U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_function_decl_init(value, value->name, 1U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->name, "sum") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void){
    if (UmiNativeFunctionDeclConstructionCases() != 0) return 1;
UmiNativeFunctionDecl d;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_function_decl_init(&d,"sum",1U)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_function_decl_add_parameter(&d,2U)!=UMI_STATUS_OK||umi_nc_function_decl_add_parameter(&d,2U)!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_function_decl_signature(&d)==0U)return 3;return 0;}
