/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_checkpoint.c
 * Purpose: Check portable checkpoint round trips, bounds, corruption and
 *          no-clobber file publication. No production model file is touched.
 * Author: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/checkpoint.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)
int main(void)
{
    const UmiAiTrainingDecoderConfig config = {4U, 2U, 4U, 8U, 2U, UINT64_C(13)};
    const char *const path = "test_umicom_training_checkpoint_fixture.umidec";
    UmiAiTrainingDecoder *a = NULL, *b = NULL, *c = NULL;
    size_t bytesRequired = 0U, bytesWritten = 0U, bytesWritten2 = 0U;
    uint8_t *bytes = NULL, *other = NULL;
    size_t tokens[2] = {0U,1U}, targets[2] = {1U,2U};
    float loss = 0.0F;
    UmiAiTrainingDecoderConfig info;
    uint64_t steps = 0U;
    CHECK(UmiAiTrainingDecoderCreate(&config, &a) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderTrainStep(a, tokens, targets, 2, 0.1F, &loss) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointSize(a, &bytesRequired) == UMI_STATUS_OK);
    CHECK(bytesRequired > 64U && bytesRequired < UMI_AI_DECODER_MAX_CHECKPOINT_BYTES);
    bytes = malloc(bytesRequired+1U);
    other = malloc(bytesRequired+1U);
    CHECK(bytes != NULL && other != NULL);
    memset(bytes, 0xA5, bytesRequired+1U);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(a,bytes,bytesRequired-1U,
                                                &bytesWritten) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(bytes[0] == 0xA5 && bytesWritten == 0U);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(a,bytes,bytesRequired,
                                                &bytesWritten) == UMI_STATUS_OK);
    CHECK(bytesWritten == bytesRequired && bytes[bytesRequired] == 0xA5);
    CHECK(UmiAiTrainingDecoderCheckpointDecode(bytes,bytesRequired,&b) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderInspect(b,&info,&steps) == UMI_STATUS_OK);
    CHECK(steps == 1U && info.initialSeed == config.initialSeed);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(b,other,bytesRequired,
                                                &bytesWritten2) == UMI_STATUS_OK);
    CHECK(bytesWritten2 == bytesRequired && memcmp(bytes,other,bytesRequired) == 0);
    UmiAiTrainingDecoderDestroy(b);
    b = NULL;
    bytes[bytesRequired / 2U] ^= 0x40U;
    CHECK(UmiAiTrainingDecoderCheckpointDecode(bytes,bytesRequired,&b) == UMI_STATUS_PARSE_ERROR);
    CHECK(b == NULL);
    bytes[bytesRequired / 2U] ^= 0x40U;
    CHECK(UmiAiTrainingDecoderCheckpointDecode(bytes,bytesRequired-1U,&b) == UMI_STATUS_PARSE_ERROR);
    CHECK(UmiAiTrainingDecoderCheckpointDecode(bytes,bytesRequired+1U,&b) == UMI_STATUS_PARSE_ERROR);
    CHECK(b == NULL);
    /* Idempotent CTest cleanup is restricted to our own uniquely named fixture
     * inside the build directory, never a user-selected model path. */
    (void)remove(path);
    CHECK(UmiAiTrainingDecoderCheckpointSaveNewFile(a,path) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointSaveNewFile(a,path) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(UmiAiTrainingDecoderCheckpointLoadFile(path,&c) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(c,other,bytesRequired,
                                                &bytesWritten2) == UMI_STATUS_OK);
    CHECK(memcmp(bytes,other,bytesRequired) == 0);
    /* Deterministic continuation: both models receive the same next batch
     * and learning rate, so their full parameter bytes must remain equal. */
    CHECK(UmiAiTrainingDecoderTrainStep(a,tokens,targets,2,0.1F,&loss) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderTrainStep(c,tokens,targets,2,0.1F,&loss) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(a,bytes,bytesRequired,
                                                &bytesWritten) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(c,other,bytesRequired,
                                                &bytesWritten2) == UMI_STATUS_OK);
    CHECK(memcmp(bytes,other,bytesRequired) == 0);
    CHECK(remove(path) == 0);
    UmiAiTrainingDecoderDestroy(c);
    UmiAiTrainingDecoderDestroy(a);
    free(bytes);
    free(other);
    puts("Checkpoint bytes and no-clobber file roundtrip: passed");
    return 0;
}
