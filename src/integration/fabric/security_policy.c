/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/security_policy.c
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
#include "umicom/integration/fabric/security_policy.h"
#include <string.h>
#include <limits.h>


void umi_fabric_security_policy_default(UmiFabricSecurityPolicy *policy){if(policy!=NULL){policy->tls_required=true;policy->authentication_required=true;policy->tenant_required=true;policy->payload_encryption_required=false;}}
UmiStatus umi_fabric_security_policy_evaluate(const UmiFabricSecurityPolicy *p,bool secure_transport,bool authenticated,const char *tenant_id,bool payload_encrypted,bool *out_allowed){if(p==NULL||out_allowed==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_allowed=true;if(p->tls_required&&!secure_transport)*out_allowed=false;if(p->authentication_required&&!authenticated)*out_allowed=false;if(p->tenant_required&&(tenant_id==NULL||tenant_id[0]=='\0'))*out_allowed=false;if(p->payload_encryption_required&&!payload_encrypted)*out_allowed=false;return UMI_STATUS_OK;}
