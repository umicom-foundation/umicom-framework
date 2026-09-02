/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/security_policy.h
 *
 * PURPOSE:
 *   Evaluate transport security, authentication and tenant-isolation evidence before integration dispatch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_SECURITY_POLICY_H
#define UMICOM_INTEGRATION_FABRIC_SECURITY_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the fabric security policy data shared with callers of this public contract.
 */
typedef struct UmiFabricSecurityPolicy { bool tls_required; bool authentication_required; bool tenant_required; bool payload_encryption_required; } UmiFabricSecurityPolicy;
/**
 * Provide the fabric security policy default operation used by this module and its client
 * applications.
 */
void umi_fabric_security_policy_default(UmiFabricSecurityPolicy *policy);
/**
 * Provide the fabric security policy evaluate operation used by this module and its client
 * applications.
 */
UmiStatus umi_fabric_security_policy_evaluate(const UmiFabricSecurityPolicy *policy,bool secure_transport,bool authenticated,const char *tenant_id,bool payload_encrypted,bool *out_allowed);

#ifdef __cplusplus
}
#endif
#endif
