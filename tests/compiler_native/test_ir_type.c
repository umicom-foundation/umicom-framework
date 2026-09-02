/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_ir_type.c
 *
 * PURPOSE:
 *   Regression coverage for describe compact native intermediate-representation scalar and pointer types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/ir_type.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeIrType i=umi_nc_ir_type_make(UMI_NC_IR_I32,32U,1U);/* Use the stable identifier comparison to choose the matching record or policy. */ if(umi_nc_ir_type_size(&i,8U)!=4U||!umi_nc_ir_type_is_integer(&i)||!umi_nc_ir_type_equal(&i,&i))return 1;return 0;}
