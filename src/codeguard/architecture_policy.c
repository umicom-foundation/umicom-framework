/* Umicom Framework | CodeGuard architecture policy | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/codeguard/architecture_policy.h"
#include <stdio.h>
#include <string.h>
static void add_rule(UmiCodeGuardArchitecturePolicy *policy,UmiCodeGuardArchitectureLayer source,UmiCodeGuardArchitectureLayer target,bool allowed,const char *reason)
{
    UmiCodeGuardArchitectureRule *rule;
    if (policy->count >= UMI_CODEGUARD_QUALITY_MAX_POLICIES) return;
    rule = &policy->rules[policy->count++];
    rule->source = source;
    rule->target = target;
    rule->allowed = allowed;
    (void)umi_codeguard_quality_copy(rule->reason,sizeof(rule->reason),reason);
}
void umi_codeguard_architecture_policy_default(UmiCodeGuardArchitecturePolicy *policy)
{
    if (policy == NULL) return;
    (void)memset(policy,0,sizeof(*policy));
    add_rule(policy,UMI_CODEGUARD_LAYER_FRAMEWORK_PUBLIC,UMI_CODEGUARD_LAYER_FRAMEWORK_PRIVATE,false,"Public Framework headers cannot depend on private implementation headers");
    add_rule(policy,UMI_CODEGUARD_LAYER_STUDIO,UMI_CODEGUARD_LAYER_FRAMEWORK_PRIVATE,false,"Studio consumes exported Framework APIs only");
    add_rule(policy,UMI_CODEGUARD_LAYER_APPLICATION,UMI_CODEGUARD_LAYER_FRAMEWORK_PRIVATE,false,"Applications consume public Framework contracts only");
    add_rule(policy,UMI_CODEGUARD_LAYER_PLUGIN,UMI_CODEGUARD_LAYER_FRAMEWORK_PRIVATE,false,"Plug-ins must remain behind the stable public ABI");
}
bool umi_codeguard_architecture_is_private_header(const char *path)
{
    return path != NULL && (strstr(path,"/src/") != NULL || strstr(path,"\\src\\") != NULL || strstr(path,"/internal/") != NULL || strstr(path,"_private.h") != NULL);
}
UmiCodeGuardArchitectureLayer umi_codeguard_architecture_classify(const char *path)
{
    if (path == NULL) return UMI_CODEGUARD_LAYER_UNKNOWN;
    if (strstr(path,"applications/studio/") != NULL) return UMI_CODEGUARD_LAYER_STUDIO;
    if (strstr(path,"plugins/") != NULL) return UMI_CODEGUARD_LAYER_PLUGIN;
    if (strstr(path,"framework/include/") != NULL || strstr(path,"include/umicom/") != NULL) return UMI_CODEGUARD_LAYER_FRAMEWORK_PUBLIC;
    if (strstr(path,"framework/src/") != NULL || strstr(path,"src/") != NULL || umi_codeguard_architecture_is_private_header(path)) return UMI_CODEGUARD_LAYER_FRAMEWORK_PRIVATE;
    if (strstr(path,"applications/") != NULL) return UMI_CODEGUARD_LAYER_APPLICATION;
    return UMI_CODEGUARD_LAYER_UNKNOWN;
}
UmiStatus umi_codeguard_architecture_check_dependency(const UmiCodeGuardArchitecturePolicy *policy,const char *source_path,const char *target_path,UmiCodeGuardEvidenceStore *evidence)
{
    size_t index;
    UmiCodeGuardArchitectureLayer source;
    UmiCodeGuardArchitectureLayer target;
    if (policy == NULL || source_path == NULL || target_path == NULL || evidence == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    source = umi_codeguard_architecture_classify(source_path);
    target = umi_codeguard_architecture_classify(target_path);
    for (index = 0U; index < policy->count; ++index) {
        const UmiCodeGuardArchitectureRule *rule = &policy->rules[index];
        if (rule->source == source && rule->target == target && !rule->allowed) {
            UmiCodeGuardEvidence item = {0};
            int length = snprintf(item.id,sizeof(item.id),"ARCH-%zu",evidence->revision + 1U);
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
