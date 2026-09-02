/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_source_buffer.c
 *
 * PURPOSE:
 *   Regression coverage for own a bounded source snapshot with deterministic revision and line-count tracking.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/source_buffer.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiNativeSourceBuffer b; umi_nc_source_buffer_init(&b); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_source_buffer_set(&b,"main.c","int main(void){\nreturn 0;\n}")!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_source_buffer_line_count(&b)!=3U) return 2; uint64_t h=umi_nc_source_buffer_fingerprint(&b); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(h==0U) return 3; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_source_buffer_append(&b,"\n")!=UMI_STATUS_OK) return 4; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(b.revision!=2U) return 5; return 0; }
