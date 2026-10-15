/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_machine_function.c
 *
 * PURPOSE:
 *   Regression coverage for aggregate machine blocks, frame size and target architecture for one lowered function.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/machine_function.h"
#include "../value_archive/construction_cases.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted machine function value. */
static int UmiNativeMachineFunctionConstructionCases(void)
{
    UmiNativeMachineFunction *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_nc_machine_function_init(value, "main", UMI_NC_ARCH_RISCV64) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->name) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_machine_function_init(value, oversized, UMI_NC_ARCH_RISCV64) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_machine_function_init(value, value->name, UMI_NC_ARCH_RISCV64) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->name, "main") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void){
    if (UmiNativeMachineFunctionConstructionCases() != 0) return 1;
UmiNativeMachineFunction f;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_machine_function_init(&f,"main",UMI_NC_ARCH_RISCV64)!=UMI_STATUS_OK)return 1;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_machine_function_add_block(&f,1U,"entry")!=UMI_STATUS_OK)return 2;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_machine_function_block(&f,1U)==NULL||umi_nc_machine_function_next_instruction(&f)!=1U)return 3;return 0;}
