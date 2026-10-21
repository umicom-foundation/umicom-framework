/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/decoder_private.h
 * PURPOSE:
 *   Internal, versioned parameter ordering shared ONLY by model construction
 *   and checkpoint encoding. This is deliberately not part of the public ABI.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_DECODER_PRIVATE_H
#define UMICOM_AI_TRAINING_DECODER_PRIVATE_H
#include "umicom/ai/training/decoder.h"
#include "umicom/ai/training/tape.h"
#include <stddef.h>
#include <stdint.h>

enum UmiAiDecoderParameterId {
    UMI_DECODER_TOKEN_EMBEDDING = 0,
    UMI_DECODER_POSITION_EMBEDDING,
    UMI_DECODER_NORM1_SCALE,
    UMI_DECODER_NORM1_BIAS,
    UMI_DECODER_QUERY,
    UMI_DECODER_KEY,
    UMI_DECODER_VALUE,
    UMI_DECODER_PROJECTION,
    UMI_DECODER_NORM2_SCALE,
    UMI_DECODER_NORM2_BIAS,
    UMI_DECODER_UP,
    UMI_DECODER_UP_BIAS,
    UMI_DECODER_DOWN,
    UMI_DECODER_DOWN_BIAS,
    UMI_DECODER_LOGITS,
    UMI_DECODER_PARAMETER_COUNT
};

struct UmiAiTrainingDecoder {
    UmiAiTrainingDecoderConfig config;
    UmiAiTrainingTape *tape;
    UmiAiTrainingTensor *parameters[UMI_DECODER_PARAMETER_COUNT];
    uint64_t completedSteps;
};

/* All private helpers validate arguments and leave output untouched on error. */
UmiStatus UmiAiDecoderValidateConfig(const UmiAiTrainingDecoderConfig *config);
void UmiAiDecoderParameterShape(const UmiAiTrainingDecoderConfig *config,
                               size_t parameterId, size_t *rows,
                               size_t *columns);
/* values=NULL means generate all weights from initialSeed. Otherwise array
 * entries must point to finite float arrays sized by ParameterShape. */
UmiStatus UmiAiDecoderCreateFromValues(const UmiAiTrainingDecoderConfig *config,
                                      const float *const *values,
                                      uint64_t completedSteps,
                                      UmiAiTrainingDecoder **outModel);
#endif /* UMICOM_AI_TRAINING_DECODER_PRIVATE_H */
