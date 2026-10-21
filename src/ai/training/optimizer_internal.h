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

#endif
