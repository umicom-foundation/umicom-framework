/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_compilation_pipeline.c
 *
 * PURPOSE:
 *   Regression coverage for track deterministic compiler stages, status and readiness from source through link completion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/compilation_pipeline.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeCompilationPipeline p;umi_nc_compilation_pipeline_init(&p,false);const UmiNativeStageRecord *n=umi_nc_compilation_pipeline_next(&p);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(n==NULL||n->stage!=UMI_NC_STAGE_SOURCE)return 1;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<p.count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p.records[i].stage!=UMI_NC_STAGE_COMPLETE)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_compilation_pipeline_mark(&p,p.records[i].stage,UMI_NC_STAGE_PASSED,UMI_STATUS_OK)!=UMI_STATUS_OK)return 2;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_nc_compilation_pipeline_ready(&p))return 3;return 0;}
