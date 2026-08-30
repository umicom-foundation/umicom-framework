/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_source_location.c
 *
 * PURPOSE:
 *   Regression coverage for provide source-span construction, advancement and containment helpers for diagnostics and syntax nodes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/source_location.h"
int main(void){ UmiNativeSourceSpan a=umi_nc_source_span_make(1U,10U,3U,2U,4U); UmiNativeSourceSpan b=umi_nc_source_span_make(1U,13U,5U,2U,7U); UmiNativeSourceSpan m=umi_nc_source_span_merge(a,b); if(m.offset!=10U||m.length!=8U) return 1; if(!umi_nc_source_span_contains(&m,17U)) return 2; m=umi_nc_source_span_advance(m,'\n'); if(m.line!=3U||m.column!=1U) return 3; return 0; }
