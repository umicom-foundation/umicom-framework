/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_incremental_compile.c
 *
 * PURPOSE:
 *   Regression coverage for decide translation-unit rebuilds from source, options, dependency and artifact fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/incremental_compile.h"
#include "../value_archive/construction_cases.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted incremental compile value. */
static int UmiNativeIncrementalUnitConstructionCases(void)
{
    UmiNativeIncrementalUnit *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_nc_incremental_unit_init(value, "a.c", 1U, 2U, 3U) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->path) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_incremental_unit_init(value, oversized, 1U, 2U, 3U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_incremental_unit_init(value, value->path, 1U, 2U, 3U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->path, "a.c") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void){
    if (UmiNativeIncrementalUnitConstructionCases() != 0) return 1;
UmiNativeIncrementalUnit a,b;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_incremental_unit_init(&a,"a.c",1U,2U,3U)!=UMI_STATUS_OK||umi_nc_incremental_unit_init(&b,"a.c",1U,2U,3U)!=UMI_STATUS_OK)return 1;a.artifact_present=true;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_incremental_needs_rebuild(&a,&b))return 2;b.source_hash=9U;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_nc_incremental_needs_rebuild(&a,&b))return 3;return 0;}
