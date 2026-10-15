/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_macro.c
 *
 * PURPOSE:
 *   Regression coverage for describe object-like and function-like preprocessor macros with stable fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/macro.h"
#include "../value_archive/construction_cases.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted macro value. */
static int UmiNativeMacroConstructionCases(void)
{
    UmiNativeMacro *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_nc_macro_init(value, "MAX", "((a)>(b)?(a):(b))", true) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->name) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_macro_init(value, oversized, "((a)>(b)?(a):(b))", true) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_macro_init(value, value->name, "((a)>(b)?(a):(b))", true) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->name, "MAX") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->replacement) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_macro_init(value, "MAX", oversized, true) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_macro_init(value, "MAX", value->replacement, true) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->replacement, "((a)>(b)?(a):(b))") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void){
    if (UmiNativeMacroConstructionCases() != 0) return 1;
 UmiNativeMacro m; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_macro_init(&m,"MAX","((a)>(b)?(a):(b))",true)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_macro_add_parameter(&m,"a")!=UMI_STATUS_OK||umi_nc_macro_add_parameter(&m,"b")!=UMI_STATUS_OK) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_macro_validate(&m)!=UMI_STATUS_OK||m.fingerprint==0U) return 3; return 0; }
