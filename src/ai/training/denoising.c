/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/denoising.c
 *
 * PURPOSE:
 *   Validate complete scalar spans before publishing finite float denoising results.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/denoising.h"
#include <float.h>
#include <math.h>

/* One validated numerical owner supports both teaching directions. Evaluate
 * in double, check float representability before publishing any element, then
 * repeat the bounded arithmetic into the caller's disjoint output. This avoids
 * a temporary corpus allocation while retaining all-or-nothing publication. */
static double Value(float left, float right, double signal, double noise, int recover)
{
    return recover ? ((double)left - noise * (double)right) / signal :
        signal * (double)left + noise * (double)right;
}
static UmiStatus Transform(const float *left, const float *right, size_t count,
    float alphaBar, float *output, size_t capacity, int recover)
{
    if (left == NULL || right == NULL || output == NULL ||
        count == 0U || count > UMI_AI_DENOISING_MAX_VALUES ||
        !isfinite(alphaBar) || alphaBar < 0.0F || alphaBar > 1.0F ||
        (recover && alphaBar == 0.0F))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < count) return UMI_STATUS_CAPACITY_EXCEEDED;
    double signal = sqrt((double)alphaBar), noise = sqrt(1.0 - (double)alphaBar);
    for (size_t i = 0U; i < count; ++i) {
        if (!isfinite(left[i]) || !isfinite(right[i])) return UMI_STATUS_INVALID_ARGUMENT;
        double candidate = Value(left[i], right[i], signal, noise, recover);
        if (!isfinite(candidate) || candidate > FLT_MAX || candidate < -FLT_MAX)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (size_t i = 0U; i < count; ++i)
        output[i] = (float)Value(left[i], right[i], signal, noise, recover);
    return UMI_STATUS_OK;
}
UmiStatus UmiAiDenoisingMix(const float *clean, const float *noise,
    size_t count, float alphaBar, float *outNoisy, size_t capacity)
{
    return Transform(clean, noise, count, alphaBar, outNoisy, capacity, 0);
}
UmiStatus UmiAiDenoisingRecover(const float *noisy, const float *predictedNoise,
    size_t count, float alphaBar, float *outClean, size_t capacity)
{
    return Transform(noisy, predictedNoise, count, alphaBar, outClean, capacity, 1);
}

/* Exact endpoints are semantically useful and avoid subtract/add cancellation
 * when the two predictions have very different magnitudes. Other scales use
 * double intermediates and the same validate-then-publish ownership pattern. */
static double GuidedNoiseValue(float unconditioned, float conditioned, float scale)
{
    if (scale == 0.0F) return (double)unconditioned;
    if (scale == 1.0F) return (double)conditioned;
    return (double)unconditioned + (double)scale *
        ((double)conditioned - (double)unconditioned);
}
UmiStatus UmiAiDenoisingGuideNoise(const float *unconditioned,
    const float *conditioned, size_t count, float scale,
    float *outGuided, size_t capacity)
{
    if (unconditioned == NULL || conditioned == NULL || outGuided == NULL ||
        count == 0U || count > UMI_AI_DENOISING_MAX_VALUES ||
        !isfinite(scale) || scale < 0.0F)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < count) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < count; ++i) {
        if (!isfinite(unconditioned[i]) || !isfinite(conditioned[i]))
            return UMI_STATUS_INVALID_ARGUMENT;
        double value = GuidedNoiseValue(unconditioned[i], conditioned[i], scale);
        if (!isfinite(value) || value > FLT_MAX || value < -FLT_MAX)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (size_t i = 0U; i < count; ++i)
        outGuided[i] = (float)GuidedNoiseValue(unconditioned[i], conditioned[i], scale);
    return UMI_STATUS_OK;
}

/* Keep reconstruction and reverse stepping under the same numerical owner.
 * x0 stays in a scalar double instead of allocating another full tensor. The
 * admission pass covers every element before the publication pass writes.
 * Equal schedule points preserve the original float, including signed zero. */
