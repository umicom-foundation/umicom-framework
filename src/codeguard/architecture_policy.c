/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/architecture_policy.c
 *
 * PURPOSE:
 *   Implement the architecture policy behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | CodeGuard architecture policy | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/codeguard/architecture_policy.h"
#include <stdio.h>
#include <string.h>
/* Provide the add rule operation used by this module and its client applications. */
static void add_rule(UmiCodeGuardArchitecturePolicy *policy,UmiCodeGuardArchitectureLayer source,UmiCodeGuardArchitectureLayer target,bool allowed,const char *reason)
{
    UmiCodeGuardArchitectureRule *rule;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (policy->count >= UMI_CODEGUARD_QUALITY_MAX_POLICIES) return;
    rule = &policy->rules[policy->count++];
    rule->source = source;
    rule->target = target;
    rule->allowed = allowed;
    (void)umi_codeguard_quality_copy(rule->reason,sizeof(rule->reason),reason);
}
/*
 * Provide the codeguard architecture policy default operation used by this module and its
 * client applications.
 */
void umi_codeguard_architecture_policy_default(UmiCodeGuardArchitecturePolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) return;
    (void)memset(policy,0,sizeof(*policy));
    add_rule(policy,UMI_CODEGUARD_LAYER_FRAMEWORK_PUBLIC,UMI_CODEGUARD_LAYER_FRAMEWORK_PRIVATE,false,"Public Framework headers cannot depend on private implementation headers");
    add_rule(policy,UMI_CODEGUARD_LAYER_STUDIO,UMI_CODEGUARD_LAYER_FRAMEWORK_PRIVATE,false,"Studio consumes exported Framework APIs only");
    add_rule(policy,UMI_CODEGUARD_LAYER_APPLICATION,UMI_CODEGUARD_LAYER_FRAMEWORK_PRIVATE,false,"Applications consume public Framework contracts only");
    add_rule(policy,UMI_CODEGUARD_LAYER_PLUGIN,UMI_CODEGUARD_LAYER_FRAMEWORK_PRIVATE,false,"Plug-ins must remain behind the stable public ABI");
}
/*
 * Provide the codeguard architecture is private header operation used by this module and
 * its client applications.
 */
bool umi_codeguard_architecture_is_private_header(const char *path)
{
    return path != NULL && (strstr(path,"/src/") != NULL || strstr(path,"\\src\\") != NULL || strstr(path,"/internal/") != NULL || strstr(path,"_private.h") != NULL);
}
/*
 * Provide the codeguard architecture classify operation used by this module and its client
 * applications.
 */
UmiCodeGuardArchitectureLayer umi_codeguard_architecture_classify(const char *path)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL) return UMI_CODEGUARD_LAYER_UNKNOWN;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strstr(path,"applications/studio/") != NULL) return UMI_CODEGUARD_LAYER_STUDIO;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strstr(path,"plugins/") != NULL) return UMI_CODEGUARD_LAYER_PLUGIN;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strstr(path,"framework/include/") != NULL || strstr(path,"include/umicom/") != NULL) return UMI_CODEGUARD_LAYER_FRAMEWORK_PUBLIC;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strstr(path,"framework/src/") != NULL || strstr(path,"src/") != NULL || umi_codeguard_architecture_is_private_header(path)) return UMI_CODEGUARD_LAYER_FRAMEWORK_PRIVATE;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strstr(path,"applications/") != NULL) return UMI_CODEGUARD_LAYER_APPLICATION;
    return UMI_CODEGUARD_LAYER_UNKNOWN;
}
/*
 * Provide the codeguard architecture check dependency operation used by this module and
 * its client applications.
 */
UmiStatus umi_codeguard_architecture_check_dependency(const UmiCodeGuardArchitecturePolicy *policy,const char *source_path,const char *target_path,UmiCodeGuardEvidenceStore *evidence)
{
    size_t index;
    UmiCodeGuardArchitectureLayer source;
    UmiCodeGuardArchitectureLayer target;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || source_path == NULL || target_path == NULL || evidence == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    source = umi_codeguard_architecture_classify(source_path);
    target = umi_codeguard_architecture_classify(target_path);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < policy->count; ++index) {
        const UmiCodeGuardArchitectureRule *rule = &policy->rules[index];
        /* Apply this operation only while the related capability or state is available. */
        if (rule->source == source && rule->target == target && !rule->allowed) {
            UmiCodeGuardEvidence item = {0};
            int length = snprintf(item.id,sizeof(item.id),"ARCH-%zu",evidence->revision + 1U);
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (length < 0 || (size_t)length >= sizeof(item.id)) return UMI_STATUS_CAPACITY_EXCEEDED;
            item.kind = UMI_CODEGUARD_EVIDENCE_ARCHITECTURE;
            item.state = UMI_CODEGUARD_EVIDENCE_FAIL;
            item.observed = 1U;
            item.allowed = 0U;
            (void)umi_codeguard_quality_copy(item.path,sizeof(item.path),source_path);
            (void)umi_codeguard_quality_copy(item.summary,sizeof(item.summary),rule->reason);
            (void)umi_codeguard_quality_copy(item.remediation,sizeof(item.remediation),"Replace the dependency with an installed public header or stable exported contract");
            return umi_codeguard_evidence_add(evidence,&item);
        }
    }
    return UMI_STATUS_OK;
}
