/* Umicom Framework | Exact byte tokenizer round-trip | Sammy Hegab | MIT */
#include "stacked_inference_fixture.h"
#include "umicom/ai/training/byte_tokenizer.h"
int main(void)
{
    UmiAiTrainingStackedDecoder *model = NULL;
    const UmiAiTrainingNucleusOptions policy = {0.92F, 11U, 1.0F};
    const uint8_t binary[] = {0U, 'C', '\n', 255U};
    uint32_t encoded[4] = {0};
    size_t prompt[4] = {0}, tokens[8] = {0};
    size_t encodedCount = 0U, count = 0U, byteCount = 0U;
    uint8_t expected[8] = {0}, actual[8] = {0};
    CHECK(MakeModel(UINT64_C(8), &model) == UMI_STATUS_OK);
    CHECK(UmiAiByteEncode(binary, sizeof(binary), encoded, 4U,
                            &encodedCount) == UMI_STATUS_OK);
    CHECK(encodedCount == 4U);
    for (size_t i = 0U; i < 4U; ++i) prompt[i] = (size_t)encoded[i];
    CHECK(UmiAiTrainingStackedGenerateNucleus(model, prompt, 4U,
        &policy, 8U, 112U, tokens, 8U, &count, NULL) == UMI_STATUS_OK);
    /* Decode the sampled IDs one at a time with the original tokenizer. */
    for (size_t i = 0U; i < count; ++i) {
        uint32_t id = (uint32_t)tokens[i];
        size_t one = 0U;
        CHECK(UmiAiByteDecode(&id, 1U, &expected[i], 1U, &one) == UMI_STATUS_OK);
    }
    CHECK(UmiAiTrainingStackedGenerateBytesNucleus(model,
        binary, sizeof(binary), &policy, 8U, 112U,
        actual, sizeof(actual), &byteCount) == UMI_STATUS_OK);
    CHECK(byteCount == 8U && memcmp(actual, expected, 8U) == 0);
    UmiAiTrainingStackedDecoderDestroy(model);
    puts("stacked inference: arbitrary binary byte boundaries preserved");
    return 0;
}
