/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_machine_block.c
 *
 * PURPOSE:
 *   Regression coverage for store selected machine instructions under stable block labels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/machine_block.h"
#include "../value_archive/construction_cases.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted machine block value. */
static int UmiNativeMachineBlockConstructionCases(void)
{
    UmiNativeMachineBlock *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_nc_machine_block_init(value, 1U, "entry") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->label) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_machine_block_init(value, 1U, oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_machine_block_init(value, 1U, value->label) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->label, "entry") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void){
    if (UmiNativeMachineBlockConstructionCases() != 0) return 1;
UmiNativeMachineBlock b;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_machine_block_init(&b,1U,"entry")!=UMI_STATUS_OK)return 1;UmiNativeMachineInstruction i;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_machine_instruction_init(&i,1U,UMI_NC_MOPC_RET)!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_machine_block_append(&b,&i)!=UMI_STATUS_OK||b.instruction_count!=1U)return 3;return 0;}
