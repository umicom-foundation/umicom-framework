/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/model_latency.h
 *
 * PURPOSE:
 *   Enforce latency service-level budgets for interactive AI operations.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_MODEL_LATENCY_H
#define UMICOM_AI_DEVELOPER_PLATFORM_MODEL_LATENCY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev model latency data shared with callers of this public contract.
 */
typedef struct UmiAiDevModelLatency { uint64_t limit; uint64_t reserved; uint64_t consumed; uint64_t revision; } UmiAiDevModelLatency;
/**
 * Initialise ai dev model latency from caller-provided values so later operations receive
 * a known state.
 */
void umi_ai_dev_model_latency_init(UmiAiDevModelLatency *budget, uint64_t limit);
/**
 * Provide the ai dev model latency reserve operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_model_latency_reserve(UmiAiDevModelLatency *budget, uint64_t amount);
/**
 * Provide the ai dev model latency consume operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_model_latency_consume(UmiAiDevModelLatency *budget, uint64_t amount);
/**
 * Provide the ai dev model latency remaining operation used by this module and its client
 * applications.
 */
uint64_t umi_ai_dev_model_latency_remaining(const UmiAiDevModelLatency *budget);
/**
 * Provide the ai dev model latency utilisation operation used by this module and its
 * client applications.
 */
uint32_t umi_ai_dev_model_latency_utilisation(const UmiAiDevModelLatency *budget);

#ifdef __cplusplus
}
#endif

#endif
