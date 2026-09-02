/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/origin.c
 *
 * PURPOSE:
 *   Implement a browser origin allowlist.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/origin.h"
#include <stdlib.h>
#include <string.h>
struct UmiWebOriginPolicy{char origins[UMI_WEB_MAX_ORIGINS][256];size_t count;};
/*
 * Initialise web origin policy from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_web_origin_policy_create(UmiWebOriginPolicy **out_policy){UmiWebOriginPolicy *p;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_policy==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_policy=NULL;p=(UmiWebOriginPolicy*)calloc(1U,sizeof(*p));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return UMI_STATUS_OUT_OF_MEMORY;*out_policy=p;return UMI_STATUS_OK;}
/*
 * Release or reset state held by web origin policy so the same storage can be reused
 * safely.
 */
void umi_web_origin_policy_destroy(UmiWebOriginPolicy *policy){free(policy);}
/* Provide the web origin allow operation used by this module and its client applications. */
UmiStatus umi_web_origin_allow(UmiWebOriginPolicy *policy,const char *origin){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(policy==NULL||origin==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(policy->count>=UMI_WEB_MAX_ORIGINS)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_web_copy_text(policy->origins[policy->count],sizeof(policy->origins[policy->count]),origin)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;++policy->count;return UMI_STATUS_OK;}
/*
 * Provide the web origin is allowed operation used by this module and its client
 * applications.
 */
int umi_web_origin_is_allowed(const UmiWebOriginPolicy *policy,const char *origin){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(policy==NULL||origin==NULL)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<policy->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(policy->origins[i],origin)==0)return 1;return 0;}
/* Return the number of records represented by web origin without changing their state. */
size_t umi_web_origin_count(const UmiWebOriginPolicy *policy){return policy!=NULL?policy->count:0U;}
