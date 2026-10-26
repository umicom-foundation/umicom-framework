/* Umicom Framework | Reproducibility evidence | Sammy Hegab | MIT */
#include "stacked_inference_fixture.h"
#include "umicom/ai/training/stacked_generation_evidence.h"
int main(void)
{
    UmiAiTrainingStackedDecoder *model = NULL;
    const UmiAiTrainingNucleusOptions policy = {0.85F, 8U, 0.9F};
    const size_t prompt[] = {109U, 114U, 120U};
    size_t tokens[6] = {0}, changed[6] = {0}, written = 0U;
    UmiAiTrainingStackedGenerationEvidence evidence = {0}, candidate;
    CHECK(MakeModel(UINT64_C(61), &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedGenerateNucleus(model, prompt, 3U,
        &policy, 6U, 17U, tokens, 6U, &written, NULL) == UMI_STATUS_OK);
    CHECK(written == 6U);
    CHECK(UmiAiTrainingStackedGenerationEvidenceBuild(model, prompt, 3U,
        &policy, 17U, tokens, 6U, &evidence) == UMI_STATUS_OK);
    CHECK(evidence.promptTokenCount == 3U && evidence.outputTokenCount == 6U);
    CHECK(evidence.generation.computedTransformerTokenLayerRows == 48U);
    CHECK(UmiAiTrainingStackedGenerationEvidenceVerify(model, prompt, 3U,
        tokens, 6U, &evidence) == UMI_STATUS_OK);
    memcpy(changed, tokens, sizeof(changed));
    changed[2] = (changed[2] == 4U) ? 5U : 4U;
    memset(&candidate, 0x55, sizeof(candidate));
    UmiAiTrainingStackedGenerationEvidence untouched = candidate;
    CHECK(UmiAiTrainingStackedGenerationEvidenceBuild(model, prompt, 3U,
        &policy, 17U, changed, 6U, &candidate) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&candidate, &untouched, sizeof(candidate)) == 0);
    evidence.samplingPolicySha256[0] ^= 1U;
    CHECK(UmiAiTrainingStackedGenerationEvidenceVerify(model, prompt, 3U,
        tokens, 6U, &evidence) == UMI_STATUS_INVALID_STATE);
    evidence.samplingPolicySha256[0] ^= 1U;
    evidence.orderedOutputSha256[0] ^= 1U;
    CHECK(UmiAiTrainingStackedGenerationEvidenceVerify(model, prompt, 3U,
        tokens, 6U, &evidence) == UMI_STATUS_INVALID_STATE);
    UmiAiTrainingStackedDecoderDestroy(model);
    puts("stacked evidence: exact replay, digest matching and rejection passed");
    return 0;
}
