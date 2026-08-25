/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/context_source.h
 *
 * PURPOSE:
 *   Describe a source of developer context such as repository files, diagnostics, history or symbols.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_SOURCE_H
#define UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_SOURCE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevContextSource {
    char id[UMI_AI_DEV_ID_CAPACITY];
    char label[UMI_AI_DEV_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t flags;
    uint32_t priority;
    int enabled;
} UmiAiDevContextSource;

void umi_ai_dev_context_source_init(UmiAiDevContextSource *value);
UmiStatus umi_ai_dev_context_source_configure(UmiAiDevContextSource *value, const char *id, const char *label, uint32_t priority, uint64_t flags);
UmiStatus umi_ai_dev_context_source_validate(const UmiAiDevContextSource *value);
uint32_t umi_ai_dev_context_source_evidence_score(const UmiAiDevContextSource *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
