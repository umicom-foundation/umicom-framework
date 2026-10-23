/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_model_quality_evidence.c
 * PURPOSE: Check deterministic, locale-independent model-quality JSON output,
 *          derived from validated canonical audit reports and policy.
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

static UmiAiTrainingModelAudit MakeAudit(uint64_t identity,
                                         double crossEntropy,
                                         size_t topOne, size_t topFive)
{
    UmiAiTrainingModelAudit audit = {0};
    audit.modelFingerprint = identity;
    audit.datasetFingerprint = UINT64_C(0x3344);
    audit.planFingerprint = UINT64_C(0x5566);
    audit.dataset = (UmiAiTrainingDatasetSummary){
        .documentCount = 2U, .trainingDocuments = 1U,
        .evaluationDocuments = 1U, .trainingBytes = 100U,
        .evaluationBytes = 80U
    };
    audit.completedSteps = 3U;
    audit.evaluatedTokens = 64U;
    audit.topOneCorrect = topOne;
    audit.topFiveCorrect = topFive;
    audit.heldOutCrossEntropy = crossEntropy;
    audit.heldOutPerplexity = exp(crossEntropy);
    return audit;
}

int main(void)
{
    UmiAiTrainingModelQualityPolicy policy;
    UmiAiTrainingModelQualityReport comparison;
    UmiAiTrainingModelAudit baseline = MakeAudit(1U, 3.0, 15U, 35U);
    UmiAiTrainingModelAudit candidate = MakeAudit(2U, 2.5, 20U, 40U);
    char json[UMI_AI_MODEL_QUALITY_EVIDENCE_JSON_CAPACITY];
    char repeated[UMI_AI_MODEL_QUALITY_EVIDENCE_JSON_CAPACITY];
    size_t count = 0U, repeatedCount = 0U;
    CHECK(UmiAiTrainingModelQualityDefaults(&policy) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingModelQualityCompareReports(
        &baseline, &candidate, &policy, &comparison) == UMI_STATUS_OK);
    CHECK(comparison.accepted == 1);
    CHECK(UmiAiTrainingModelQualityEvidenceJson(
        &comparison, &policy, json, sizeof(json), &count) == UMI_STATUS_OK);
    CHECK(count == strlen(json));
    CHECK(strstr(json, "\"schema\":\"umicom.ai.model_quality.evidence.1\"") != NULL);
    CHECK(strstr(json, "\"dataset_fnv1a64\":\"0000000000003344\"") != NULL);
    CHECK(strstr(json, "\"plan_fnv1a64\":\"0000000000005566\"") != NULL);
    CHECK(strstr(json, "\"evaluated_tokens\":64") != NULL);
    CHECK(strstr(json, "\"accepted\":true") != NULL);
    CHECK(strstr(json, "\"cross_entropy_reduction_binary64\":\"3fe0000000000000\"") != NULL);
    CHECK(strstr(json, "\"publisher_signature_verified\":false") != NULL);
    CHECK(strstr(json, "\"independent_release_approval_verified\":false") != NULL);

    CHECK(UmiAiTrainingModelQualityEvidenceJson(
        &comparison, &policy, repeated, sizeof(repeated),
        &repeatedCount) == UMI_STATUS_OK);
    CHECK(repeatedCount == count);
    CHECK(memcmp(json, repeated, count + 1U) == 0);

    /* A policy-valid non-improvement still has a complete evidence record:
     * a quality failure is a decision, not an API or parsing error. */
    policy.minimumCrossEntropyReduction = 0.75;
    CHECK(UmiAiTrainingModelQualityCompareReports(
        &baseline, &candidate, &policy, &comparison) == UMI_STATUS_OK);
    CHECK(comparison.accepted == 0);
    CHECK(UmiAiTrainingModelQualityEvidenceJson(
        &comparison, &policy, json, sizeof(json), &count) == UMI_STATUS_OK);
    CHECK(strstr(json, "\"accepted\":false") != NULL);
    puts("model quality evidence: deterministic JSON and honest acceptance PASS");
    return 0;
}
