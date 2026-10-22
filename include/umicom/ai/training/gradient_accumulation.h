/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/gradient_accumulation.h
 *
 * PURPOSE:
 *   Train the established native decoder with an effective token batch larger
 *   than one bounded training-tape graph, without changing its mathematics or
 *   allocating an unbounded intermediate graph.
 *
 * ARCHITECTURE AND OWNERSHIP:
 *   The existing decoder builds every forward and backward graph. The tape
 *   remains the sole authority for atomic SGD parameter updates. Gradients
 *   are reduced in caller-independent, temporary double-precision storage;
 *   microbatch graphs are rewound after each backward pass. Parameters are
 *   updated once per effective batch, NOT once per microbatch.
 *
 *   Calls are synchronous; callers serialise access to decoders and tapes.
 *   These are research/training facilities, not production financial advice.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_GRADIENT_ACCUMULATION_H
#define UMICOM_AI_TRAINING_GRADIENT_ACCUMULATION_H

#include <stddef.h>
#include "umicom/ai/training/decoder.h"
#include "umicom/ai/training/tape.h"
#include "umicom/ai/training/optimizer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Upper bound on the TOTAL token count of an accumulated training step.
 * Individual microbatch graphs retain the normal decoder/tape shape limits.
 * This limit guards CPU work and all caller-controlled size calculations. */
#define UMI_AI_TRAINING_MAX_ACCUMULATED_TOKENS 65536U

/* Apply externally aggregated finite gradients to distinct trainable leaves
 * owned by this tape. Gradient array lengths must match the corresponding
 * tensor shapes exactly; the caller owns each array during this call.
 *
 * All handles, duplicate identities, gradient values, and candidate updated
 * FP32 weights are verified BEFORE the first parameter changes. This update
 * deliberately does not require a preceding tape Backward: it supports
 * aggregation from multiple separately rewound graphs.
 *
 * Callers must construct gradients from trusted sources and choose a valid
 * positive finite learning rate. This function does not increment a model's
 * optimiser-step counter or alter the graph's lifetime. */
UmiStatus UmiAiTrainingSgdStepWithGradients(
    UmiAiTrainingTape *tape,
    UmiAiTrainingTensor *const *parameters,
    const float *const *gradients,
    size_t parameterCount,
    float learningRate);

/* Perform ONE optimiser step on `tokenCount` aligned next-token examples.
 * `microbatchSequences` is the maximum number of complete context-length
 * sequences processed by any single tape graph. The final microbatch may
 * contain fewer sequences. All input and target IDs must be valid.
 *
 * Each microbatch produces a MEAN loss and mean parameter gradients. Both
 * are weighted by the number of token positions in that microbatch so the
 * effective step optimises the mean over ALL supplied tokens. The graph is
 * rewound after every microbatch. The result is mathematically equivalent to
 * one large batch, apart from ordinary FP32 summation and operation ordering.
 *
 * On failure, trained weights, step count, and *outLoss are unchanged.
 * Temporary graphs are released before returning. No checkpoint file or
 * external dataset is changed. This API preserves the established TrainStep
 * and portable checkpoint format. */
UmiStatus UmiAiTrainingDecoderTrainAccumulated(
    UmiAiTrainingDecoder *model,
    const size_t *inputIds,
    const size_t *targetIds,
    size_t tokenCount,
    size_t microbatchSequences,
    float learningRate,
    float *outLoss);

/* Train one effective batch with AdamW using the Framework's canonical
 * token-weighted microbatch gradient reducer. A single AdamW update changes
 * both model parameters and first/second moment state. On failure the model's
 * parameters, optimiser state, completed steps and *outLoss are preserved.
 * The AdamW optimiser must be bound to this decoder and outlive the call.
 * Combined decoder/AdamW checkpoints remain compatible and permit exact
 * continuation under an unchanged dataset, schedule and microbatch policy. */
UmiStatus UmiAiTrainingDecoderTrainAdamWAccumulated(
    UmiAiTrainingDecoder *model,
    struct UmiAiTrainingAdamW *optimizer,
    const size_t *inputIds,
    const size_t *targetIds,
    size_t tokenCount,
    size_t microbatchSequences,
    float learningRate,
    float *outLoss);

/* Apply one global norm threshold AFTER all microbatch gradients have been
 * combined. Each microbatch still uses the canonical decoder's bounded tape.
 * On failure the optimiser moments, model weights, step count, loss and
 * clipping report remain unchanged. A resumed experiment must retain the same
 * clipping threshold alongside its approved dataset and schedule. */
UmiStatus UmiAiTrainingDecoderTrainAdamWAccumulatedClipped(
    UmiAiTrainingDecoder *model,
    struct UmiAiTrainingAdamW *optimizer,
    const size_t *inputIds, const size_t *targetIds,
    size_t tokenCount, size_t microbatchSequences,
    float learningRate, float maximumGradientNorm,
    float *outLoss, UmiAiTrainingGradientClipReport *outReport);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_GRADIENT_ACCUMULATION_H */
