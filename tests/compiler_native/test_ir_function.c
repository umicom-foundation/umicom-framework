/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_ir_function.c
 *
 * PURPOSE:
 *   Regression coverage for own ir basic blocks and stable value/block id allocation for one native function.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/ir_function.h"
int main(void){UmiNativeIrFunction f;UmiNativeIrType t=umi_nc_ir_type_make(UMI_NC_IR_I32,32U,1U);if(umi_nc_ir_function_init(&f,"main",t)!=UMI_STATUS_OK)return 1;uint32_t id=0U;if(umi_nc_ir_function_add_block(&f,"entry",&id)!=UMI_STATUS_OK)return 2;if(id!=1U||f.entry_block_id!=1U||umi_nc_ir_function_next_value(&f)!=1U)return 3;return 0;}
