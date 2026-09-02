/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/model_selection.h
 *
 * PURPOSE:
 *   Rank eligible models deterministically for a requested developer operation.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_MODEL_SELECTION_H
#define UMICOM_AI_DEVELOPER_PLATFORM_MODEL_SELECTION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev model selection score data shared with callers of this public
 * contract.
 */
typedef struct UmiAiDevModelSelectionScore { uint32_t relevance; uint32_t authority; uint32_t freshness; uint32_t penalty; } UmiAiDevModelSelectionScore;
/**
 * Initialise ai dev model selection from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_model_selection_init(UmiAiDevModelSelectionScore *score);
/**
 * Provide the ai dev model selection compute operation used by this module and its client
 * applications.
 */
uint32_t umi_ai_dev_model_selection_compute(const UmiAiDevModelSelectionScore *score);
/**
 * Provide the ai dev model selection prefer operation used by this module and its client
 * applications.
 */
int umi_ai_dev_model_selection_prefer(const UmiAiDevModelSelectionScore *left, const UmiAiDevModelSelectionScore *right);
/**
 * Provide the ai dev model selection weighted operation used by this module and its client
 * applications.
 */
uint32_t umi_ai_dev_model_selection_weighted(uint32_t primary, uint32_t secondary, uint32_t primary_weight);

#ifdef __cplusplus
}
#endif

#endif
