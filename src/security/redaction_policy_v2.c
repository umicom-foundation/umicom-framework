/* Umicom Framework | Structured redaction policy v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/security/redaction_policy_v2.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
static bool equals_ignore_case(const char *first,const char *second)
{
    while (*first != '\0' && *second != '\0') { if (tolower((unsigned char)*first) != tolower((unsigned char)*second)) return false; ++first; ++second; }
    return *first == *second;
}
void umi_security_redaction_policy_default(UmiSecurityRedactionPolicy *policy)
{
    if (policy == NULL) return;
    (void)memset(policy,0,sizeof(*policy));
    (void)snprintf(policy->marker,sizeof(policy->marker),"[REDACTED]");
    (void)umi_security_redaction_policy_add(policy,"password");
    (void)umi_security_redaction_policy_add(policy,"secret");
    (void)umi_security_redaction_policy_add(policy,"token");
    (void)umi_security_redaction_policy_add(policy,"api_key");
    (void)umi_security_redaction_policy_add(policy,"authorization");
}
UmiStatus umi_security_redaction_policy_add(UmiSecurityRedactionPolicy *policy,const char *field_name)
{
    size_t index;
    int length;
    if (policy == NULL || field_name == NULL || field_name[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < policy->field_count; ++index) if (equals_ignore_case(policy->sensitive_fields[index],field_name)) return UMI_STATUS_ALREADY_EXISTS;
    if (policy->field_count >= 32U) return UMI_STATUS_CAPACITY_EXCEEDED;
    length = snprintf(policy->sensitive_fields[policy->field_count],sizeof(policy->sensitive_fields[0]),"%s",field_name);
    if (length < 0 || (size_t)length >= sizeof(policy->sensitive_fields[0])) return UMI_STATUS_CAPACITY_EXCEEDED;
    policy->field_count += 1U;
    return UMI_STATUS_OK;
}
bool umi_security_redaction_policy_sensitive(const UmiSecurityRedactionPolicy *policy,const char *field_name)
{
    size_t index;
    if (policy == NULL || field_name == NULL) return false;
    for (index = 0U; index < policy->field_count; ++index) if (equals_ignore_case(policy->sensitive_fields[index],field_name)) return true;
    return false;
}
UmiStatus umi_security_redaction_copy(const UmiSecurityRedactionPolicy *policy,const char *field_name,const char *value,char *out_value,size_t capacity)
{
    const char *source;
    int length;
    if (policy == NULL || field_name == NULL || value == NULL || out_value == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    source = umi_security_redaction_policy_sensitive(policy,field_name) ? policy->marker : value;
    length = snprintf(out_value,capacity,"%s",source);
    return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
