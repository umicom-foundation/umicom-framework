/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_preprocess_plan.c
 *
 * PURPOSE:
 *   Regression coverage for scan preprocessor structure, count directives and validate conditional nesting before expansion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/preprocess_plan.h"
int main(void){ UmiNativeSourceBuffer s; umi_nc_source_buffer_init(&s); if(umi_nc_source_buffer_set(&s,"x.c","#define A 1\n#if A\n#include <x.h>\n#endif\n")!=UMI_STATUS_OK) return 1; UmiNativePreprocessPlan p; if(umi_nc_preprocess_plan_scan(&s,&p)!=UMI_STATUS_OK) return 2; if(p.defines!=1U||p.includes!=1U||p.max_conditional_depth!=1U||!umi_nc_preprocess_plan_ready(&p)) return 3; return 0; }
