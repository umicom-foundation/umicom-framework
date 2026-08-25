/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_object_plan.c
 *
 * PURPOSE:
 *   Regression coverage for aggregate object sections, relocations and object-format intent before backend serialization.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/object_plan.h"
int main(void){UmiNativeObjectPlan p;umi_nc_object_plan_init(&p,UMI_NC_OBJECT_ELF);UmiNativeObjectSection s;if(umi_nc_object_section_init(&s,".text",UMI_NC_SECTION_TEXT,16U)!=UMI_STATUS_OK||umi_nc_object_section_reserve(&s,9U)!=UMI_STATUS_OK)return 1;if(umi_nc_object_plan_add_section(&p,&s)!=UMI_STATUS_OK||umi_nc_object_plan_total_size(&p)!=16U)return 2;return 0;}
