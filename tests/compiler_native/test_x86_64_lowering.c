/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_x86_64_lowering.c
 *
 * PURPOSE:
 *   Regression coverage for map target-neutral machine opcodes to x86-64 intel-style assembly mnemonics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/x86_64_lowering.h"
#include <string.h>
int main(void){if(strcmp(umi_nc_x86_64_mnemonic(UMI_NC_MOPC_MUL),"imul")!=0)return 1;return 0;}
