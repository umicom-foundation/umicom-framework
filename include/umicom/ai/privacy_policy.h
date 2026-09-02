/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/privacy_policy.h
 *
 * PURPOSE:
 *   Apply explicit data-classification, remote-sharing and persistence rules
 *   before context or conversation material crosses an AI boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A model response can never relax this policy.  The application decides which
 * classification may be sent or saved and whether human approval is required.
 */
#ifndef UMICOM_AI_PRIVACY_POLICY_H
#define UMICOM_AI_PRIVACY_POLICY_H

#include "umicom/ai/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named ai data classification values accepted by this public contract.
 */
typedef enum UmiAiDataClassification {
    UMI_AI_DATA_PUBLIC = 1,
    UMI_AI_DATA_INTERNAL = 2,
    UMI_AI_DATA_CONFIDENTIAL = 3,
    UMI_AI_DATA_RESTRICTED = 4
} UmiAiDataClassification;

/**
 * Represent the ai privacy policy data shared with callers of this public contract.
 */
typedef struct UmiAiPrivacyPolicy {
    UmiAiDataClassification maximum_local_classification;
    UmiAiDataClassification maximum_remote_classification;
    int persist_sessions;
    int persist_prompt_text;
    int require_sensitive_approval;
} UmiAiPrivacyPolicy;

/**
 * Provide the ai privacy policy default operation used by this module and its client
 * applications.
 */
UmiAiPrivacyPolicy umi_ai_privacy_policy_default(void);
/**
 * Provide the ai privacy policy check share operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_privacy_policy_check_share(
    const UmiAiPrivacyPolicy *policy,
    UmiAiProviderKind provider_kind,
    UmiAiDataClassification classification,
    int approved);
/**
 * Provide the ai privacy policy check persistence operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_privacy_policy_check_persistence(
    const UmiAiPrivacyPolicy *policy,
    UmiAiDataClassification classification,
    int approved);
/**
 * Provide the ai data classification text operation used by this module and its client
 * applications.
 */
const char *umi_ai_data_classification_text(
    UmiAiDataClassification classification);

#ifdef __cplusplus
}
#endif

#endif
