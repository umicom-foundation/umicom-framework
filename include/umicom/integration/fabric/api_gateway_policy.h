/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/api_gateway_policy.h
 *
 * PURPOSE:
 *   Evaluate authentication, payload-size and operation-idempotency gates before API dispatch.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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


typedef struct UmiFabricApiGatewayPolicy { bool authentication_required; bool reject_non_idempotent_retries; uint64_t maximum_payload_bytes; } UmiFabricApiGatewayPolicy;
void umi_fabric_api_gateway_policy_default(UmiFabricApiGatewayPolicy *policy);
UmiStatus umi_fabric_api_gateway_policy_evaluate(const UmiFabricApiGatewayPolicy *policy,const UmiFabricApiOperation *operation,bool authenticated,bool retry,uint64_t payload_bytes,bool *out_allowed);

#ifdef __cplusplus
}
#endif
#endif
