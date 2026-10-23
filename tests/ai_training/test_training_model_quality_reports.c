/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_model_quality_reports.c
 * PURPOSE: Check deterministic candidate selection with comparable held-out
 *          evidence and distinct model checkpoint identities.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/model_quality_gate.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; \
} } while (0)

static UmiAiTrainingModelAudit MakeAudit(uint64_t modelIdentity,
                                        double loss, size_t topOne,
                                        size_t topFive)
{
    UmiAiTrainingModelAudit audit = {0};
    audit.modelFingerprint = modelIdentity;
    audit.datasetFingerprint = UINT64_C(234);
    audit.planFingerprint = UINT64_C(345);
    audit.combinedFingerprint = modelIdentity ^ UINT64_C(786);
    audit.completedSteps = 4U;
    audit.dataset = (UmiAiTrainingDatasetSummary){
        .documentCount = 3U, .trainingDocuments = 2U,
        .evaluationDocuments = 1U, .trainingBytes = 800U,
        .evaluationBytes = 200U};
    audit.evaluatedTokens = 100U;
    audit.topOneCorrect = topOne;
    audit.topFiveCorrect = topFive;
    audit.heldOutCrossEntropy = loss;
    audit.heldOutPerplexity = exp(loss);
    return audit;
}

int main(void)
{
    UmiAiTrainingModelQualityPolicy policy;
    UmiAiTrainingModelQualityReport result;
    UmiAiTrainingModelAudit baseline = MakeAudit(1U, 3.0, 30U, 70U);
    UmiAiTrainingModelAudit candidate = MakeAudit(2U, 2.5, 35U, 75U);
    CHECK(UmiAiTrainingModelQualityDefaults(&policy) == UMI_STATUS_OK);
    CHECK(policy.minimumEvaluatedTokens == 32U);
    CHECK(UmiAiTrainingModelQualityCompareReports(
        &baseline, &candidate, &policy, &result) == UMI_STATUS_OK);
    CHECK(result.accepted == 1);
    CHECK(fabs(result.crossEntropyReduction - 0.5) < 1e-12);
    CHECK(fabs(result.topOneAccuracyChange - 0.05) < 1e-12);
    CHECK(fabs(result.topFiveAccuracyChange - 0.05) < 1e-12);

    /* Loss reduction and top-k regressions are independent requirements. */
    policy.minimumCrossEntropyReduction = 0.75;
    CHECK(UmiAiTrainingModelQualityCompareReports(
        &baseline, &candidate, &policy, &result) == UMI_STATUS_OK);
    CHECK(result.accepted == 0);
    policy.minimumCrossEntropyReduction = 0.1;
    candidate.topOneCorrect = 25U;
    CHECK(UmiAiTrainingModelQualityCompareReports(
        &baseline, &candidate, &policy, &result) == UMI_STATUS_OK);
    CHECK(result.accepted == 0);
    policy.maximumTopOneAccuracyRegression = 0.06;
    CHECK(UmiAiTrainingModelQualityCompareReports(
        &baseline, &candidate, &policy, &result) == UMI_STATUS_OK);
    CHECK(result.accepted == 1);

    candidate.heldOutCrossEntropy = baseline.heldOutCrossEntropy;
    candidate.heldOutPerplexity = baseline.heldOutPerplexity;
    CHECK(UmiAiTrainingModelQualityCompareReports(
        &baseline, &candidate, &policy, &result) == UMI_STATUS_OK);
    CHECK(result.accepted == 0); /* A tie is NOT an improvement. */

    candidate = MakeAudit(2U, 2.5, 35U, 75U);
    candidate.modelFingerprint = baseline.modelFingerprint;
    CHECK(UmiAiTrainingModelQualityCompareReports(
        &baseline, &candidate, &policy, &result) == UMI_STATUS_OK);
    CHECK(result.accepted == 0); /* Same checkpoint is not a new candidate. */
    puts("native model quality: measured improvement and independent top-k guards PASS");
    return 0;
}
