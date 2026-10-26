/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/stacked_decoder_internal.h
 * PURPOSE: Private parameter order shared by the new model and its codec.
 * This header is intentionally not installed or exposed to applications.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STACKED_DECODER_INTERNAL_H
#define UMICOM_STACKED_DECODER_INTERNAL_H
#include "umicom/ai/training/stacked_decoder.h"

/* First: token/position embedding; then 12 tensors for EACH layer in the
 * public TransformerBlockParameters field order; final: logits projection.
 * This is a NEW checkpoint family, never the legacy decoder parameter IDs. */
enum {
    UMI_STACKED_TOKEN_EMBEDDING = 0U,
    UMI_STACKED_POSITION_EMBEDDING = 1U,
    UMI_STACKED_LAYER_START = 2U
};
enum {
    UMI_STACKED_NORM1_SCALE = 0U, UMI_STACKED_NORM1_BIAS,
    UMI_STACKED_QUERY, UMI_STACKED_KEY, UMI_STACKED_VALUE,
    UMI_STACKED_ATTENTION_OUTPUT, UMI_STACKED_NORM2_SCALE,
    UMI_STACKED_NORM2_BIAS, UMI_STACKED_FEED_FORWARD_UP,
    UMI_STACKED_FEED_FORWARD_UP_BIAS, UMI_STACKED_FEED_FORWARD_DOWN,
    UMI_STACKED_FEED_FORWARD_DOWN_BIAS
};

struct UmiAiTrainingStackedDecoder {
    UmiAiTrainingStackedDecoderConfig config;
    UmiAiTrainingTape *tape;
    UmiAiTrainingTensor *parameters[UMI_AI_STACKED_MAX_PARAMETERS];
    size_t parameterCount;
    size_t parameterElements;
    uint64_t completedSteps;
};

UmiStatus UmiAiStackedValidateConfig(
    const UmiAiTrainingStackedDecoderConfig *config);
void UmiAiStackedParameterShape(
    const UmiAiTrainingStackedDecoderConfig *config, size_t index,
    size_t *outRows, size_t *outColumns);
/* valuePointers may be NULL for seeded initialisation. Otherwise every
 * parameter has its exact row-major finite FP32 array. */
UmiStatus UmiAiStackedCreateFromValues(
    const UmiAiTrainingStackedDecoderConfig *config,
    const float *const *valuePointers, uint64_t completedSteps,
    UmiAiTrainingStackedDecoder **outModel);
#endif
