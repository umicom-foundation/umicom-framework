/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/model.h
 *
 * PURPOSE:
 *   Describe one AI model independently of the provider that hosts it.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A model record stores capabilities and limits, while provider-specific loading details remain private to adapters.
 */

#ifndef INCLUDE_UMICOM_AI_MODEL_H
#define INCLUDE_UMICOM_AI_MODEL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai model data shared with callers of this public contract.
 */
typedef struct UmiAiModel {
    char model_id[UMI_AI_ID_CAPACITY];
    char display_name[UMI_AI_SMALL_TEXT_CAPACITY];
    uint32_t context_tokens;
    uint32_t max_output_tokens;
    int supports_tools;
    int supports_embeddings;
} UmiAiModel;

/**
 * Initialise ai model from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ai_model_init(UmiAiModel *value);

#ifdef __cplusplus
}
#endif

#endif
