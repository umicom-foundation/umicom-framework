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
/*
 * Initialise nc compilation pipeline from caller-provided values so later operations
 * receive a known state.
 */
void umi_nc_compilation_pipeline_init(UmiNativeCompilationPipeline *p,bool link){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return;memset(p,0,sizeof(*p));p->link_required=link;/* Visit each bounded item once so every record receives the same rule. */ for(int s=(int)UMI_NC_STAGE_SOURCE;s<=(int)UMI_NC_STAGE_COMPLETE;s++){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!link&&s==(int)UMI_NC_STAGE_LINK)continue;p->records[p->count].stage=(UmiNativeCompileStage)s;p->records[p->count].state=UMI_NC_STAGE_PENDING;p->records[p->count].status=UMI_STATUS_OK;p->count++;}}
/*
 * Provide the nc compilation pipeline mark operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_compilation_pipeline_mark(UmiNativeCompilationPipeline *p,UmiNativeCompileStage stage,UmiNativeStageState state,UmiStatus status){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<p->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->records[i].stage==stage){p->records[i].state=state;p->records[i].status=status;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
/*
 * Provide the nc compilation pipeline next operation used by this module and its client
 * applications.
 */
const UmiNativeStageRecord *umi_nc_compilation_pipeline_next(const UmiNativeCompilationPipeline *p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<p->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->records[i].state==UMI_NC_STAGE_PENDING)return &p->records[i];return NULL;}
/*
 * Provide the nc compilation pipeline ready operation used by this module and its client
 * applications.
 */
bool umi_nc_compilation_pipeline_ready(const UmiNativeCompilationPipeline *p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return false;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<p->count;i++){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->records[i].state==UMI_NC_STAGE_FAILED)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->records[i].stage!=UMI_NC_STAGE_COMPLETE&&p->records[i].state!=UMI_NC_STAGE_PASSED&&p->records[i].state!=UMI_NC_STAGE_SKIPPED)return false;}return true;}
