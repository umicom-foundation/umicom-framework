/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_ir_value.c
 *
 * PURPOSE:
 *   Regression coverage for represent ssa-style ir values, constants, parameters and temporaries using stable value ids.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/ir_value.h"
int main(void){UmiNativeIrValue v;UmiNativeIrType t=umi_nc_ir_type_make(UMI_NC_IR_I64,64U,1U);if(umi_nc_ir_value_constant(&v,1U,t,umi_nc_constant_signed(9))!=UMI_STATUS_OK)return 1;if(v.kind!=UMI_NC_IR_VALUE_CONSTANT||v.constant.signed_value!=9)return 2;return 0;}
