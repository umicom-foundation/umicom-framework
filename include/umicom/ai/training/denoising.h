/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/denoising.h
 *
 * PURPOSE:
 *   Define bounded forward noising and epsilon-based reconstruction for model-specific native consumers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_TRAINING_DENOISING_H
#define UMICOM_AI_TRAINING_DENOISING_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_AI_DENOISING_MAX_VALUES 16384U

/* Mix finite scalar values: noisy = sqrt(alphaBar)*clean
 *                                  + sqrt(1-alphaBar)*noise.
 * alphaBar is the cumulative signal coefficient in [0,1], NOT beta, a step
 * number or an automatically chosen schedule. The caller supplies noise;
 * this function neither samples a distribution nor creates a model.
 *
 * Counts/capacities are floats, not bytes; require 1..MAX_VALUES values.
 * Input spans and output must be readable/writable for their lengths and
 * disjoint. Callers serialize access and keep input values unchanged during
 * the call. All output values remain unchanged on any failure.
 *
 * No allocation or I/O. No clipping, color conversion, pixel normalization,
 * latent scaling or image decoding. A model-specific caller owns those rules.
 */
UmiStatus UmiAiDenoisingMix(const float *clean, const float *noise,
    size_t count, float alphaBar, float *outNoisy, size_t capacity);

/* Recover the clean estimate from an epsilon-predicting model:
 * cleanEstimate = (noisy - sqrt(1-alphaBar)*predictedNoise)/sqrt(alphaBar).
 * Require 0 < alphaBar <= 1; zero loses all clean signal and is not invertible.
 * The prediction may be wrong. Successful arithmetic does not prove image
 * quality, distribution correctness or compatibility with v/x0 prediction.
 * Finite-input, bound, disjointness and transactional-output rules above apply.
 */
UmiStatus UmiAiDenoisingRecover(const float *noisy, const float *predictedNoise,
    size_t count, float alphaBar, float *outClean, size_t capacity);

#define UMI_AI_DENOISING_MAX_STEPS 4096U

/* Convert a caller-selected per-step beta schedule into cumulative alphaBar:
 * outAlphaBars[i] = product(1 - betas[j]), j=0..i. The implicit value BEFORE
 * step zero is 1; output element zero is AFTER applying betas[0]. Each beta
 * must be finite and in [0,1). There is no automatic linear/cosine schedule.
 *
 * Require 1..MAX_STEPS elements. Capacity is measured in floats. Reject a
 * cumulative product smaller than FLT_MIN (the minimum positive normal float)
 * rather than publishing zero or depending on subnormal hardware behavior.
 * Such a schedule needs a different reviewed representation; it is not clipped.
 *
 * Input and output are disjoint caller-owned spans and remain unchanged by
 * other threads for the call. All output, including its unused tail, remains
 * unchanged on failure. No allocation, RNG, file I/O or model execution occurs.
 * This supplies coefficients to Mix/Recover; it is not a reverse sampler or
 * a claim that the coefficients match a pretrained model's training schedule.
 */
UmiStatus UmiAiDenoisingScheduleFromBetas(const float *betas, size_t count,
    float *outAlphaBars, size_t capacity);
/* Classifier-free guidance for two compatible epsilon predictions:
 * guided = unconditioned + scale * (conditioned - unconditioned).
 * Both arrays must describe the SAME noisy sample, timestep, shape, element
 * order and epsilon parameterization. This function cannot verify that model
 * contract. A negative-prompt prediction is not automatically unconditioned.
 *
 * scale must be finite and nonnegative. Zero copies unconditioned exactly;
 * one copies conditioned exactly. Values above one extrapolate; output is not
 * clipped, normalized or guaranteed to improve quality. All values in BOTH
 * inputs must be finite even at scale zero/one. Invalid or nonrepresentable
 * results leave the complete output unchanged. Unused capacity is untouched.
 *
 * Counts/capacities are floats; the existing 1..MAX_VALUES bound applies.
 * Inputs may alias each other; output must be disjoint from both. All spans
 * remain stable and caller-owned during the call. No allocation, model invocation,
 * scheduling, tokenization, random sampling or image/video I/O occurs.
 */
UmiStatus UmiAiDenoisingGuideNoise(const float *unconditioned,
    const float *conditioned, size_t count, float scale,
    float *outGuided, size_t capacity);

