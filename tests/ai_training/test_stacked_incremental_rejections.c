/* Umicom Framework | atomic rejection of malformed requests and short buffers.
 * Existing model/output memory must remain unchanged. | MIT */
#include "stacked_incremental_fixture.h"

int main(void)
{
    const UmiAiTrainingStackedDecoderConfig cfg = IncrementalConfig(2U, 5U);
    const UmiAiTrainingNucleusOptions policy = {1.0F, 1U, 1.0F};
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedKvIncrementalSession *session = NULL;
    const size_t prompt[2] = {45U, 46U};
    size_t result = 98765U;
    size_t generated[3] = {123U, 123U, 123U};
    size_t written = 777U;
    UmiAiTrainingStackedKvIncrementalStats stats;
    memset(&stats, 0xa5, sizeof(stats));
    UmiAiTrainingStackedKvIncrementalStats previous = stats;
    VERIFY(UmiAiTrainingStackedDecoderCreate(&cfg, &model) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvIncrementalSessionCreate(model,
        prompt, 0U, &policy, 1U, &session) == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(session == NULL);
    const size_t invalidPrompt[2] = {45U, 5000U};
    VERIFY(UmiAiTrainingStackedKvIncrementalSessionCreate(model,
        invalidPrompt, 2U, &policy, 1U, &session) == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(session == NULL);
    VERIFY(UmiAiTrainingStackedKvIncrementalSessionCreate(model,
        prompt, 2U, &policy, 1U, &session) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvIncrementalSessionNext(session,
        NULL) == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(result == 98765U);
    VERIFY(UmiAiTrainingStackedKvIncrementalGenerate(model,
        prompt, 2U, &policy, 3U, 1U, generated, 2U, &written, &stats) ==
        UMI_STATUS_CAPACITY_EXCEEDED);
    VERIFY(generated[0] == 123U && generated[1] == 123U &&
        generated[2] == 123U && written == 777U &&
        memcmp(&previous, &stats, sizeof(stats)) == 0);
    UmiAiTrainingStackedKvIncrementalSessionDestroy(session);
    UmiAiTrainingStackedDecoderDestroy(model);
    puts("Malformed inputs preserve caller-owned outputs and session state");
    return 0;
}
