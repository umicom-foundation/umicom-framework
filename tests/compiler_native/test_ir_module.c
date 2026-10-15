/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_ir_module.c
 *
 * PURPOSE:
 *   Regression coverage for aggregate native ir functions under a target triple and deterministic module revision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/ir_module.h"
#include "../value_archive/construction_cases.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted ir module value. */
static int UmiNativeIrModuleConstructionCases(void)
{
    UmiNativeIrModule *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_nc_ir_module_init(value, "m", "riscv64-unknown-umicom") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->module_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_ir_module_init(value, oversized, "riscv64-unknown-umicom") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_ir_module_init(value, value->module_id, "riscv64-unknown-umicom") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->module_id, "m") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->target_triple) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_ir_module_init(value, "m", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_ir_module_init(value, "m", value->target_triple) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->target_triple, "riscv64-unknown-umicom") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void){
    if (UmiNativeIrModuleConstructionCases() != 0) return 1;
UmiNativeIrModule m;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_ir_module_init(&m,"m","riscv64-unknown-umicom")!=UMI_STATUS_OK)return 1;UmiNativeIrFunction f;UmiNativeIrType t=umi_nc_ir_type_make(UMI_NC_IR_I32,32U,1U);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_ir_function_init(&f,"main",t)!=UMI_STATUS_OK)return 2;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_ir_module_add_function(&m,&f)!=UMI_STATUS_OK||umi_nc_ir_module_find(&m,"main")==NULL)return 3;return 0;}
