/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/model_quality_evidence.c
 *
 * PURPOSE:
 *   Produce exact, locale-independent JSON evidence for already compared
 *   native model-quality reports. Preserve original quality-gate behaviour.
 *
 * DESIGN:
 *   The existing ModelQualityCompareReports remains the sole decision owner.
 *   We recompute its decision from the two audits and policy and refuse stale
 *   reports. The JSON includes exact binary64 bit patterns rather than locale-
 *   dependent decimal strings or native struct representations. Every field
 *   is fixed ASCII or an integer, so no dynamic JSON dependency is required.
 *   Only a fully formed local buffer is ever copied to caller storage.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/model_quality_evidence.h"

#include <float.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#if FLT_RADIX != 2 || DBL_MANT_DIG != 53 || DBL_MAX_EXP != 1024
#error Model quality evidence requires IEEE754 binary64 doubles.
#endif
_Static_assert(sizeof(double) == sizeof(uint64_t),
               "Model quality evidence requires a 64-bit double");

/* memcpy avoids violating strict aliasing and does not depend on host byte
 * order: the numeric uint64_t value is formatted as canonical hexadecimal. */
static uint64_t DoubleBits(double number)
{
    uint64_t bits = UINT64_C(0);
    memcpy(&bits, &number, sizeof(bits));
    return bits;
}

UmiStatus UmiAiTrainingModelQualityEvidenceJson(
    const UmiAiTrainingModelQualityReport *report,
    const UmiAiTrainingModelQualityPolicy *policy,
    char *outJson, size_t capacity, size_t *outWritten)
{
    UmiAiTrainingModelQualityReport checked = {0};
    char staged[UMI_AI_MODEL_QUALITY_EVIDENCE_JSON_CAPACITY];
    int count;
    UmiStatus status;

    if (report == NULL || policy == NULL || outJson == NULL ||
        outWritten == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;

    /* Invalid audit data or policy must never cause an output write. */
    status = UmiAiTrainingModelQualityCompareReports(
        &report->baseline, &report->candidate, policy, &checked);
    if (status != UMI_STATUS_OK) return status;
    if ((report->accepted != 0 && report->accepted != 1) ||
        report->accepted != checked.accepted ||
        DoubleBits(report->crossEntropyReduction) !=
            DoubleBits(checked.crossEntropyReduction) ||
        DoubleBits(report->topOneAccuracyChange) !=
            DoubleBits(checked.topOneAccuracyChange) ||
        DoubleBits(report->topFiveAccuracyChange) !=
            DoubleBits(checked.topFiveAccuracyChange))
        return UMI_STATUS_INVALID_ARGUMENT;

    /* No caller-controlled names or paths are included. Deliberately mark
     * unverified assurances as false instead of implying that the unsigned
     * report is itself a valid release authorisation. */
    count = snprintf(staged, sizeof(staged),
        "{\"schema\":\"umicom.ai.model_quality.evidence.1\","
        "\"evaluation\":{\"dataset_fnv1a64\":\"%016" PRIx64 "\","
        "\"plan_fnv1a64\":\"%016" PRIx64 "\","
        "\"evaluated_tokens\":%zu,\"training_documents\":%zu,"
        "\"evaluation_documents\":%zu},"
        "\"baseline\":{\"checkpoint_fnv1a64\":\"%016" PRIx64 "\","
        "\"held_out_cross_entropy_binary64\":\"%016" PRIx64 "\","
        "\"held_out_perplexity_binary64\":\"%016" PRIx64 "\","
        "\"top_one_correct\":%zu,\"top_five_correct\":%zu},"
        "\"candidate\":{\"checkpoint_fnv1a64\":\"%016" PRIx64 "\","
        "\"held_out_cross_entropy_binary64\":\"%016" PRIx64 "\","
        "\"held_out_perplexity_binary64\":\"%016" PRIx64 "\","
        "\"top_one_correct\":%zu,\"top_five_correct\":%zu},"
        "\"policy\":{\"minimum_evaluated_tokens\":%zu,"
        "\"minimum_cross_entropy_reduction_binary64\":\"%016" PRIx64 "\","
        "\"maximum_top_one_regression_binary64\":\"%016" PRIx64 "\","
        "\"maximum_top_five_regression_binary64\":\"%016" PRIx64 "\"},"
        "\"decision\":{\"accepted\":%s,"
        "\"cross_entropy_reduction_binary64\":\"%016" PRIx64 "\","
        "\"top_one_fraction_change_binary64\":\"%016" PRIx64 "\","
        "\"top_five_fraction_change_binary64\":\"%016" PRIx64 "\"},"
        "\"assurance\":{\"publisher_signature_verified\":false,"
        "\"independent_release_approval_verified\":false,"
        "\"programming_competence_proven\":false}}",
        checked.baseline.datasetFingerprint,
        checked.baseline.planFingerprint,
        checked.baseline.evaluatedTokens,
        checked.baseline.dataset.trainingDocuments,
        checked.baseline.dataset.evaluationDocuments,
        checked.baseline.modelFingerprint,
        DoubleBits(checked.baseline.heldOutCrossEntropy),
        DoubleBits(checked.baseline.heldOutPerplexity),
        checked.baseline.topOneCorrect,
        checked.baseline.topFiveCorrect,
        checked.candidate.modelFingerprint,
        DoubleBits(checked.candidate.heldOutCrossEntropy),
        DoubleBits(checked.candidate.heldOutPerplexity),
        checked.candidate.topOneCorrect,
        checked.candidate.topFiveCorrect,
        policy->minimumEvaluatedTokens,
        DoubleBits(policy->minimumCrossEntropyReduction),
        DoubleBits(policy->maximumTopOneAccuracyRegression),
        DoubleBits(policy->maximumTopFiveAccuracyRegression),
        checked.accepted != 0 ? "true" : "false",
        DoubleBits(checked.crossEntropyReduction),
        DoubleBits(checked.topOneAccuracyChange),
        DoubleBits(checked.topFiveAccuracyChange));

    if (count < 0) return UMI_STATUS_INTERNAL_ERROR;
    if ((size_t)count >= sizeof(staged) || (size_t)count >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(outJson, staged, (size_t)count + 1U);
    *outWritten = (size_t)count;
    return UMI_STATUS_OK;
}
