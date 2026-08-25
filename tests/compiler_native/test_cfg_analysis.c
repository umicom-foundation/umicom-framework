/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_cfg_analysis.c
 *
 * PURPOSE:
 *   Regression coverage for compute reachable basic blocks and edge counts from the native ir control-flow graph.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/cfg_analysis.h"
int main(void){UmiNativeIrFunction f;UmiNativeIrType t=umi_nc_ir_type_make(UMI_NC_IR_VOID,0U,1U);if(umi_nc_ir_function_init(&f,"f",t)!=UMI_STATUS_OK)return 1;uint32_t a=0U,b=0U;if(umi_nc_ir_function_add_block(&f,"a",&a)!=UMI_STATUS_OK||umi_nc_ir_function_add_block(&f,"b",&b)!=UMI_STATUS_OK)return 2;if(umi_nc_ir_block_add_successor(&f.blocks[0],b)!=UMI_STATUS_OK)return 3;UmiNativeCfgAnalysis x;if(umi_nc_cfg_analyze(&f,&x)!=UMI_STATUS_OK||x.reachable_count!=2U||x.edge_count!=1U)return 4;return 0;}
