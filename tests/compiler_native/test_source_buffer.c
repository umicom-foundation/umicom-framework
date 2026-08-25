/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_source_buffer.c
 *
 * PURPOSE:
 *   Regression coverage for own a bounded source snapshot with deterministic revision and line-count tracking.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/source_buffer.h"
int main(void){ UmiNativeSourceBuffer b; umi_nc_source_buffer_init(&b); if(umi_nc_source_buffer_set(&b,"main.c","int main(void){\nreturn 0;\n}")!=UMI_STATUS_OK) return 1; if(umi_nc_source_buffer_line_count(&b)!=3U) return 2; uint64_t h=umi_nc_source_buffer_fingerprint(&b); if(h==0U) return 3; if(umi_nc_source_buffer_append(&b,"\n")!=UMI_STATUS_OK) return 4; if(b.revision!=2U) return 5; return 0; }
