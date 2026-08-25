/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_macro_table.c
 *
 * PURPOSE:
 *   Regression coverage for maintain a bounded macro catalogue with replaceable definitions and deterministic revision tracking.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/macro_table.h"
int main(void){ UmiNativeMacroTable t; umi_nc_macro_table_init(&t); UmiNativeMacro m; if(umi_nc_macro_init(&m,"N","4",false)!=UMI_STATUS_OK) return 1; if(umi_nc_macro_table_define(&t,&m)!=UMI_STATUS_OK) return 2; if(umi_nc_macro_table_find(&t,"N")==NULL) return 3; if(umi_nc_macro_table_undef(&t,"N")!=UMI_STATUS_OK||t.count!=0U) return 4; return 0; }
