/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/idempotency_policy.c
 *
 * PURPOSE:
 *   Determine when an operation requires a stable idempotency key before retries are permitted.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/idempotency_policy.h"
#include <string.h>
#include <limits.h>


void umi_fabric_idempotency_policy_default(UmiFabricIdempotencyPolicy *policy){if(policy!=NULL){policy->require_key_for_retry=true;policy->require_key_for_write=true;policy->minimum_key_length=8U;}}
UmiStatus umi_fabric_idempotency_policy_evaluate(const UmiFabricIdempotencyPolicy *policy,bool write,bool retry,const char *key,bool *out_allowed){size_t n=key!=NULL?strlen(key):0U;if(policy==NULL||out_allowed==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_allowed=true;if((write&&policy->require_key_for_write)||(retry&&policy->require_key_for_retry))*out_allowed=n>=policy->minimum_key_length;return UMI_STATUS_OK;}
