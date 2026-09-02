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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/model_health.h"

#include <string.h>

/*
 * Initialise ai dev model health from caller-provided values so later operations receive a
 * known state.
 */
void umi_ai_dev_model_health_init(UmiAiDevModelHealth *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) {
        return;
    }
    memset(value, 0, sizeof(*value));
    value->state = UMI_AI_DEV_STATE_UNKNOWN;
}
/*
 * Provide the ai dev model health record operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_model_health_record(UmiAiDevModelHealth *value, int success) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (success) ++value->successes; /* Use this fallback path when the earlier condition does not apply. */ else ++value->failures;
    ++value->revision;
    /* Apply this operation only while the related capability or state is available. */
    if (value->failures == 0U && value->successes > 0U) value->state = UMI_AI_DEV_STATE_READY;
    else /* Apply this branch only when its contract condition is satisfied. */ if (value->successes > value->failures) value->state = UMI_AI_DEV_STATE_DEGRADED;
    /* Use this fallback path when the earlier condition does not apply. */
    else value->state = UMI_AI_DEV_STATE_BLOCKED;
    return UMI_STATUS_OK;
}
/*
 * Provide the ai dev model health set pending operation used by this module and its client
 * applications.
 */
void umi_ai_dev_model_health_set_pending(UmiAiDevModelHealth *value, uint32_t pending) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { value->pending = pending; ++value->revision; } }
/*
 * Provide the ai dev model health health score operation used by this module and its
 * client applications.
 */
uint32_t umi_ai_dev_model_health_health_score(const UmiAiDevModelHealth *value) {
    uint32_t total;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) {
        return 0U;
    }
    total = value->successes + value->failures;
    /* Apply this branch only when its contract condition is satisfied. */
    if (total == 0U) {
        return 0U;
    }
    return (uint32_t)(((uint64_t)value->successes * UINT64_C(100)) / total);
}
/*
 * Provide the ai dev model health ready operation used by this module and its client
 * applications.
 */
int umi_ai_dev_model_health_ready(const UmiAiDevModelHealth *value) { return value != NULL && value->state == UMI_AI_DEV_STATE_READY && value->pending == 0U; }
