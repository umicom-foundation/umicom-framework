/* Umicom Framework | Structured redaction policy v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_SECURITY_REDACTION_POLICY_H
#define UMICOM_SECURITY_REDACTION_POLICY_H
#include "umicom/security/governance.h"
typedef struct UmiSecurityRedactionPolicy {
    char sensitive_fields[32U][UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    size_t field_count;
    char marker[32U];
} UmiSecurityRedactionPolicy;
void umi_security_redaction_policy_default(UmiSecurityRedactionPolicy *policy);
UmiStatus umi_security_redaction_policy_add(UmiSecurityRedactionPolicy *policy,const char *field_name);
bool umi_security_redaction_policy_sensitive(const UmiSecurityRedactionPolicy *policy,const char *field_name);
UmiStatus umi_security_redaction_copy(const UmiSecurityRedactionPolicy *policy,const char *field_name,const char *value,char *out_value,size_t capacity);
#endif
