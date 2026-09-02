/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/model_fallback.h
 *
 * PURPOSE:
 *   Decide whether a failed request may fail over to another model.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_MODEL_FALLBACK_H
#define UMICOM_AI_DEVELOPER_PLATFORM_MODEL_FALLBACK_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev model fallback data shared with callers of this public contract.
 */
typedef struct UmiAiDevModelFallback {
    uint64_t allowed_flags;
    uint64_t denied_flags;
    UmiAiDevRisk maximum_risk;
    UmiAiDevLocality locality;
    int require_approval;
    int enabled;
} UmiAiDevModelFallback;

/**
 * Initialise ai dev model fallback from caller-provided values so later operations receive
 * a known state.
 */
void umi_ai_dev_model_fallback_init(UmiAiDevModelFallback *policy);
/**
 * Provide the ai dev model fallback configure operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_model_fallback_configure(UmiAiDevModelFallback *policy, uint64_t allowed_flags, uint64_t denied_flags, UmiAiDevRisk maximum_risk, UmiAiDevLocality locality, int require_approval);
/**
 * Provide the ai dev model fallback allows operation used by this module and its client
 * applications.
 */
int umi_ai_dev_model_fallback_allows(const UmiAiDevModelFallback *policy, uint64_t requested_flags, UmiAiDevRisk risk, UmiAiDevLocality locality);
/**
 * Provide the ai dev model fallback approval required operation used by this module and
 * its client applications.
 */
int umi_ai_dev_model_fallback_approval_required(const UmiAiDevModelFallback *policy, UmiAiDevRisk risk);

#ifdef __cplusplus
}
#endif

#endif
