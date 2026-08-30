/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/context_item.h
 *
 * PURPOSE:
 *   Represent one provenance-bearing context item selected for a prompt.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_ITEM_H
#define UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_ITEM_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevContextItem {
    char id[UMI_AI_DEV_ID_CAPACITY];
    char label[UMI_AI_DEV_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t flags;
    uint32_t priority;
    int enabled;
} UmiAiDevContextItem;

void umi_ai_dev_context_item_init(UmiAiDevContextItem *value);
UmiStatus umi_ai_dev_context_item_configure(UmiAiDevContextItem *value, const char *id, const char *label, uint32_t priority, uint64_t flags);
UmiStatus umi_ai_dev_context_item_validate(const UmiAiDevContextItem *value);
uint32_t umi_ai_dev_context_item_evidence_score(const UmiAiDevContextItem *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
