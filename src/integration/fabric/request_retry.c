/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/request_retry.c
 *
 * PURPOSE:
 *   Calculate bounded exponential retry delays while leaving execution to existing resilience services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/request_retry.h"
#include <string.h>
#include <limits.h>


void umi_fabric_request_retry_default(UmiFabricRequestRetry *policy){if(policy!=NULL){policy->max_attempts=4U;policy->initial_delay_ms=100U;policy->maximum_delay_ms=5000U;policy->multiplier=2.0;}}
UmiStatus umi_fabric_request_retry_delay(const UmiFabricRequestRetry *policy,uint32_t attempt,uint64_t *out_delay_ms){uint32_t i;double v;if(policy==NULL||out_delay_ms==NULL||attempt==0U||attempt>policy->max_attempts||policy->multiplier<1.0)return UMI_STATUS_INVALID_ARGUMENT;v=(double)policy->initial_delay_ms;for(i=1U;i<attempt;++i){v*=policy->multiplier;if(v>=(double)policy->maximum_delay_ms){v=(double)policy->maximum_delay_ms;break;}}*out_delay_ms=(uint64_t)v;return UMI_STATUS_OK;}
