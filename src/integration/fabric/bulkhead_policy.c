/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/bulkhead_policy.c
 *
 * PURPOSE:
 *   Enforce bounded concurrency and queue capacity before integration work enters shared executors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/bulkhead_policy.h"
#include <string.h>
#include <limits.h>


/*
 * Provide the fabric bulkhead policy default operation used by this module and its client
 * applications.
 */
void umi_fabric_bulkhead_policy_default(UmiFabricBulkheadPolicy *policy){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(policy!=NULL){policy->maximum_concurrent=16U;policy->maximum_queued=64U;}}
/*
 * Provide the fabric bulkhead policy evaluate operation used by this module and its client
 * applications.
 */
UmiStatus umi_fabric_bulkhead_policy_evaluate(const UmiFabricBulkheadPolicy *p,uint32_t active,uint32_t queued,bool *out_admit,bool *out_queue){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||out_admit==NULL||out_queue==NULL||p->maximum_concurrent==0U)return UMI_STATUS_INVALID_ARGUMENT;*out_admit=active<p->maximum_concurrent;*out_queue=!(*out_admit)&&queued<p->maximum_queued;return UMI_STATUS_OK;}
