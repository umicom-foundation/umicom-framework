/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_model_quality_evidence_rejections.c
 * PURPOSE: Reject stale or malformed evidence and insufficient JSON capacity
 *          without modifying caller-owned bytes or length.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/model_quality_evidence.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; \
} } while (0)

static UmiAiTrainingModelAudit Audit(uint64_t modelIdentity, double loss)
{
    UmiAiTrainingModelAudit audit = {0};
    audit.modelFingerprint = modelIdentity;
    audit.datasetFingerprint = UINT64_C(20);
    audit.planFingerprint = UINT64_C(30);
    audit.dataset = (UmiAiTrainingDatasetSummary){
        .documentCount = 2U, .trainingDocuments = 1U,
        .evaluationDocuments = 1U, .trainingBytes = 50U,
        .evaluationBytes = 50U};
    audit.evaluatedTokens = 64U;
    audit.topOneCorrect = 20U;
    audit.topFiveCorrect = 35U;
    audit.heldOutCrossEntropy = loss;
    audit.heldOutPerplexity = exp(loss);
    return audit;
}

static int RejectWithoutMutation(
    const UmiAiTrainingModelQualityReport *report,
    const UmiAiTrainingModelQualityPolicy *policy,
    size_t capacity)
{
    char output[UMI_AI_MODEL_QUALITY_EVIDENCE_JSON_CAPACITY];
    char saved[UMI_AI_MODEL_QUALITY_EVIDENCE_JSON_CAPACITY];
    size_t written = 987U;
    memset(output, 0x5a, sizeof(output));
    memcpy(saved, output, sizeof(output));
    CHECK(UmiAiTrainingModelQualityEvidenceJson(
        report, policy, output, capacity, &written) != UMI_STATUS_OK);
    CHECK(memcmp(output, saved, sizeof(output)) == 0);
    CHECK(written == 987U);
    return 0;
}

int main(void)
{
    UmiAiTrainingModelAudit baseline = Audit(1U, 3.0);
    UmiAiTrainingModelAudit candidate = Audit(2U, 2.5);
    UmiAiTrainingModelQualityPolicy policy;
    UmiAiTrainingModelQualityReport report;
    char output[UMI_AI_MODEL_QUALITY_EVIDENCE_JSON_CAPACITY];
    size_t written = 0U;
    CHECK(UmiAiTrainingModelQualityDefaults(&policy) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingModelQualityCompareReports(
        &baseline, &candidate, &policy, &report) == UMI_STATUS_OK);
    CHECK(RejectWithoutMutation(&report, &policy, 1U) == 0);
    CHECK(RejectWithoutMutation(&report, &policy, 0U) == 0);
    CHECK(RejectWithoutMutation(NULL, &policy, sizeof(output)) == 0);
    CHECK(RejectWithoutMutation(&report, NULL, sizeof(output)) == 0);
    CHECK(UmiAiTrainingModelQualityEvidenceJson(
        &report, &policy, NULL, 0U, &written) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingModelQualityEvidenceJson(
        &report, &policy, output, sizeof(output), NULL) == UMI_STATUS_INVALID_ARGUMENT);

    report.accepted = !report.accepted;
    CHECK(RejectWithoutMutation(&report, &policy, sizeof(output)) == 0);
    report.accepted = !report.accepted;
    report.crossEntropyReduction += 0.1;
    CHECK(RejectWithoutMutation(&report, &policy, sizeof(output)) == 0);
    CHECK(UmiAiTrainingModelQualityCompareReports(
        &baseline, &candidate, &policy, &report) == UMI_STATUS_OK);
    report.candidate.planFingerprint++;
    CHECK(RejectWithoutMutation(&report, &policy, sizeof(output)) == 0);
    report.candidate.planFingerprint--;
    report.baseline.heldOutCrossEntropy = NAN;
    CHECK(RejectWithoutMutation(&report, &policy, sizeof(output)) == 0);
    CHECK(UmiAiTrainingModelQualityCompareReports(
        &baseline, &candidate, &policy, &report) == UMI_STATUS_OK);
    policy.maximumTopOneAccuracyRegression = INFINITY;
    CHECK(RejectWithoutMutation(&report, &policy, sizeof(output)) == 0);
    puts("model quality evidence: invalid inputs, stale results and atomic outputs PASS");
    return 0;
}
