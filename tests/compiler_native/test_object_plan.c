/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_object_plan.c
 *
 * PURPOSE:
 *   Regression coverage for aggregate object sections, relocations and object-format intent before backend serialization.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/object_plan.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeObjectPlan p;umi_nc_object_plan_init(&p,UMI_NC_OBJECT_ELF);UmiNativeObjectSection s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_object_section_init(&s,".text",UMI_NC_SECTION_TEXT,16U)!=UMI_STATUS_OK||umi_nc_object_section_reserve(&s,9U)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_object_plan_add_section(&p,&s)!=UMI_STATUS_OK||umi_nc_object_plan_total_size(&p)!=16U)return 2;return 0;}
