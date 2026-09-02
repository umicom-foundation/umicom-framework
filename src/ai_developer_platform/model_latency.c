/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_platform/model_latency.c
 *
 * PURPOSE:
 *   Enforce latency service-level budgets for interactive AI operations.
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
#include "umicom/ai/developer_platform/model_latency.h"
/*
 * Initialise ai dev model latency from caller-provided values so later operations receive
 * a known state.
 */
void umi_ai_dev_model_latency_init(UmiAiDevModelLatency *budget, uint64_t limit) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (budget == NULL) {
        return;
    }
    budget->limit = limit;
    budget->reserved = 0U;
    budget->consumed = 0U;
    budget->revision = 1U;
}
/*
 * Provide the ai dev model latency remaining operation used by this module and its client
 * applications.
 */
uint64_t umi_ai_dev_model_latency_remaining(const UmiAiDevModelLatency *budget) {
    uint64_t used;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (budget == NULL) {
        return 0U;
    }
    used = budget->reserved + budget->consumed;
    return used >= budget->limit ? 0U : budget->limit - used;
}
/*
 * Provide the ai dev model latency reserve operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_model_latency_reserve(UmiAiDevModelLatency *budget, uint64_t amount) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (budget == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (amount > umi_ai_dev_model_latency_remaining(budget)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    budget->reserved += amount;
    ++budget->revision;
    return UMI_STATUS_OK;
}
/*
 * Provide the ai dev model latency consume operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_model_latency_consume(UmiAiDevModelLatency *budget, uint64_t amount) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (budget == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (amount > budget->reserved + umi_ai_dev_model_latency_remaining(budget)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (amount <= budget->reserved) {
        budget->reserved -= amount;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        budget->reserved = 0U;
    }
    budget->consumed += amount;
    ++budget->revision;
    return UMI_STATUS_OK;
}
/*
 * Provide the ai dev model latency utilisation operation used by this module and its
 * client applications.
 */
uint32_t umi_ai_dev_model_latency_utilisation(const UmiAiDevModelLatency *budget) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (budget == NULL) {
        return 0U;
    }
    return umi_ai_dev_percent(budget->consumed + budget->reserved, budget->limit);
}
