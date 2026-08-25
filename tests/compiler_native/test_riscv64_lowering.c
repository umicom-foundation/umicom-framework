/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_riscv64_lowering.c
 *
 * PURPOSE:
 *   Regression coverage for map target-neutral machine opcodes to risc-v 64 assembly mnemonics and immediate constraints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/riscv64_lowering.h"
#include <string.h>
int main(void){if(strcmp(umi_nc_riscv64_mnemonic(UMI_NC_MOPC_ADD),"add")!=0)return 1;if(!umi_nc_riscv64_immediate_fits_i12(2047)||umi_nc_riscv64_immediate_fits_i12(4096))return 2;return 0;}
