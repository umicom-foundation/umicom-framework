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

typedef struct UmiAiDevModelFallback {
    uint64_t allowed_flags;
    uint64_t denied_flags;
    UmiAiDevRisk maximum_risk;
    UmiAiDevLocality locality;
    int require_approval;
    int enabled;
} UmiAiDevModelFallback;

void umi_ai_dev_model_fallback_init(UmiAiDevModelFallback *policy);
UmiStatus umi_ai_dev_model_fallback_configure(UmiAiDevModelFallback *policy, uint64_t allowed_flags, uint64_t denied_flags, UmiAiDevRisk maximum_risk, UmiAiDevLocality locality, int require_approval);
int umi_ai_dev_model_fallback_allows(const UmiAiDevModelFallback *policy, uint64_t requested_flags, UmiAiDevRisk risk, UmiAiDevLocality locality);
int umi_ai_dev_model_fallback_approval_required(const UmiAiDevModelFallback *policy, UmiAiDevRisk risk);

#ifdef __cplusplus
}
#endif

#endif
