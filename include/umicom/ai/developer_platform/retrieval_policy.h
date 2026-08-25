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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiAiDevRetrievalPolicy {
    uint64_t allowed_flags;
    uint64_t denied_flags;
    UmiAiDevRisk maximum_risk;
    UmiAiDevLocality locality;
    int require_approval;
    int enabled;
} UmiAiDevRetrievalPolicy;

void umi_ai_dev_retrieval_policy_init(UmiAiDevRetrievalPolicy *policy);
UmiStatus umi_ai_dev_retrieval_policy_configure(UmiAiDevRetrievalPolicy *policy, uint64_t allowed_flags, uint64_t denied_flags, UmiAiDevRisk maximum_risk, UmiAiDevLocality locality, int require_approval);
int umi_ai_dev_retrieval_policy_allows(const UmiAiDevRetrievalPolicy *policy, uint64_t requested_flags, UmiAiDevRisk risk, UmiAiDevLocality locality);
int umi_ai_dev_retrieval_policy_approval_required(const UmiAiDevRetrievalPolicy *policy, UmiAiDevRisk risk);

#ifdef __cplusplus
}
#endif

#endif
