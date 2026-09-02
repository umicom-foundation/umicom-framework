/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_relocation.c
 *
 * PURPOSE:
 *   Regression coverage for represent object relocation requests independently from elf, coff and mach-o writers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/relocation.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeRelocation r;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_relocation_init(&r,UMI_NC_RELOC_CALL,".text",4U,"puts",0)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_relocation_validate(&r,UMI_NC_OBJECT_ELF)!=UMI_STATUS_OK)return 2;return 0;}
