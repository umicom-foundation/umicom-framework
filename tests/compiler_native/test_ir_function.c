/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_ir_function.c
 *
 * PURPOSE:
 *   Regression coverage for own ir basic blocks and stable value/block id allocation for one native function.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/ir_function.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeIrFunction f;UmiNativeIrType t=umi_nc_ir_type_make(UMI_NC_IR_I32,32U,1U);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_ir_function_init(&f,"main",t)!=UMI_STATUS_OK)return 1;uint32_t id=0U;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_ir_function_add_block(&f,"entry",&id)!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(id!=1U||f.entry_block_id!=1U||umi_nc_ir_function_next_value(&f)!=1U)return 3;return 0;}