static double DeterministicStepValue(float noisy, float predictedNoise,
    double currentSignal, double currentNoise, double previousSignal,
    double previousNoise, int identity)
{
    if (identity) return (double)noisy;
    double clean = Value(noisy, predictedNoise, currentSignal, currentNoise, 1);
    return previousSignal * clean + previousNoise * (double)predictedNoise;
}
UmiStatus UmiAiDenoisingDeterministicStep(const float *noisy,
    const float *predictedNoise, size_t count, float currentAlphaBar,
    float previousAlphaBar, float *outPrevious, size_t capacity)
{
    if (noisy == NULL || predictedNoise == NULL || outPrevious == NULL ||
        count == 0U || count > UMI_AI_DENOISING_MAX_VALUES ||
        !isfinite(currentAlphaBar) || !isfinite(previousAlphaBar) ||
        currentAlphaBar <= 0.0F || currentAlphaBar > 1.0F ||
        previousAlphaBar < currentAlphaBar || previousAlphaBar > 1.0F)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < count) return UMI_STATUS_CAPACITY_EXCEEDED;
    double currentSignal = sqrt((double)currentAlphaBar);
    double currentNoise = sqrt(1.0 - (double)currentAlphaBar);
    double previousSignal = sqrt((double)previousAlphaBar);
    double previousNoise = sqrt(1.0 - (double)previousAlphaBar);
    int identity = currentAlphaBar == previousAlphaBar;
    for (size_t i = 0U; i < count; ++i) {
        if (!isfinite(noisy[i]) || !isfinite(predictedNoise[i]))
            return UMI_STATUS_INVALID_ARGUMENT;
        double candidate = DeterministicStepValue(noisy[i], predictedNoise[i],
            currentSignal, currentNoise, previousSignal, previousNoise, identity);
        if (!isfinite(candidate) || candidate > FLT_MAX || candidate < -FLT_MAX)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (size_t i = 0U; i < count; ++i)
        outPrevious[i] = (float)DeterministicStepValue(noisy[i], predictedNoise[i],
            currentSignal, currentNoise, previousSignal, previousNoise, identity);
    return UMI_STATUS_OK;
}

/* A diffusion prediction is a coordinate convention, not an interchangeable
 * float array. Keep both inverse rotation rows in one Framework owner so
 * consumers can explicitly adapt v-predicting models to epsilon operations.
 * Endpoint branches preserve the selected float, including its signed zero;
 * the clean endpoint at zero correctly negates v. Double intermediates and
 * two passes avoid a tensor-sized scratch allocation or partial publication. */
static double VelocityPredictionValue(float noisy, float velocity,
    float alphaBar, double signal, double noise, int clean)
{
    if (alphaBar == 0.0F) return clean ? -(double)velocity : (double)noisy;
    if (alphaBar == 1.0F) return clean ? (double)noisy : (double)velocity;
    return clean ? signal * (double)noisy - noise * (double)velocity :
        noise * (double)noisy + signal * (double)velocity;
}
static UmiStatus VelocityPredictionTransform(const float *noisy,
    const float *velocity, size_t count, float alphaBar, float *output,
    size_t capacity, int clean)
{
    if (noisy == NULL || velocity == NULL || output == NULL ||
        count == 0U || count > UMI_AI_DENOISING_MAX_VALUES ||
        !isfinite(alphaBar) || alphaBar < 0.0F || alphaBar > 1.0F)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < count) return UMI_STATUS_CAPACITY_EXCEEDED;
    double signal = sqrt((double)alphaBar);
    double noise = sqrt(1.0 - (double)alphaBar);
    for (size_t i = 0U; i < count; ++i) {
        if (!isfinite(noisy[i]) || !isfinite(velocity[i]))
            return UMI_STATUS_INVALID_ARGUMENT;
        double candidate = VelocityPredictionValue(noisy[i], velocity[i],
            alphaBar, signal, noise, clean);
        if (!isfinite(candidate) || candidate > FLT_MAX || candidate < -FLT_MAX)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (size_t i = 0U; i < count; ++i)
        output[i] = (float)VelocityPredictionValue(noisy[i], velocity[i],
            alphaBar, signal, noise, clean);
    return UMI_STATUS_OK;
}
UmiStatus UmiAiDenoisingVelocityToNoise(const float *noisy,
    const float *predictedVelocity, size_t count, float alphaBar,
    float *outNoise, size_t capacity)
{
    return VelocityPredictionTransform(noisy, predictedVelocity, count,
        alphaBar, outNoise, capacity, 0);
}
UmiStatus UmiAiDenoisingVelocityToClean(const float *noisy,
    const float *predictedVelocity, size_t count, float alphaBar,
    float *outClean, size_t capacity)
{
    return VelocityPredictionTransform(noisy, predictedVelocity, count,
        alphaBar, outClean, capacity, 1);
}

