/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/origin.c
 *
 * PURPOSE:
 *   Implement a browser origin allowlist.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/origin.h"
#include <stdlib.h>
#include <string.h>
struct UmiWebOriginPolicy{char origins[UMI_WEB_MAX_ORIGINS][256];size_t count;};
UmiStatus umi_web_origin_policy_create(UmiWebOriginPolicy **out_policy){UmiWebOriginPolicy *p;if(out_policy==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_policy=NULL;p=(UmiWebOriginPolicy*)calloc(1U,sizeof(*p));if(p==NULL)return UMI_STATUS_OUT_OF_MEMORY;*out_policy=p;return UMI_STATUS_OK;}
void umi_web_origin_policy_destroy(UmiWebOriginPolicy *policy){free(policy);}
UmiStatus umi_web_origin_allow(UmiWebOriginPolicy *policy,const char *origin){if(policy==NULL||origin==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(policy->count>=UMI_WEB_MAX_ORIGINS)return UMI_STATUS_CAPACITY_EXCEEDED;if(umi_web_copy_text(policy->origins[policy->count],sizeof(policy->origins[policy->count]),origin)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;++policy->count;return UMI_STATUS_OK;}
int umi_web_origin_is_allowed(const UmiWebOriginPolicy *policy,const char *origin){size_t i;if(policy==NULL||origin==NULL)return 0;for(i=0U;i<policy->count;++i)if(strcmp(policy->origins[i],origin)==0)return 1;return 0;}
size_t umi_web_origin_count(const UmiWebOriginPolicy *policy){return policy!=NULL?policy->count:0U;}
