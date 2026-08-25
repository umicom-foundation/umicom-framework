/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_assembly_emitter.c
 *
 * PURPOSE:
 *   Regression coverage for emit deterministic textual assembly from selected machine functions for bootstrap inspection and external assemblers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/assembly_emitter.h"
#include <string.h>
int main(void){UmiNativeMachineFunction f;if(umi_nc_machine_function_init(&f,"main",UMI_NC_ARCH_RISCV64)!=UMI_STATUS_OK||umi_nc_machine_function_add_block(&f,1U,"entry")!=UMI_STATUS_OK)return 1;UmiNativeMachineInstruction i;if(umi_nc_machine_instruction_init(&i,1U,UMI_NC_MOPC_RET)!=UMI_STATUS_OK||umi_nc_machine_block_append(&f.blocks[0],&i)!=UMI_STATUS_OK)return 2;UmiNativeAssemblyBuffer b;if(umi_nc_assembly_emit_function(&f,&b)!=UMI_STATUS_OK)return 3;if(strstr(b.text,"main:")==NULL||strstr(b.text,"ret")==NULL)return 4;return 0;}
