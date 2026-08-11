/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/model.h
 *
 * PURPOSE:
 *   Describe one AI model independently of the provider that hosts it.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

typedef struct UmiAiModel {
    char model_id[UMI_AI_ID_CAPACITY];
    char display_name[UMI_AI_SMALL_TEXT_CAPACITY];
    uint32_t context_tokens;
    uint32_t max_output_tokens;
    int supports_tools;
    int supports_embeddings;
} UmiAiModel;

UmiStatus umi_ai_model_init(UmiAiModel *value);

#ifdef __cplusplus
}
#endif

#endif
