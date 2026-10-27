/* Umicom Framework | record executed work without inventing speedups. | MIT */
#include "stacked_incremental_fixture.h"

int main(void)
{
    const UmiAiTrainingStackedDecoderConfig cfg = IncrementalConfig(2U, 6U);
    const UmiAiTrainingNucleusOptions policy = {1.0F, 1U, 1.0F};
    const size_t prompt[2] = {41U, 42U};
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedKvIncrementalSession *session = NULL;
    UmiAiTrainingStackedKvIncrementalStats report = {0};
    VERIFY(UmiAiTrainingStackedDecoderCreate(&cfg, &model) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvIncrementalSessionCreate(model,
        prompt, 2U, &policy, UINT64_C(23), &session) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 7U; ++i) {
        size_t sample = 0U;
        VERIFY(UmiAiTrainingStackedKvIncrementalSessionNext(session,
            &sample) == UMI_STATUS_OK);
    }
    VERIFY(UmiAiTrainingStackedKvIncrementalSessionInspect(session,
        &report) == UMI_STATUS_OK);
    /* First trace: 6*2=12; then 4 prefix appends (2 layers each);
     * then two full-window rebuilds (12 each). Sampling's last emitted
     * token stays pending and does NOT cause a seventh state transition. */
    VERIFY(report.referenceFullContextTokenLayerRows == 84U);
    VERIFY(report.observedFullContextTokenLayerRows == 36U);
    VERIFY(report.observedIncrementalProjectionTokenLayerRows == 8U);
    VERIFY(report.observedFullWindowRebuilds == 2U);
    VERIFY(report.observedIncrementalAppends == 4U);
#if 0
    /* Historical multi-row attention used 72 token-layer query rows. */
    VERIFY(report.observedAttentionQueryTokenLayerRows == 72U);
#endif
    /* One forward query per layer for each accepted prefix append:
     * 36 full-context token-layer rows + 4*2 incremental query rows. */
    VERIFY(report.observedAttentionQueryTokenLayerRows == 44U);
    VERIFY(report.reservedKvBytes == 768U);
    UmiAiTrainingStackedKvIncrementalSessionDestroy(session);
    UmiAiTrainingStackedDecoderDestroy(model);
    puts("Projected K/V work and actual single-query attention reported separately");
    return 0;
}
