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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the ai dev memory item data shared with callers of this public contract.
 */
typedef struct UmiAiDevMemoryItem {
    char id[UMI_AI_DEV_ID_CAPACITY];
    char label[UMI_AI_DEV_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t flags;
    uint32_t priority;
    int enabled;
} UmiAiDevMemoryItem;

/**
 * Initialise ai dev memory item from caller-provided values so later operations receive a
 * known state.
 */
void umi_ai_dev_memory_item_init(UmiAiDevMemoryItem *value);
/**
 * Provide the ai dev memory item configure operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_memory_item_configure(UmiAiDevMemoryItem *value, const char *id, const char *label, uint32_t priority, uint64_t flags);
/**
 * Check that ai dev memory item satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ai_dev_memory_item_validate(const UmiAiDevMemoryItem *value);
/**
 * Provide the ai dev memory item evidence score operation used by this module and its
 * client applications.
 */
uint32_t umi_ai_dev_memory_item_evidence_score(const UmiAiDevMemoryItem *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
