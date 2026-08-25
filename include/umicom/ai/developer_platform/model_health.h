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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiAiDevModelHealth {
    UmiAiDevState state;
    uint32_t successes;
    uint32_t failures;
    uint32_t pending;
    uint64_t revision;
} UmiAiDevModelHealth;

void umi_ai_dev_model_health_init(UmiAiDevModelHealth *value);
UmiStatus umi_ai_dev_model_health_record(UmiAiDevModelHealth *value, int success);
void umi_ai_dev_model_health_set_pending(UmiAiDevModelHealth *value, uint32_t pending);
uint32_t umi_ai_dev_model_health_health_score(const UmiAiDevModelHealth *value);
int umi_ai_dev_model_health_ready(const UmiAiDevModelHealth *value);

#ifdef __cplusplus
}
#endif

#endif
