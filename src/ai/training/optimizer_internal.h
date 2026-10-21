/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/optimizer_internal.h
 * PURPOSE: Internal identity check for the decoder-to-optimiser relationship.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_OPTIMIZER_INTERNAL_H
#define UMICOM_AI_TRAINING_OPTIMIZER_INTERNAL_H

#include "umicom/ai/training/optimizer.h"

UmiStatus UmiAiTrainingAdamWValidateBinding(
    const UmiAiTrainingAdamW *optimizer,
    const UmiAiTrainingTape *tape,
    UmiAiTrainingTensor *const *parameters,
    size_t count,
    uint64_t expectedSteps);


/* Restricted codec bridge: checkpointing reads immutable state, then may
 * restore moments only into a newly created decoder-bound optimiser. This
 * deliberately does not expose writable moment buffers to applications. */
UmiStatus UmiAiTrainingAdamWInternalInspect(
    const UmiAiTrainingAdamW *optimizer,
    UmiAiTrainingAdamWConfig *outConfig,
    uint64_t *outSteps, size_t *outParameterCount, size_t *outElements,
    const size_t **outCounts, const double **outFirst,
    const double **outSecond);
UmiStatus UmiAiTrainingAdamWInternalRestore(
    UmiAiTrainingAdamW *optimizer, const double *first,
    const double *second, size_t elementCount, uint64_t completedSteps);

/* Construct a resume-only optimiser on the decoder restored by the portable
 * decoder codec. The public fresh-start AdamWCreate restriction is retained. */
UmiStatus UmiAiDecoderAdamWRestoreCreate(
    UmiAiTrainingDecoder *model, const UmiAiTrainingAdamWConfig *config,
    UmiAiTrainingAdamW **outOptimizer);
UmiStatus UmiAiDecoderAdamWCheckBinding(
    const UmiAiTrainingDecoder *model, const UmiAiTrainingAdamW *optimizer);

#endif
