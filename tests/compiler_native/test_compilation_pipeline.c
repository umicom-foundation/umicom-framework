/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_compilation_pipeline.c
 *
 * PURPOSE:
 *   Regression coverage for track deterministic compiler stages, status and readiness from source through link completion.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/compilation_pipeline.h"
int main(void){UmiNativeCompilationPipeline p;umi_nc_compilation_pipeline_init(&p,false);const UmiNativeStageRecord *n=umi_nc_compilation_pipeline_next(&p);if(n==NULL||n->stage!=UMI_NC_STAGE_SOURCE)return 1;for(size_t i=0U;i<p.count;i++)if(p.records[i].stage!=UMI_NC_STAGE_COMPLETE)if(umi_nc_compilation_pipeline_mark(&p,p.records[i].stage,UMI_NC_STAGE_PASSED,UMI_STATUS_OK)!=UMI_STATUS_OK)return 2;if(!umi_nc_compilation_pipeline_ready(&p))return 3;return 0;}
