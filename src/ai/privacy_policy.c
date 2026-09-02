/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/privacy_policy.c
 *
 * PURPOSE:
 *   Enforce classification-aware AI sharing and conversation persistence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A classification is checked before sharing or saving.  Approval can satisfy
 * an approval requirement, but it cannot raise a configured privacy maximum.
 */
#include "umicom/ai/privacy_policy.h"

/*
 * Provide the ai privacy policy default operation used by this module and its client
 * applications.
 */
UmiAiPrivacyPolicy umi_ai_privacy_policy_default(void)
{
    UmiAiPrivacyPolicy policy;
    policy.maximum_local_classification = UMI_AI_DATA_CONFIDENTIAL;
    policy.maximum_remote_classification = UMI_AI_DATA_PUBLIC;
    policy.persist_sessions = 1;
    policy.persist_prompt_text = 1;
    policy.require_sensitive_approval = 1;
    return policy;
}

/* Check that classification satisfies its contract before another service relies on it. */
static int classification_valid(UmiAiDataClassification classification)
{
    return classification >= UMI_AI_DATA_PUBLIC &&
           classification <= UMI_AI_DATA_RESTRICTED;
}

/*
 * Provide the ai privacy policy check share operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_privacy_policy_check_share(
    const UmiAiPrivacyPolicy *policy,
    UmiAiProviderKind provider_kind,
    UmiAiDataClassification classification,
    int approved)
{
    UmiAiDataClassification maximum;
    int remote;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || !classification_valid(classification)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    remote = provider_kind == UMI_AI_PROVIDER_REMOTE;
    maximum = remote ? policy->maximum_remote_classification
                     : policy->maximum_local_classification;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (classification > maximum) return UMI_STATUS_PERMISSION_DENIED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->require_sensitive_approval &&
        classification >= UMI_AI_DATA_CONFIDENTIAL && !approved) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the ai privacy policy check persistence operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_privacy_policy_check_persistence(
    const UmiAiPrivacyPolicy *policy,
    UmiAiDataClassification classification,
    int approved)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || !classification_valid(classification)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!policy->persist_sessions || !policy->persist_prompt_text) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->require_sensitive_approval &&
        classification >= UMI_AI_DATA_CONFIDENTIAL && !approved) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the ai data classification text operation used by this module and its client
 * applications.
 */
const char *umi_ai_data_classification_text(
    UmiAiDataClassification classification)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (classification) {
        case UMI_AI_DATA_PUBLIC: return "public";
        case UMI_AI_DATA_INTERNAL: return "internal";
        case UMI_AI_DATA_CONFIDENTIAL: return "confidential";
        case UMI_AI_DATA_RESTRICTED: return "restricted";
        default: return "unknown";
    }
}
