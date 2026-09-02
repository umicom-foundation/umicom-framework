/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_object_section.c
 *
 * PURPOSE:
 *   Regression coverage for describe object-file sections, flags, alignments and planned byte sizes before backend encoding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/object_section.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeObjectSection s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_object_section_init(&s,".text",UMI_NC_SECTION_TEXT,16U)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_object_section_reserve(&s,17U)!=UMI_STATUS_OK||umi_nc_object_section_aligned_size(&s)!=32U||!s.executable)return 2;return 0;}
