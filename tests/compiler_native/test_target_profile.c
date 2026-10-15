/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_target_profile.c
 *
 * PURPOSE:
 *   Regression coverage for define native code-generation profiles for x86-64 and risc-v without replacing canonical compiler triples.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/target_profile.h"
#include "../value_archive/construction_cases.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted target profile value. */
static int UmiNativeTargetProfileConstructionCases(void)
{
    UmiNativeTargetProfile *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_nc_target_profile_init(value, "riscv64-unknown-umicom") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->triple) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_target_profile_init(value, oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_target_profile_init(value, value->triple) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->triple, "riscv64-unknown-umicom") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void){
    if (UmiNativeTargetProfileConstructionCases() != 0) return 1;
UmiNativeTargetProfile p;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_target_profile_init(&p,"riscv64-unknown-umicom")!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(p.architecture!=UMI_NC_ARCH_RISCV64||p.object_format!=UMI_NC_OBJECT_ELF||p.pointer_bits!=64U)return 2;return 0;}
