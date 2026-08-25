/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/memory_item.h
 *
 * PURPOSE:
 *   Describe one durable AI workspace-memory item and its revision.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_MEMORY_ITEM_H
#define UMICOM_AI_DEVELOPER_PLATFORM_MEMORY_ITEM_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevMemoryItem {
    char id[UMI_AI_DEV_ID_CAPACITY];
    char label[UMI_AI_DEV_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t flags;
    uint32_t priority;
    int enabled;
} UmiAiDevMemoryItem;

void umi_ai_dev_memory_item_init(UmiAiDevMemoryItem *value);
UmiStatus umi_ai_dev_memory_item_configure(UmiAiDevMemoryItem *value, const char *id, const char *label, uint32_t priority, uint64_t flags);
UmiStatus umi_ai_dev_memory_item_validate(const UmiAiDevMemoryItem *value);
uint32_t umi_ai_dev_memory_item_evidence_score(const UmiAiDevMemoryItem *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
