/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_model_quality_rejections.c
 * PURPOSE: Reject malformed/incomparable held-out evidence without touching
 *          an existing caller-owned quality decision.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/model_quality_gate.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; \
} } while (0)

static UmiAiTrainingModelAudit Audit(void)
{
    UmiAiTrainingModelAudit audit = {0};
    audit.modelFingerprint = 11U;
    audit.datasetFingerprint = 22U;
    audit.planFingerprint = 33U;
    audit.dataset = (UmiAiTrainingDatasetSummary){
        .documentCount = 2U, .trainingDocuments = 1U,
        .evaluationDocuments = 1U, .trainingBytes = 200U,
        .evaluationBytes = 100U};
    audit.evaluatedTokens = 64U;
    audit.topOneCorrect = 20U;
    audit.topFiveCorrect = 35U;
    audit.heldOutCrossEntropy = 3.0;
    audit.heldOutPerplexity = exp(3.0);
    return audit;
}

static int RejectWithoutOutputChange(
    const UmiAiTrainingModelAudit *baseline,
    const UmiAiTrainingModelAudit *candidate,
    const UmiAiTrainingModelQualityPolicy *policy)
{
    UmiAiTrainingModelQualityReport output;
    UmiAiTrainingModelQualityReport saved;
    memset(&output, 0x6a, sizeof(output));
    memcpy(&saved, &output, sizeof(saved));
    CHECK(UmiAiTrainingModelQualityCompareReports(
        baseline, candidate, policy, &output) != UMI_STATUS_OK);
    CHECK(memcmp(&output, &saved, sizeof(output)) == 0);
    return 0;
}

int main(void)
{
    UmiAiTrainingModelAudit baseline = Audit();
    UmiAiTrainingModelAudit candidate = Audit();
    UmiAiTrainingModelQualityPolicy policy;
    UmiAiTrainingModelQualityReport report;
    CHECK(UmiAiTrainingModelQualityDefaults(&policy) == UMI_STATUS_OK);
    candidate.modelFingerprint = 99U;
    candidate.heldOutCrossEntropy = 2.75;
    candidate.heldOutPerplexity = exp(2.75);
    CHECK(RejectWithoutOutputChange(NULL, &candidate, &policy) == 0);
    CHECK(RejectWithoutOutputChange(&baseline, NULL, &policy) == 0);
    CHECK(RejectWithoutOutputChange(&baseline, &candidate, NULL) == 0);
    CHECK(UmiAiTrainingModelQualityCompareReports(
        &baseline, &candidate, &policy, NULL) == UMI_STATUS_INVALID_ARGUMENT);

    policy.minimumCrossEntropyReduction = NAN;
    CHECK(RejectWithoutOutputChange(&baseline, &candidate, &policy) == 0);
    policy.minimumCrossEntropyReduction = -0.01;
    CHECK(RejectWithoutOutputChange(&baseline, &candidate, &policy) == 0);
    policy.minimumCrossEntropyReduction = 0.0;
    policy.maximumTopOneAccuracyRegression = INFINITY;
    CHECK(RejectWithoutOutputChange(&baseline, &candidate, &policy) == 0);
    policy.maximumTopOneAccuracyRegression = 0.0;
    policy.maximumTopFiveAccuracyRegression = 1.01;
    CHECK(RejectWithoutOutputChange(&baseline, &candidate, &policy) == 0);
    policy.maximumTopFiveAccuracyRegression = 0.0;
    policy.minimumEvaluatedTokens = 0U;
    CHECK(RejectWithoutOutputChange(&baseline, &candidate, &policy) == 0);
    policy.minimumEvaluatedTokens = 32U;

    candidate.planFingerprint++;
    CHECK(RejectWithoutOutputChange(&baseline, &candidate, &policy) == 0);
    candidate = Audit(); candidate.datasetFingerprint++;
    CHECK(RejectWithoutOutputChange(&baseline, &candidate, &policy) == 0);
    candidate = Audit(); candidate.dataset.evaluationBytes++;
    CHECK(RejectWithoutOutputChange(&baseline, &candidate, &policy) == 0);
    candidate = Audit(); candidate.evaluatedTokens++;
    CHECK(RejectWithoutOutputChange(&baseline, &candidate, &policy) == 0);
    candidate = Audit(); candidate.topFiveCorrect = candidate.topOneCorrect - 1U;
    CHECK(RejectWithoutOutputChange(&baseline, &candidate, &policy) == 0);
    candidate = Audit(); candidate.heldOutPerplexity = NAN;
    CHECK(RejectWithoutOutputChange(&baseline, &candidate, &policy) == 0);
    candidate = Audit(); candidate.heldOutCrossEntropy = -1.0;
    CHECK(RejectWithoutOutputChange(&baseline, &candidate, &policy) == 0);
    candidate = Audit(); candidate.evaluatedTokens = 0U;
    CHECK(RejectWithoutOutputChange(&baseline, &candidate, &policy) == 0);

    /* A valid comparison with an insufficient sample is an honest failure of
     * the policy, not an invalid API call. */
    candidate = Audit(); candidate.modelFingerprint = 99U;
    candidate.heldOutCrossEntropy = 2.75;
    candidate.heldOutPerplexity = exp(2.75);
    policy.minimumEvaluatedTokens = 65U;
    CHECK(UmiAiTrainingModelQualityCompareReports(
        &baseline, &candidate, &policy, &report) == UMI_STATUS_OK);
    CHECK(report.accepted == 0);
    puts("native model quality: incompatible reports, invalid thresholds and atomic outputs PASS");
    return 0;
}
