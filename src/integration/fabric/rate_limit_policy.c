/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/rate_limit_policy.c
 *
 * PURPOSE:
 *   Apply a deterministic fixed-window rate-limit policy using caller-supplied counters and time.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/rate_limit_policy.h"
#include <string.h>
#include <limits.h>


/*
 * Provide the fabric rate limit policy default operation used by this module and its
 * client applications.
 */
void umi_fabric_rate_limit_policy_default(UmiFabricRateLimitPolicy *policy){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(policy!=NULL){policy->maximum_requests=100U;policy->window_ms=1000U;}}
/*
 * Provide the fabric rate limit policy evaluate operation used by this module and its
 * client applications.
 */
UmiStatus umi_fabric_rate_limit_policy_evaluate(const UmiFabricRateLimitPolicy *policy,uint32_t requests_in_window,bool *out_allowed,uint32_t *out_remaining){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(policy==NULL||out_allowed==NULL||out_remaining==NULL||policy->maximum_requests==0U||policy->window_ms==0U)return UMI_STATUS_INVALID_ARGUMENT;*out_allowed=requests_in_window<policy->maximum_requests;*out_remaining=requests_in_window>=policy->maximum_requests?0U:policy->maximum_requests-requests_in_window;return UMI_STATUS_OK;}
