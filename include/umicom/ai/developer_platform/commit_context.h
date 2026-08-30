/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/commit_context.h
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_COMMIT_CONTEXT_H
#define UMICOM_AI_DEVELOPER_PLATFORM_COMMIT_CONTEXT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevCommitContext {
    char id[UMI_AI_DEV_ID_CAPACITY];
    char label[UMI_AI_DEV_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t flags;
    uint32_t priority;
    int enabled;
} UmiAiDevCommitContext;

void umi_ai_dev_commit_context_init(UmiAiDevCommitContext *value);
UmiStatus umi_ai_dev_commit_context_configure(UmiAiDevCommitContext *value, const char *id, const char *label, uint32_t priority, uint64_t flags);
UmiStatus umi_ai_dev_commit_context_validate(const UmiAiDevCommitContext *value);
uint32_t umi_ai_dev_commit_context_evidence_score(const UmiAiDevCommitContext *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
