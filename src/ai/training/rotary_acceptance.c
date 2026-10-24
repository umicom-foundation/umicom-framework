/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/rotary_acceptance.c
 * PURPOSE: Reject incomplete or contradictory real-tape acceptance evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_acceptance.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
UmiStatus UmiAiTrainingRotaryAcceptanceEvaluate(
    const UmiAiTrainingRotaryAcceptanceObservation *observations,
    size_t count, double gradientTolerance,
    UmiAiTrainingRotaryAcceptanceReport *outReport)
{
    UmiAiTrainingRotaryAcceptanceReport candidate = {0};
    bool seen[UMI_AI_ROTARY_ACCEPTANCE_TESTS] = {false, false, false};
    if (observations == NULL || outReport == NULL ||
        count != UMI_AI_ROTARY_ACCEPTANCE_TESTS ||
        !isfinite(gradientTolerance) || gradientTolerance <= 0.0 ||
        gradientTolerance > UMI_AI_ROTARY_ACCEPTANCE_MAX_ABS_TOLERANCE)
        return UMI_STATUS_INVALID_ARGUMENT;
    candidate.accepted = 1;
    for (size_t i = 0; i < count; ++i) {
        const UmiAiTrainingRotaryAcceptanceObservation *entry = &observations[i];
        if (entry->testId >= UMI_AI_ROTARY_ACCEPTANCE_TESTS ||
            seen[entry->testId] ||
            (entry->executed != 0 && entry->executed != 1) ||
            (entry->passed != 0 && entry->passed != 1) ||
            (entry->passed != 0 && entry->executed == 0) ||
            !isfinite(entry->maximumAbsoluteError) ||
            entry->maximumAbsoluteError < 0.0 ||
            (entry->executed == 0 && entry->checks != 0U) ||
            (entry->executed == 0 && entry->maximumAbsoluteError != 0.0) ||
            entry->checks > UINT32_MAX - candidate.totalChecks)
            return UMI_STATUS_INVALID_ARGUMENT;
        seen[entry->testId] = true;
        candidate.executedTests += (uint32_t)entry->executed;
        candidate.passedTests += (uint32_t)entry->passed;
        candidate.totalChecks += entry->checks;
        if (entry->testId == UMI_AI_ROTARY_ACCEPTANCE_GRADIENTS)
            candidate.maximumGradientError = entry->maximumAbsoluteError;
        /* An executed test without checks cannot establish its assertions. */
        if (entry->executed == 0 || entry->passed == 0 || entry->checks == 0U)
            candidate.accepted = 0;
    }
    for (size_t i = 0; i < UMI_AI_ROTARY_ACCEPTANCE_TESTS; ++i)
        if (!seen[i]) return UMI_STATUS_INVALID_ARGUMENT;
    if (candidate.maximumGradientError > gradientTolerance)
        candidate.accepted = 0;
    *outReport = candidate;
    return UMI_STATUS_OK;
}
