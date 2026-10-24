/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/rotary_acceptance.h
 * PURPOSE: Evaluate explicit, caller-supplied real-tape verification evidence.
 * ARCHITECTURE: Report interpretation only; no substitute neural kernel,
 * automatic-differentiation engine, or synthetic pass results. The caller
 * must run the existing real-tape qualification tests first.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_ROTARY_ACCEPTANCE_H
#define UMICOM_AI_TRAINING_ROTARY_ACCEPTANCE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_AI_ROTARY_ACCEPTANCE_TESTS 3U
#define UMI_AI_ROTARY_ACCEPTANCE_MAX_ABS_TOLERANCE 0.01
typedef enum UmiAiTrainingRotaryAcceptanceTest {
    UMI_AI_ROTARY_ACCEPTANCE_GRADIENTS = 0,
    UMI_AI_ROTARY_ACCEPTANCE_BUDGET = 1,
    UMI_AI_ROTARY_ACCEPTANCE_REWIND = 2
} UmiAiTrainingRotaryAcceptanceTest;
/* A test runner, not this module, supplies measurements. 'executed' is
 * distinct from a claimed pass. Maximum error is meaningful for gradients.
 * This is NOT a signed record, a test runner or evidence of machine provenance. */
typedef struct UmiAiTrainingRotaryAcceptanceObservation {
    uint32_t testId;
    int executed;
    int passed;
    uint32_t checks;
    double maximumAbsoluteError;
} UmiAiTrainingRotaryAcceptanceObservation;
typedef struct UmiAiTrainingRotaryAcceptanceReport {
    uint32_t executedTests;
    uint32_t passedTests;
    uint32_t totalChecks;
    double maximumGradientError;
    int accepted;
} UmiAiTrainingRotaryAcceptanceReport;
/* Exactly one observation of each real-tape regression is mandatory. There
 * are no defaults, no skipped tests and no acceptance of incomplete arrays.
 * Invalid data is rejected with outputs unchanged. An actual failed test
 * produces a successful evaluation with accepted=0, not a fabricated pass. */
UmiStatus UmiAiTrainingRotaryAcceptanceEvaluate(
    const UmiAiTrainingRotaryAcceptanceObservation *observations,
    size_t count, double gradientTolerance,
    UmiAiTrainingRotaryAcceptanceReport *outReport);
#ifdef __cplusplus
}
#endif
#endif
