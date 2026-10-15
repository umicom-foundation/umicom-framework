/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_ir_block.c
 *
 * PURPOSE:
 *   Regression coverage for store ordered ir instructions and control-flow edges for a basic block.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/ir_block.h"
#include "../value_archive/construction_cases.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted ir block value. */
static int UmiNativeIrBlockConstructionCases(void)
{
    UmiNativeIrBlock *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_nc_ir_block_init(value, 1U, "entry") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->name) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_ir_block_init(value, 1U, oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_ir_block_init(value, 1U, value->name) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->name, "entry") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void){
    if (UmiNativeIrBlockConstructionCases() != 0) return 1;
UmiNativeIrBlock b;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_ir_block_init(&b,1U,"entry")!=UMI_STATUS_OK)return 1;UmiNativeIrInstruction i;UmiNativeIrType v=umi_nc_ir_type_make(UMI_NC_IR_VOID,0U,1U);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_ir_instruction_init(&i,1U,UMI_NC_IR_RET,0U,v)!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_ir_block_append(&b,&i)!=UMI_STATUS_OK||!umi_nc_ir_block_terminated(&b))return 3;return 0;}
