/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_types.c
 *
 * PURPOSE:
 *   Regression coverage for define stable constants, enums and utility helpers for the native umicom compiler pipeline.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/types.h"
#include <string.h>
int main(void){ char b[8]; if(umi_nc_copy_text(b,sizeof(b),"umicc")!=UMI_STATUS_OK) return 1; if(strcmp(b,"umicc")!=0) return 2; if(umi_nc_hash_text("abc")==0U) return 3; if(strcmp(umi_nc_stage_name(UMI_NC_STAGE_IR),"ir")!=0) return 4; return 0; }
