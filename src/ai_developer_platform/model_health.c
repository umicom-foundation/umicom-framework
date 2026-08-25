/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_platform/model_health.c
 *
 * PURPOSE:
 *   Represent model health and determine whether routing is safe.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/model_health.h"

#include <string.h>

void umi_ai_dev_model_health_init(UmiAiDevModelHealth *value) {
    if (value == NULL) {
        return;
    }
    memset(value, 0, sizeof(*value));
    value->state = UMI_AI_DEV_STATE_UNKNOWN;
}
UmiStatus umi_ai_dev_model_health_record(UmiAiDevModelHealth *value, int success) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (success) ++value->successes; else ++value->failures;
    ++value->revision;
    if (value->failures == 0U && value->successes > 0U) value->state = UMI_AI_DEV_STATE_READY;
    else if (value->successes > value->failures) value->state = UMI_AI_DEV_STATE_DEGRADED;
    else value->state = UMI_AI_DEV_STATE_BLOCKED;
    return UMI_STATUS_OK;
}
void umi_ai_dev_model_health_set_pending(UmiAiDevModelHealth *value, uint32_t pending) { if (value != NULL) { value->pending = pending; ++value->revision; } }
uint32_t umi_ai_dev_model_health_health_score(const UmiAiDevModelHealth *value) {
    uint32_t total;
    if (value == NULL) {
        return 0U;
    }
    total = value->successes + value->failures;
    if (total == 0U) {
        return 0U;
    }
    return (uint32_t)(((uint64_t)value->successes * UINT64_C(100)) / total);
}
int umi_ai_dev_model_health_ready(const UmiAiDevModelHealth *value) { return value != NULL && value->state == UMI_AI_DEV_STATE_READY && value->pending == 0U; }
