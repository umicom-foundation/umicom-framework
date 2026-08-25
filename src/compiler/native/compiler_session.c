/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/compiler_session.c
 *
 * PURPOSE:
 *   Coordinate one native compilation session across target, provider, pipeline and deterministic fingerprints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/compiler_session.h"
#include <string.h>
UmiStatus umi_nc_compiler_session_begin(UmiNativeCompilerSession *s,const char *id,const char *triple,UmiNativeOptimizationLevel opt,const UmiNativeProviderAvailability *providers,bool selfhost,uint64_t source,uint64_t options,uint64_t deps){if(s==NULL||id==NULL||triple==NULL||providers==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof(*s));if(umi_nc_copy_text(s->session_id,sizeof(s->session_id),id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;UmiStatus st=umi_nc_target_profile_init(&s->target,triple);if(st!=UMI_STATUS_OK)return st;s->optimization=umi_nc_optimization_profile_default(opt);st=umi_nc_provider_bridge_plan(providers,true,selfhost,&s->provider_plan);if(st!=UMI_STATUS_OK)return st;umi_nc_compilation_pipeline_init(&s->pipeline,true);UmiNativeCompileFingerprintInput fp={source,options,umi_nc_hash_text(triple),deps,umi_nc_hash_text("umicc-native-pipeline")};s->compile_fingerprint=umi_nc_compile_fingerprint(&fp);s->active=true;return UMI_STATUS_OK;}
UmiStatus umi_nc_compiler_session_stage(UmiNativeCompilerSession *s,UmiNativeCompileStage stage,bool passed){if(s==NULL||!s->active||s->completed)return UMI_STATUS_INVALID_STATE;UmiStatus st=umi_nc_compilation_pipeline_mark(&s->pipeline,stage,passed?UMI_NC_STAGE_PASSED:UMI_NC_STAGE_FAILED,passed?UMI_STATUS_OK:UMI_STATUS_INTERNAL_ERROR);if(!passed){s->errors++;s->diagnostics++;}return st;}
UmiStatus umi_nc_compiler_session_finish(UmiNativeCompilerSession *s){if(s==NULL||!s->active)return UMI_STATUS_INVALID_STATE;if(s->errors!=0U)return UMI_STATUS_INVALID_STATE;for(size_t i=0U;i<s->pipeline.count;i++){if(s->pipeline.records[i].stage==UMI_NC_STAGE_COMPLETE)continue;if(s->pipeline.records[i].state==UMI_NC_STAGE_PENDING)return UMI_STATUS_BUSY;if(s->pipeline.records[i].state==UMI_NC_STAGE_FAILED)return UMI_STATUS_INVALID_STATE;}umi_nc_compilation_pipeline_mark(&s->pipeline,UMI_NC_STAGE_COMPLETE,UMI_NC_STAGE_PASSED,UMI_STATUS_OK);s->completed=true;s->active=false;return UMI_STATUS_OK;}