/* Keep all denoising arithmetic in Framework. Stable positive coefficient
 * expressions avoid subtracting nearly equal variances. The two passes retain
 * transactional publication without allocating a tensor-sized scratch buffer. */
static double StochasticStepValue(float noisy, float predictedNoise, float stepNoise,
    double currentSignal, double currentNoise, double previousSignal,
    double direction, double sigma)
{
    double clean = Value(noisy, predictedNoise, currentSignal, currentNoise, 1);
    return previousSignal * clean + direction * (double)predictedNoise + sigma * (double)stepNoise;
}
UmiStatus UmiAiDenoisingStochasticStep(const float *noisy,
    const float *predictedNoise, const float *stepNoise, size_t count,
    float currentAlphaBar, float previousAlphaBar, float eta,
    float *outPrevious, size_t capacity)
{
    if (noisy == NULL || predictedNoise == NULL || stepNoise == NULL || outPrevious == NULL ||
        count == 0U || count > UMI_AI_DENOISING_MAX_VALUES ||
        !isfinite(currentAlphaBar) || !isfinite(previousAlphaBar) || !isfinite(eta) ||
        currentAlphaBar <= 0.0F || currentAlphaBar > 1.0F ||
        previousAlphaBar < currentAlphaBar || previousAlphaBar > 1.0F || eta < 0.0F || eta > 1.0F)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < count) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < count; ++i)
        if (!isfinite(stepNoise[i])) return UMI_STATUS_INVALID_ARGUMENT;
    if (eta == 0.0F || currentAlphaBar == previousAlphaBar || previousAlphaBar == 1.0F)
        return UmiAiDenoisingDeterministicStep(noisy, predictedNoise, count,
            currentAlphaBar, previousAlphaBar, outPrevious, capacity);
    double a = currentAlphaBar, p = previousAlphaBar, etaSquared = (double)eta * (double)eta;
    double denominator = p * (1.0 - a);
    double sigma = (double)eta * sqrt((1.0 - p) * (p - a) / denominator);
    double direction = sqrt((1.0 - p) * ((1.0 - etaSquared) +
        etaSquared * a * (1.0 - p) / denominator));
    double currentSignal = sqrt(a), currentNoise = sqrt(1.0 - a), previousSignal = sqrt(p);
    for (size_t i = 0U; i < count; ++i) {
        if (!isfinite(noisy[i]) || !isfinite(predictedNoise[i])) return UMI_STATUS_INVALID_ARGUMENT;
        double candidate = StochasticStepValue(noisy[i], predictedNoise[i], stepNoise[i],
            currentSignal, currentNoise, previousSignal, direction, sigma);
        if (!isfinite(candidate) || candidate > FLT_MAX || candidate < -FLT_MAX)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (size_t i = 0U; i < count; ++i)
        outPrevious[i] = (float)StochasticStepValue(noisy[i], predictedNoise[i], stepNoise[i],
            currentSignal, currentNoise, previousSignal, direction, sigma);
    return UMI_STATUS_OK;
}
