/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/runtime_selector.c
 *
 * PURPOSE:
 *   best-compatible runtime profile selection using explicit requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/runtime_selector.h"

UmiStatus umi_dr_runtime_selector_choose(const UmiDrRuntimeProfile *p,size_t count,const UmiDrRuntimeProbe *host,size_t *out){size_t i,best=0U;uint64_t score=0U;bool found=false;if(p==NULL||host==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<count;++i){uint64_t s;if(p[i].platform!=host->platform||p[i].architecture!=host->architecture)continue;if(!umi_dr_version_at_least(host->version,p[i].minimum_version))continue;if((p[i].required_capabilities & ~host->capabilities)!=0U)continue;s=p[i].score_bias+1U;if(!found||s>score){found=true;score=s;best=i;}}if(!found)return UMI_STATUS_NOT_FOUND;*out=best;return UMI_STATUS_OK;}
