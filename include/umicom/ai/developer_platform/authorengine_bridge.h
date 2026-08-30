/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/authorengine_bridge.h
 *
 * PURPOSE:
 *   Bind AuthorEngine sessions/indexes/models to the Framework AI developer orchestration layer.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_AUTHORENGINE_BRIDGE_H
#define UMICOM_AI_DEVELOPER_PLATFORM_AUTHORENGINE_BRIDGE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevAuthorengineBridge {
    char id[UMI_AI_DEV_ID_CAPACITY];
    char label[UMI_AI_DEV_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t flags;
    uint32_t priority;
    int enabled;
} UmiAiDevAuthorengineBridge;

void umi_ai_dev_authorengine_bridge_init(UmiAiDevAuthorengineBridge *value);
UmiStatus umi_ai_dev_authorengine_bridge_configure(UmiAiDevAuthorengineBridge *value, const char *id, const char *label, uint32_t priority, uint64_t flags);
UmiStatus umi_ai_dev_authorengine_bridge_validate(const UmiAiDevAuthorengineBridge *value);
uint32_t umi_ai_dev_authorengine_bridge_evidence_score(const UmiAiDevAuthorengineBridge *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
