/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/inline_candidate.h
 *
 * PURPOSE:
 *   Describe a candidate inline edit without applying it.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_INLINE_CANDIDATE_H
#define UMICOM_AI_DEVELOPER_PLATFORM_INLINE_CANDIDATE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevInlineCandidate {
    char id[UMI_AI_DEV_ID_CAPACITY];
    char label[UMI_AI_DEV_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t flags;
    uint32_t priority;
    int enabled;
} UmiAiDevInlineCandidate;

void umi_ai_dev_inline_candidate_init(UmiAiDevInlineCandidate *value);
UmiStatus umi_ai_dev_inline_candidate_configure(UmiAiDevInlineCandidate *value, const char *id, const char *label, uint32_t priority, uint64_t flags);
UmiStatus umi_ai_dev_inline_candidate_validate(const UmiAiDevInlineCandidate *value);
uint32_t umi_ai_dev_inline_candidate_evidence_score(const UmiAiDevInlineCandidate *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
