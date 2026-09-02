/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/api_gateway_policy.h
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
#ifndef UMICOM_INTEGRATION_FABRIC_API_GATEWAY_POLICY_H
#define UMICOM_INTEGRATION_FABRIC_API_GATEWAY_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"
#include "umicom/integration/fabric/api_operation.h"
#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the fabric api gateway policy data shared with callers of this public
 * contract.
 */
typedef struct UmiFabricApiGatewayPolicy { bool authentication_required; bool reject_non_idempotent_retries; uint64_t maximum_payload_bytes; } UmiFabricApiGatewayPolicy;
/**
 * Provide the fabric api gateway policy default operation used by this module and its
 * client applications.
 */
void umi_fabric_api_gateway_policy_default(UmiFabricApiGatewayPolicy *policy);
/**
 * Provide the fabric api gateway policy evaluate operation used by this module and its
 * client applications.
 */
UmiStatus umi_fabric_api_gateway_policy_evaluate(const UmiFabricApiGatewayPolicy *policy,const UmiFabricApiOperation *operation,bool authenticated,bool retry,uint64_t payload_bytes,bool *out_allowed);

#ifdef __cplusplus
}
#endif
#endif
