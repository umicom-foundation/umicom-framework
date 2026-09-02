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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiNativeSourceSpan a=umi_nc_source_span_make(1U,10U,3U,2U,4U); UmiNativeSourceSpan b=umi_nc_source_span_make(1U,13U,5U,2U,7U); UmiNativeSourceSpan m=umi_nc_source_span_merge(a,b); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(m.offset!=10U||m.length!=8U) return 1; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!umi_nc_source_span_contains(&m,17U)) return 2; m=umi_nc_source_span_advance(m,'\n'); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(m.line!=3U||m.column!=1U) return 3; return 0; }
