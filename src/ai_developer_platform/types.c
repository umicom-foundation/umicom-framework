/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_platform/types.c
 *
 * PURPOSE:
 *   Define shared AI developer platform constants, states, locality and risk classifications.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/types.h"

const char *umi_ai_dev_state_text(UmiAiDevState state) {
    switch (state) {
        case UMI_AI_DEV_STATE_READY: return "ready";
        case UMI_AI_DEV_STATE_DEGRADED: return "degraded";
        case UMI_AI_DEV_STATE_BLOCKED: return "blocked";
        case UMI_AI_DEV_STATE_FAILED: return "failed";
        case UMI_AI_DEV_STATE_UNKNOWN:
        default: return "unknown";
    }
}
const char *umi_ai_dev_locality_text(UmiAiDevLocality locality) {
    switch (locality) {
        case UMI_AI_DEV_LOCALITY_LOCAL: return "local";
        case UMI_AI_DEV_LOCALITY_REMOTE: return "remote";
        case UMI_AI_DEV_LOCALITY_ANY:
        default: return "any";
    }
}
const char *umi_ai_dev_risk_text(UmiAiDevRisk risk) {
    switch (risk) {
        case UMI_AI_DEV_RISK_LOW: return "low";
        case UMI_AI_DEV_RISK_MEDIUM: return "medium";
        case UMI_AI_DEV_RISK_HIGH: return "high";
        case UMI_AI_DEV_RISK_CRITICAL: return "critical";
        case UMI_AI_DEV_RISK_NONE:
        default: return "none";
    }
}
uint32_t umi_ai_dev_percent(uint64_t value, uint64_t maximum) {
    if (maximum == 0U) return 0U;
    if (value >= maximum) return 100U;
    return (uint32_t)((value * UINT64_C(100)) / maximum);
}
