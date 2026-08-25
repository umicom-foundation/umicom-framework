/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_relocation.c
 *
 * PURPOSE:
 *   Regression coverage for represent object relocation requests independently from elf, coff and mach-o writers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/relocation.h"
int main(void){UmiNativeRelocation r;if(umi_nc_relocation_init(&r,UMI_NC_RELOC_CALL,".text",4U,"puts",0)!=UMI_STATUS_OK)return 1;if(umi_nc_relocation_validate(&r,UMI_NC_OBJECT_ELF)!=UMI_STATUS_OK)return 2;return 0;}
