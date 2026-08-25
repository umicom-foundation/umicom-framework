/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_object_section.c
 *
 * PURPOSE:
 *   Regression coverage for describe object-file sections, flags, alignments and planned byte sizes before backend encoding.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/object_section.h"
int main(void){UmiNativeObjectSection s;if(umi_nc_object_section_init(&s,".text",UMI_NC_SECTION_TEXT,16U)!=UMI_STATUS_OK)return 1;if(umi_nc_object_section_reserve(&s,17U)!=UMI_STATUS_OK||umi_nc_object_section_aligned_size(&s)!=32U||!s.executable)return 2;return 0;}
