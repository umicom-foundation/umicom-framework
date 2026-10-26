/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_inference_identity.c
 *
 * PURPOSE:
 *   Identity regression
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "stacked_inference_fixture.h"
int main(void)
{
    UmiAiTrainingStackedDecoder *first = NULL, *second = NULL;
    UmiAiTrainingStackedModelIdentity identity = {0};
    CHECK(MakeModel(UINT64_C(19), &first) == UMI_STATUS_OK);
    CHECK(MakeModel(UINT64_C(20), &second) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedModelIdentityCapture(first, &identity) == UMI_STATUS_OK);
    CHECK(identity.checkpointBytes > 0U);
    CHECK(identity.config.transformerLayers == 2U);
    CHECK(UmiAiTrainingStackedModelIdentityVerify(first, &identity) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedModelIdentityVerify(second, &identity) == UMI_STATUS_INVALID_STATE);
    UmiAiTrainingStackedModelIdentity sentinel;
    memset(&sentinel, 0x5a, sizeof(sentinel));
    UmiAiTrainingStackedModelIdentity previous = sentinel;
    CHECK(UmiAiTrainingStackedModelIdentityCapture(NULL, &sentinel) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&sentinel, &previous, sizeof(sentinel)) == 0);
    UmiAiTrainingStackedDecoderDestroy(second);
    UmiAiTrainingStackedDecoderDestroy(first);
    puts("stacked model identity: exact checkpoint binding passed");
    return 0;
}
