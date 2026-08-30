/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_target_profile.c
 *
 * PURPOSE:
 *   Regression coverage for define native code-generation profiles for x86-64 and risc-v without replacing canonical compiler triples.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/target_profile.h"
int main(void){UmiNativeTargetProfile p;if(umi_nc_target_profile_init(&p,"riscv64-unknown-umicom")!=UMI_STATUS_OK)return 1;if(p.architecture!=UMI_NC_ARCH_RISCV64||p.object_format!=UMI_NC_OBJECT_ELF||p.pointer_bits!=64U)return 2;return 0;}
