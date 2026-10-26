/* Umicom Framework | invalidate every cache session on model update. | MIT */
#include "stacked_incremental_fixture.h"

int main(void)
{
    const UmiAiTrainingStackedDecoderConfig cfg = IncrementalConfig(1U, 4U);
    const UmiAiTrainingNucleusOptions policy = {1.0F, 1U, 1.0F};
    const size_t prompt[] = {42U, 43U};
    const size_t targets[4] = {5U, 6U, 7U, 8U};
    const size_t inputs[4] = {5U, 6U, 7U, 8U};
    const uint8_t mask[4] = {1U, 1U, 1U, 1U};
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedKvIncrementalSession *session = NULL;
    UmiAiTrainingStackedStep train = {0};
    UmiAiTrainingStackedKvIncrementalStats observed;
    size_t output = 999U;
    VERIFY(UmiAiTrainingStackedDecoderCreate(&cfg, &model) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvIncrementalSessionCreate(model,
        prompt, 2U, &policy, UINT64_C(4), &session) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedDecoderTrainSelected(model,
        inputs, targets, mask, 4U, 0.0001F, &train) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvIncrementalSessionNext(session,
        &output) == UMI_STATUS_INVALID_STATE);
    VERIFY(output == 999U);
    memset(&observed, 0xa5, sizeof(observed));
    UmiAiTrainingStackedKvIncrementalStats original = observed;
    VERIFY(UmiAiTrainingStackedKvIncrementalSessionInspect(session,
        &observed) == UMI_STATUS_INVALID_STATE);
    VERIFY(memcmp(&observed, &original, sizeof(observed)) == 0);
    UmiAiTrainingStackedKvIncrementalSessionDestroy(session);
    UmiAiTrainingStackedDecoderDestroy(model);
    puts("Trained checkpoint invalidates all borrowed K/V state");
    return 0;
}
