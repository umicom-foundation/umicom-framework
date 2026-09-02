/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/retrieval_policy.h
 *
 * PURPOSE:
 *   Gate retrieval candidates by scope, trust and minimum score.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_RETRIEVAL_POLICY_H
#define UMICOM_AI_DEVELOPER_PLATFORM_RETRIEVAL_POLICY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev retrieval policy data shared with callers of this public contract.
 */
typedef struct UmiAiDevRetrievalPolicy {
    uint64_t allowed_flags;
    uint64_t denied_flags;
    UmiAiDevRisk maximum_risk;
    UmiAiDevLocality locality;
    int require_approval;
    int enabled;
} UmiAiDevRetrievalPolicy;

/**
 * Initialise ai dev retrieval policy from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_retrieval_policy_init(UmiAiDevRetrievalPolicy *policy);
/**
 * Provide the ai dev retrieval policy configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_dev_retrieval_policy_configure(UmiAiDevRetrievalPolicy *policy, uint64_t allowed_flags, uint64_t denied_flags, UmiAiDevRisk maximum_risk, UmiAiDevLocality locality, int require_approval);
/**
 * Provide the ai dev retrieval policy allows operation used by this module and its client
 * applications.
 */
int umi_ai_dev_retrieval_policy_allows(const UmiAiDevRetrievalPolicy *policy, uint64_t requested_flags, UmiAiDevRisk risk, UmiAiDevLocality locality);
/**
 * Provide the ai dev retrieval policy approval required operation used by this module and
 * its client applications.
 */
int umi_ai_dev_retrieval_policy_approval_required(const UmiAiDevRetrievalPolicy *policy, UmiAiDevRisk risk);

#ifdef __cplusplus
}
#endif

#endif
