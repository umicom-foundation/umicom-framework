/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_platform/memory_item.c
 *
 * PURPOSE:
 *   Describe one durable AI workspace-memory item and its revision.
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
#include "umicom/ai/developer_platform/memory_item.h"

#include <string.h>

/*
 * Copy ai dev memory item into module-owned storage so callers keep ownership of their
 * input values.
 */
static void umi_ai_dev_memory_item_copy(char *dst, size_t cap, const char *src) {
    size_t i = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (dst == NULL || cap == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (src != NULL) {
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (i + 1U < cap && src[i] != '\0') { dst[i] = src[i]; ++i; }
    }
    dst[i] = '\0';
}

/*
 * Initialise ai dev memory item from caller-provided values so later operations receive a
 * known state.
 */
void umi_ai_dev_memory_item_init(UmiAiDevMemoryItem *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->enabled = 1;
}
/*
 * Provide the ai dev memory item configure operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_memory_item_configure(UmiAiDevMemoryItem *value, const char *id, const char *label, uint32_t priority, uint64_t flags) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || id == NULL || id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    umi_ai_dev_memory_item_init(value);
    umi_ai_dev_memory_item_copy(value->id, sizeof(value->id), id);
    umi_ai_dev_memory_item_copy(value->label, sizeof(value->label), label);
    value->priority = priority; value->flags = flags; value->revision = 1U;
    return UMI_STATUS_OK;
}
/*
 * Check that ai dev memory item satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ai_dev_memory_item_validate(const UmiAiDevMemoryItem *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    return value->enabled ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
}
/*
 * Provide the ai dev memory item evidence score operation used by this module and its
 * client applications.
 */
uint32_t umi_ai_dev_memory_item_evidence_score(const UmiAiDevMemoryItem *value, uint32_t relevance) {
    uint32_t bonus;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || !value->enabled) return 0U;
    bonus = value->priority > 20U ? 20U : value->priority;
    relevance = relevance > 80U ? 80U : relevance;
    return relevance + bonus;
}
