/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_platform/commit_context.c
 *
 * PURPOSE:
 *   Describe source-control commit evidence supplied to AI reasoning.
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
#include "umicom/ai/developer_platform/commit_context.h"

#include <string.h>

static void umi_ai_dev_commit_context_copy(char *dst, size_t cap, const char *src) {
    size_t i = 0U;
    if (dst == NULL || cap == 0U) return;
    if (src != NULL) {
        while (i + 1U < cap && src[i] != '\0') { dst[i] = src[i]; ++i; }
    }
    dst[i] = '\0';
}

void umi_ai_dev_commit_context_init(UmiAiDevCommitContext *value) {
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->enabled = 1;
}
UmiStatus umi_ai_dev_commit_context_configure(UmiAiDevCommitContext *value, const char *id, const char *label, uint32_t priority, uint64_t flags) {
    if (value == NULL || id == NULL || id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    umi_ai_dev_commit_context_init(value);
    umi_ai_dev_commit_context_copy(value->id, sizeof(value->id), id);
    umi_ai_dev_commit_context_copy(value->label, sizeof(value->label), label);
    value->priority = priority; value->flags = flags; value->revision = 1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_ai_dev_commit_context_validate(const UmiAiDevCommitContext *value) {
    if (value == NULL || value->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    return value->enabled ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
}
uint32_t umi_ai_dev_commit_context_evidence_score(const UmiAiDevCommitContext *value, uint32_t relevance) {
    uint32_t bonus;
    if (value == NULL || !value->enabled) return 0U;
    bonus = value->priority > 20U ? 20U : value->priority;
    relevance = relevance > 80U ? 80U : relevance;
    return relevance + bonus;
}
