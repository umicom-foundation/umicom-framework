/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/request_retry.c
 *
 * PURPOSE:
 *   Calculate bounded exponential retry delays while leaving execution to existing resilience services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/request_retry.h"
#include <string.h>
#include <limits.h>


/*
 * Provide the fabric request retry default operation used by this module and its client
 * applications.
 */
void umi_fabric_request_retry_default(UmiFabricRequestRetry *policy){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(policy!=NULL){policy->max_attempts=4U;policy->initial_delay_ms=100U;policy->maximum_delay_ms=5000U;policy->multiplier=2.0;}}
/*
 * Provide the fabric request retry delay operation used by this module and its client
 * applications.
 */
UmiStatus umi_fabric_request_retry_delay(const UmiFabricRequestRetry *policy,uint32_t attempt,uint64_t *out_delay_ms){uint32_t i;double v;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(policy==NULL||out_delay_ms==NULL||attempt==0U||attempt>policy->max_attempts||policy->multiplier<1.0)return UMI_STATUS_INVALID_ARGUMENT;v=(double)policy->initial_delay_ms;/* Visit each bounded item once so every record receives the same rule. */ for(i=1U;i<attempt;++i){v*=policy->multiplier;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v>=(double)policy->maximum_delay_ms){v=(double)policy->maximum_delay_ms;break;}}*out_delay_ms=(uint64_t)v;return UMI_STATUS_OK;}