/* Deterministic DDIM step (eta = 0) for an epsilon-predicting model:
 * x0 = (noisy - sqrt(1-currentAlphaBar)*predictedNoise)/sqrt(currentAlphaBar)
 * previous = sqrt(previousAlphaBar)*x0 + sqrt(1-previousAlphaBar)*predictedNoise.
 * Require 0 < currentAlphaBar <= previousAlphaBar <= 1. "Previous" means
 * an earlier, no-less-signal point in the forward schedule, not an array index.
 * Both coefficients must belong to the caller's model-compatible schedule.
 * Equal coefficients copy noisy exactly; previousAlphaBar=1 recovers x0.
 *
 * Inputs describe the same sample, timestep, shape and epsilon convention.
 * No stochastic noise, clipping, thresholding, model invocation, schedule
 * selection or image/video conversion occurs. This is one numerical step,
 * not a complete sampler or a guarantee of quality. Caller-owned output is
 * disjoint from both stable inputs. Inputs may alias each other. All input
 * values must be finite even at the identity endpoint. Counts/capacities are
 * floats with the existing 1..MAX_VALUES bound. Nonfinite or unrepresentable
 * output refuses the whole operation; unused capacity is never written.
 * Double intermediates avoid an allocated x0 scratch array. No allocation/I/O.
 */
UmiStatus UmiAiDenoisingDeterministicStep(const float *noisy,
    const float *predictedNoise, size_t count, float currentAlphaBar,
    float previousAlphaBar, float *outPrevious, size_t capacity);


/* Convert a variance-preserving diffusion velocity prediction:
 * v = sqrt(alphaBar)*epsilon - sqrt(1-alphaBar)*clean,
 * noisy = sqrt(alphaBar)*clean + sqrt(1-alphaBar)*epsilon.
 * Then epsilon = sqrt(1-alphaBar)*noisy + sqrt(alphaBar)*v,
 * and clean = sqrt(alphaBar)*noisy - sqrt(1-alphaBar)*v.
 *
 * Require finite alphaBar in [0,1]. These rotations contain no division, so
 * both endpoints are defined: alphaBar=0 gives epsilon=noisy and clean=-v;
 * alphaBar=1 gives epsilon=v and clean=noisy. The model must use THIS v
 * convention and this sample's timestep, scaling, shape and element order.
 * A flow-matching model's field also called velocity is not automatically
 * compatible. These functions do not infer or validate a model convention.
 *
 * The prediction can be wrong. Arithmetic success proves neither recovery
 * accuracy nor model quality. Both input arrays must be finite even at an
 * endpoint that does not use one array. Counts/capacities are floats with the
 * existing 1..MAX_VALUES bound. Inputs may alias each other; output is disjoint
 * from both stable caller-owned inputs. No allocation or I/O. Any failure
 * leaves the complete output unchanged, and unused capacity is untouched.
 */
UmiStatus UmiAiDenoisingVelocityToNoise(const float *noisy,
    const float *predictedVelocity, size_t count, float alphaBar,
    float *outNoise, size_t capacity);
UmiStatus UmiAiDenoisingVelocityToClean(const float *noisy,
    const float *predictedVelocity, size_t count, float alphaBar,
    float *outClean, size_t capacity);

/* One stochastic DDIM step for the same epsilon convention as the deterministic
 * service. Let a=currentAlphaBar, p=previousAlphaBar, and supplied z be fresh
 * independent unit-normal noise for this transition:
 * sigma = eta * sqrt((1-p)/(1-a) * (1-a/p));
 * previous = sqrt(p)*x0 + sqrt(1-p-sigma*sigma)*predictedNoise + sigma*z.
 * x0 is recovered using the existing epsilon relation. Require 0<a<=p<=1
 * and finite eta in [0,1]. eta=0, a=p, or p=1 delegates to the deterministic
 * service after checking ALL supplied noise values. These endpoints consume
 * no random state here; this API never generates noise or owns an RNG.
 *
 * The caller supplies a compatible schedule and fresh noise distribution;
 * finite arrays alone do not establish statistical independence. Inputs may
 * alias each other but output is disjoint from all three stable spans. Counts
 * and capacities are float elements, 1..MAX_VALUES. Invalid arguments, capacity
 * or nonrepresentable outputs preserve every output element including the tail.
 * No allocation, clipping, model invocation, training, decoding or I/O. This is
 * one numerical transition, not a complete image/video sampler or quality claim.
 */
UmiStatus UmiAiDenoisingStochasticStep(const float *noisy,
    const float *predictedNoise, const float *stepNoise, size_t count,
    float currentAlphaBar, float previousAlphaBar, float eta,
    float *outPrevious, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
