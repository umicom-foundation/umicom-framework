/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/redaction_policy.h
 *
 * PURPOSE:
 *   Publish the public redaction policy contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_REDACTION_POLICY_H
#define UMICOM_SECURITY_REDACTION_POLICY_H
#include "umicom/security/governance.h"
/**
 * Represent the security redaction policy data shared with callers of this public
 * contract.
 */
typedef struct UmiSecurityRedactionPolicy {
    char sensitive_fields[32U][UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    size_t field_count;
    char marker[32U];
} UmiSecurityRedactionPolicy;
/**
 * Provide the security redaction policy default operation used by this module and its
 * client applications.
 */
void umi_security_redaction_policy_default(UmiSecurityRedactionPolicy *policy);
/**
 * Add security redaction policy only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_security_redaction_policy_add(UmiSecurityRedactionPolicy *policy,const char *field_name);
/**
 * Provide the security redaction policy sensitive operation used by this module and its
 * client applications.
 */
bool umi_security_redaction_policy_sensitive(const UmiSecurityRedactionPolicy *policy,const char *field_name);
/**
 * Copy security redaction into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_security_redaction_copy(const UmiSecurityRedactionPolicy *policy,const char *field_name,const char *value,char *out_value,size_t capacity);
#endif
