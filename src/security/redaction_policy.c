/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/redaction_policy.c
 *
 * PURPOSE:
 *   Implement the redaction policy behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Structured redaction policy v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/security/redaction_policy.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
/*
 * Provide the equals ignore case operation used by this module and its client
 * applications.
 */
static bool equals_ignore_case(const char *first,const char *second)
{
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*first != '\0' && *second != '\0') { /* Apply this branch only when its contract condition is satisfied. */ if (tolower((unsigned char)*first) != tolower((unsigned char)*second)) return false; ++first; ++second; }
    return *first == *second;
}
/*
 * Provide the security redaction policy default operation used by this module and its
 * client applications.
 */
void umi_security_redaction_policy_default(UmiSecurityRedactionPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) return;
    (void)memset(policy,0,sizeof(*policy));
    (void)snprintf(policy->marker,sizeof(policy->marker),"[REDACTED]");
    (void)umi_security_redaction_policy_add(policy,"password");
    (void)umi_security_redaction_policy_add(policy,"secret");
    (void)umi_security_redaction_policy_add(policy,"token");
    (void)umi_security_redaction_policy_add(policy,"api_key");
    (void)umi_security_redaction_policy_add(policy,"authorization");
}
/*
 * Add security redaction policy only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_security_redaction_policy_add(UmiSecurityRedactionPolicy *policy,const char *field_name)
{
    size_t index;
    int length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || field_name == NULL || field_name[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < policy->field_count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (equals_ignore_case(policy->sensitive_fields[index],field_name)) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (policy->field_count >= 32U) return UMI_STATUS_CAPACITY_EXCEEDED;
    length = snprintf(policy->sensitive_fields[policy->field_count],sizeof(policy->sensitive_fields[0]),"%s",field_name);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length < 0 || (size_t)length >= sizeof(policy->sensitive_fields[0])) return UMI_STATUS_CAPACITY_EXCEEDED;
    policy->field_count += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the security redaction policy sensitive operation used by this module and its
 * client applications.
 */
bool umi_security_redaction_policy_sensitive(const UmiSecurityRedactionPolicy *policy,const char *field_name)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || field_name == NULL) return false;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < policy->field_count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (equals_ignore_case(policy->sensitive_fields[index],field_name)) return true;
    return false;
}
/*
 * Copy security redaction into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_security_redaction_copy(const UmiSecurityRedactionPolicy *policy,const char *field_name,const char *value,char *out_value,size_t capacity)
{
    const char *source;
    int length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || field_name == NULL || value == NULL || out_value == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    source = umi_security_redaction_policy_sensitive(policy,field_name) ? policy->marker : value;
    length = snprintf(out_value,capacity,"%s",source);
    return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
