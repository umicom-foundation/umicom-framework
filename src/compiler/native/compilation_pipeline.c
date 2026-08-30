/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/compilation_pipeline.c
 *
 * PURPOSE:
 *   Track deterministic compiler stages, status and readiness from source through link completion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/compilation_pipeline.h"
#include <string.h>
void umi_nc_compilation_pipeline_init(UmiNativeCompilationPipeline *p,bool link){if(p==NULL)return;memset(p,0,sizeof(*p));p->link_required=link;for(int s=(int)UMI_NC_STAGE_SOURCE;s<=(int)UMI_NC_STAGE_COMPLETE;s++){if(!link&&s==(int)UMI_NC_STAGE_LINK)continue;p->records[p->count].stage=(UmiNativeCompileStage)s;p->records[p->count].state=UMI_NC_STAGE_PENDING;p->records[p->count].status=UMI_STATUS_OK;p->count++;}}
UmiStatus umi_nc_compilation_pipeline_mark(UmiNativeCompilationPipeline *p,UmiNativeCompileStage stage,UmiNativeStageState state,UmiStatus status){if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(size_t i=0U;i<p->count;i++)if(p->records[i].stage==stage){p->records[i].state=state;p->records[i].status=status;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
const UmiNativeStageRecord *umi_nc_compilation_pipeline_next(const UmiNativeCompilationPipeline *p){if(p==NULL)return NULL;for(size_t i=0U;i<p->count;i++)if(p->records[i].state==UMI_NC_STAGE_PENDING)return &p->records[i];return NULL;}
bool umi_nc_compilation_pipeline_ready(const UmiNativeCompilationPipeline *p){if(p==NULL)return false;for(size_t i=0U;i<p->count;i++){if(p->records[i].state==UMI_NC_STAGE_FAILED)return false;if(p->records[i].stage!=UMI_NC_STAGE_COMPLETE&&p->records[i].state!=UMI_NC_STAGE_PASSED&&p->records[i].state!=UMI_NC_STAGE_SKIPPED)return false;}return true;}
