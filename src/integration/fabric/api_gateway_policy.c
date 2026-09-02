/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/api_gateway_policy.c
 *
 * PURPOSE:
 *   Evaluate authentication, payload-size and operation-idempotency gates before API dispatch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/api_gateway_policy.h"
#include <string.h>
#include <limits.h>


/*
 * Provide the fabric api gateway policy default operation used by this module and its
 * client applications.
 */
void umi_fabric_api_gateway_policy_default(UmiFabricApiGatewayPolicy *policy){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(policy!=NULL){policy->authentication_required=true;policy->reject_non_idempotent_retries=true;policy->maximum_payload_bytes=UINT64_C(1048576);}}
/*
 * Provide the fabric api gateway policy evaluate operation used by this module and its
 * client applications.
 */
UmiStatus umi_fabric_api_gateway_policy_evaluate(const UmiFabricApiGatewayPolicy *policy,const UmiFabricApiOperation *operation,bool authenticated,bool retry,uint64_t payload_bytes,bool *out_allowed){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(policy==NULL||operation==NULL||out_allowed==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_allowed=true;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(policy->authentication_required&&!authenticated)*out_allowed=false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(payload_bytes>policy->maximum_payload_bytes)*out_allowed=false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(retry&&policy->reject_non_idempotent_retries&&!operation->idempotent)*out_allowed=false;return UMI_STATUS_OK;}
