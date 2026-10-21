/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/initialisation.h
 *
 * PURPOSE:
 *   Reproducible, local, seeded parameter initialisation for native AI lessons.
 *
 * ARCHITECTURE:
 *   No global random state, scripting runtime or platform RNG dependencies.
 *   Each caller owns its RNG state; a fixed seed produces identical values
 *   on conforming C23 implementations. Not suitable for cryptography.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_INITIALISATION_H
#define UMICOM_AI_TRAINING_INITIALISATION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingRandomState {
    uint64_t state;
} UmiAiTrainingRandomState;

/* All seeds (including zero) are valid. The caller owns the state. */
UmiStatus UmiAiTrainingRandomSeed(UmiAiTrainingRandomState *randomState,
                                 uint64_t seed);

/* Sample a finite FP32 value from a nominal half-open [minimum, maximum)
 * interval. Floating-point rounding can produce the upper endpoint.
 * Invalid requests leave both the state and output unchanged. */
UmiStatus UmiAiTrainingRandomUniform(UmiAiTrainingRandomState *randomState,
                                    float minimum, float maximum,
                                    float *outValue);

/* Atomically fill caller-owned storage with reproducible values.
 * count=0 succeeds without requiring values. At most UMI_AI_TRAINING_MAX_ELEMENTS values are accepted
 * in this teaching implementation (see tape.h). If validation or allocation fails, neither
 * the state nor any caller-provided output value is modified. */
UmiStatus UmiAiTrainingRandomFillUniform(UmiAiTrainingRandomState *randomState,
                                        float minimum, float maximum,
                                        float *values, size_t count);

/* Deterministic, unbiased integer sample in [0, exclusiveMaximum). Samples
 * are calculated with a local copy of the RNG state and published only on
 * success. Used by Framework dataset batching without float rounding or
 * application-local random-number implementations. NOT cryptographic. */
UmiStatus UmiAiTrainingRandomIndex(UmiAiTrainingRandomState *randomState,
                                  uint64_t exclusiveMaximum,
                                  uint64_t *outIndex);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_INITIALISATION_H */
