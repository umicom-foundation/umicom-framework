/* Umicom Framework | multi-layer tape memory and transactional graph rewind | Sammy Hegab | MIT */
#include "stacked_test_fixture.h"
int main(void)
{
    UmiAiTrainingStackedDecoderConfig cfg = TestStackedConfig();
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingTapeMemory initial = {0}, observed = {0};
    size_t inputs[4], targets[4];
    uint8_t mask[4];
    float logits[64], loss = 0.0F;
    UmiAiTrainingStackedStep step = {0};
    TestStackedWindow(inputs, targets, mask);
    CHECK(UmiAiTrainingStackedDecoderCreate(&cfg, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedDecoderMemoryInspect(model, &initial) == UMI_STATUS_OK);
    CHECK(initial.tensorCount == 27U);
    for (size_t i = 0U; i < 12U; ++i) {
        CHECK(UmiAiTrainingStackedDecoderLogits(model, inputs, 4U,
              logits, 64U) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingStackedDecoderEvaluateSelected(model, inputs,
              targets, mask, 4U, &loss) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingStackedDecoderTrainSelected(model, inputs,
              targets, mask, 4U, 0.015F, &step) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingStackedDecoderMemoryInspect(model, &observed) ==
              UMI_STATUS_OK);
        CHECK(observed.tensorCount == initial.tensorCount);
        CHECK(observed.currentBytes == initial.currentBytes);
        CHECK(observed.peakBytes >= observed.currentBytes);
    }
    CHECK(step.completedTrainingSteps == 12U);
    UmiAiTrainingStackedDecoderDestroy(model);
    puts("multi-layer graph lifetime and tracked current-memory rewind passed");
    return 0;
}
