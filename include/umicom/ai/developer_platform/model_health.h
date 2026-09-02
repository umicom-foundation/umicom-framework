/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/model_health.h
 *
 * PURPOSE:
 *   Represent model health and determine whether routing is safe.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_MODEL_HEALTH_H
#define UMICOM_AI_DEVELOPER_PLATFORM_MODEL_HEALTH_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev model health data shared with callers of this public contract.
 */
typedef struct UmiAiDevModelHealth {
    UmiAiDevState state;
    uint32_t successes;
    uint32_t failures;
    uint32_t pending;
    uint64_t revision;
} UmiAiDevModelHealth;

/**
 * Initialise ai dev model health from caller-provided values so later operations receive a
 * known state.
 */
void umi_ai_dev_model_health_init(UmiAiDevModelHealth *value);
/**
 * Provide the ai dev model health record operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_model_health_record(UmiAiDevModelHealth *value, int success);
/**
 * Provide the ai dev model health set pending operation used by this module and its client
 * applications.
 */
void umi_ai_dev_model_health_set_pending(UmiAiDevModelHealth *value, uint32_t pending);
/**
 * Provide the ai dev model health health score operation used by this module and its
 * client applications.
 */
uint32_t umi_ai_dev_model_health_health_score(const UmiAiDevModelHealth *value);
/**
 * Provide the ai dev model health ready operation used by this module and its client
 * applications.
 */
int umi_ai_dev_model_health_ready(const UmiAiDevModelHealth *value);

#ifdef __cplusplus
}
#endif

#endif
