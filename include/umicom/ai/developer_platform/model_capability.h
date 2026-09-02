/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/model_capability.h
 *
 * PURPOSE:
 *   Evaluate required model capabilities before routing a developer request.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_MODEL_CAPABILITY_H
#define UMICOM_AI_DEVELOPER_PLATFORM_MODEL_CAPABILITY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev model capability data shared with callers of this public contract.
 */
typedef struct UmiAiDevModelCapability {
    uint64_t allowed_flags;
    uint64_t denied_flags;
    UmiAiDevRisk maximum_risk;
    UmiAiDevLocality locality;
    int require_approval;
    int enabled;
} UmiAiDevModelCapability;

/**
 * Initialise ai dev model capability from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_model_capability_init(UmiAiDevModelCapability *policy);
/**
 * Provide the ai dev model capability configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_dev_model_capability_configure(UmiAiDevModelCapability *policy, uint64_t allowed_flags, uint64_t denied_flags, UmiAiDevRisk maximum_risk, UmiAiDevLocality locality, int require_approval);
/**
 * Provide the ai dev model capability allows operation used by this module and its client
 * applications.
 */
int umi_ai_dev_model_capability_allows(const UmiAiDevModelCapability *policy, uint64_t requested_flags, UmiAiDevRisk risk, UmiAiDevLocality locality);
/**
 * Provide the ai dev model capability approval required operation used by this module and
 * its client applications.
 */
int umi_ai_dev_model_capability_approval_required(const UmiAiDevModelCapability *policy, UmiAiDevRisk risk);

#ifdef __cplusplus
}
#endif

#endif
