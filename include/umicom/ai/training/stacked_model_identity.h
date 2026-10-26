/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/stacked_model_identity.h
 * PURPOSE: Bind native stacked-model operations to their exact checkpoint bytes.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_STACKED_MODEL_IDENTITY_H
#define UMICOM_AI_TRAINING_STACKED_MODEL_IDENTITY_H

#include "umicom/ai/training/stacked_checkpoint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingStackedModelIdentity {
    UmiAiTrainingStackedDecoderConfig config;
    uint64_t completedTrainingSteps;
    size_t checkpointBytes;
    uint8_t checkpointSha256[UMI_SHA256_BYTES];
} UmiAiTrainingStackedModelIdentity;

/* Capture the current canonical, separately versioned stacked checkpoint.
 * Nothing depends on C struct padding, native endianness or a caller-provided
 * model label. The hash detects content changes but DOES NOT authenticate its
 * publisher. Caller serialises model access. No files or network requests. */
UmiStatus UmiAiTrainingStackedModelIdentityCapture(
    const UmiAiTrainingStackedDecoder *model,
    UmiAiTrainingStackedModelIdentity *outIdentity);

/* Refuse a modified/replaced model even if its dimensions and completed-step
 * count happen to match. Rejection does not change any caller-owned output. */
UmiStatus UmiAiTrainingStackedModelIdentityVerify(
    const UmiAiTrainingStackedDecoder *model,
    const UmiAiTrainingStackedModelIdentity *expected);

#ifdef __cplusplus
}
#endif
#endif
