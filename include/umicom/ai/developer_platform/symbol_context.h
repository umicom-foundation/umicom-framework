/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/symbol_context.h
 *
 * PURPOSE:
 *   Describe symbol-level context independently of a particular language server.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_SYMBOL_CONTEXT_H
#define UMICOM_AI_DEVELOPER_PLATFORM_SYMBOL_CONTEXT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevSymbolContext {
    char id[UMI_AI_DEV_ID_CAPACITY];
    char label[UMI_AI_DEV_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t flags;
    uint32_t priority;
    int enabled;
} UmiAiDevSymbolContext;

void umi_ai_dev_symbol_context_init(UmiAiDevSymbolContext *value);
UmiStatus umi_ai_dev_symbol_context_configure(UmiAiDevSymbolContext *value, const char *id, const char *label, uint32_t priority, uint64_t flags);
UmiStatus umi_ai_dev_symbol_context_validate(const UmiAiDevSymbolContext *value);
uint32_t umi_ai_dev_symbol_context_evidence_score(const UmiAiDevSymbolContext *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
