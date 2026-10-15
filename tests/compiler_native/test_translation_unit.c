/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_translation_unit.c
 *
 * PURPOSE:
 *   Regression coverage for track top-level declarations and source identity for a single c translation unit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/translation_unit.h"
#include "../value_archive/construction_cases.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted translation unit value. */
static int UmiNativeTranslationUnitConstructionCases(void)
{
    UmiNativeTranslationUnit *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_nc_translation_unit_init(value, "main.c", 1U, 9U) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->path) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_translation_unit_init(value, oversized, 1U, 9U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_translation_unit_init(value, value->path, 1U, 9U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->path, "main.c") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void){
    if (UmiNativeTranslationUnitConstructionCases() != 0) return 1;
 UmiNativeTranslationUnit u; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_translation_unit_init(&u,"main.c",1U,9U)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_translation_unit_add_declaration(&u,2U)!=UMI_STATUS_OK)return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_translation_unit_fingerprint(&u)==0U)return 3;return 0; }
