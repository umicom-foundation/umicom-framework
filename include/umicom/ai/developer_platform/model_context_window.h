/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/model_context_window.h
 *
 * PURPOSE:
 *   Reserve context-window capacity for instructions, evidence and response tokens.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_MODEL_CONTEXT_WINDOW_H
#define UMICOM_AI_DEVELOPER_PLATFORM_MODEL_CONTEXT_WINDOW_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevModelContextWindow { uint64_t limit; uint64_t reserved; uint64_t consumed; uint64_t revision; } UmiAiDevModelContextWindow;
void umi_ai_dev_model_context_window_init(UmiAiDevModelContextWindow *budget, uint64_t limit);
UmiStatus umi_ai_dev_model_context_window_reserve(UmiAiDevModelContextWindow *budget, uint64_t amount);
UmiStatus umi_ai_dev_model_context_window_consume(UmiAiDevModelContextWindow *budget, uint64_t amount);
uint64_t umi_ai_dev_model_context_window_remaining(const UmiAiDevModelContextWindow *budget);
uint32_t umi_ai_dev_model_context_window_utilisation(const UmiAiDevModelContextWindow *budget);

#ifdef __cplusplus
}
#endif

#endif
