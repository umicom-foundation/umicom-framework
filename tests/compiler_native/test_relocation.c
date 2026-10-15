/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_relocation.c
 *
 * PURPOSE:
 *   Regression coverage for represent object relocation requests independently from elf, coff and mach-o writers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/relocation.h"
#include "../value_archive/construction_cases.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted relocation value. */
static int UmiNativeRelocationConstructionCases(void)
{
    UmiNativeRelocation *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_nc_relocation_init(value, UMI_NC_RELOC_CALL, ".text", 4U, "puts", 0) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->section) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_relocation_init(value, UMI_NC_RELOC_CALL, oversized, 4U, "puts", 0) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_relocation_init(value, UMI_NC_RELOC_CALL, value->section, 4U, "puts", 0) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->section, ".text") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->symbol) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_relocation_init(value, UMI_NC_RELOC_CALL, ".text", 4U, oversized, 0) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_relocation_init(value, UMI_NC_RELOC_CALL, ".text", 4U, value->symbol, 0) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->symbol, "puts") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void){
    if (UmiNativeRelocationConstructionCases() != 0) return 1;
UmiNativeRelocation r;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_relocation_init(&r,UMI_NC_RELOC_CALL,".text",4U,"puts",0)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_relocation_validate(&r,UMI_NC_OBJECT_ELF)!=UMI_STATUS_OK)return 2;return 0;}
