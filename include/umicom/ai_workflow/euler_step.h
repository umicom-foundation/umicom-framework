/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_workflow/euler_step.h
 *
 * PURPOSE:
 *   Expose a bounded native Euler denoising step for image and video latent buffers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_WORKFLOW_EULER_STEP_H
#define UMICOM_AI_WORKFLOW_EULER_STEP_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_AI_EULER_MAX_VALUES 16777216U
/* x_next = (next_sigma/sigma)*x + (1-next_sigma/sigma)*denoised.
 * Input is the model's predicted CLEAN sample, not epsilon or velocity.
 * sigma is finite and positive; 0 <= next_sigma < sigma. This is one
 * deterministic first-order Euler step, without stochastic noise injection.
 * A video latent can be a flattened buffer; layout belongs to the model.
 *
 * No allocation or model execution. Both inputs contain count finite floats.
 * count is 1..MAX_VALUES; capacity is measured in floats. Output may exactly
 * equal either input, otherwise it must be disjoint from both. Partial overlap
 * is unsupported. Inputs remain stable for the call; the caller owns storage.
 * Admission scans all values before writing; errors preserve the whole output.
 * This numerical primitive does not load weights, tokenize prompts, train a
 * model, decode latents or establish image/video generation capability. */
UmiStatus UmiAiEulerDenoisedStep(const float *sample, const float *denoised,
    size_t count, double sigma, double next_sigma, float *out, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
