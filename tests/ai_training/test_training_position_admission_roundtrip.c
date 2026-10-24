/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_position_admission_roundtrip.c
 * PURPOSE: Test bound checkpoint decode, both permitted generation contexts,
 *          model identity and opt-in benchmark composition on a real decoder.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/position_admission.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d %s\n", __FILE__, __LINE__, #x); return 1; \
} } while (0)

int main(void)
{
    const UmiAiTrainingDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(790080)
    };
    const UmiAiTrainingNucleusOptions sampling = {1.0F, 1U, 1.0F};
    const size_t prompt[] = {109U, 114U, 120U}; /* 'i', 'n', 't' */
    UmiAiTrainingDecoder *model = NULL, *loaded = NULL;
    UmiAiTrainingPositionContract sidecar = {0};
    UmiAiTrainingPositionGenerationEvidence evidence = {0};
    UmiAiTrainingPrefixBenchmarkResult benchmark = {0};
    UmiAiTrainingPrefixBenchmarkPlan plan = {3U, 0U, 1U, UINT64_C(776)};
    UmiAiTrainingGenerationCacheStats directStats = {0};
    uint8_t sidecarBytes[UMI_AI_POSITION_CONTRACT_RECORD_BYTES] = {0};
    uint8_t *checkpoint = NULL, *after = NULL;
    size_t checkpointSize = 0U, written = 0U, afterWritten = 0U;
    size_t sidecarWritten = 0U, admittedCount = 0U, directCount = 0U;
    size_t admitted[6] = {0}, direct[6] = {0};
    UmiStatus status;

    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointSize(model, &checkpointSize)
          == UMI_STATUS_OK);
    checkpoint = malloc(checkpointSize);
    after = malloc(checkpointSize);
    CHECK(checkpoint != NULL && after != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model,
        checkpoint, checkpointSize, &written) == UMI_STATUS_OK);
    CHECK(written == checkpointSize);

    /* Both context policies share weights but NOT token-position semantics. */
    const UmiAiTrainingPositionContext contexts[] = {
        UMI_AI_POSITION_CONTEXT_BOS_LEFT_PADDED,
        UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED
    };
    for (size_t k = 0U; k < 2U; ++k) {
        CHECK(UmiAiTrainingPositionContractBuild(model, contexts[k], &sidecar)
              == UMI_STATUS_OK);
        CHECK(UmiAiTrainingPositionContractEncode(&sidecar, sidecarBytes,
                  sizeof(sidecarBytes), &sidecarWritten) == UMI_STATUS_OK);
        CHECK(sidecarWritten == sizeof(sidecarBytes));
        loaded = NULL;
        CHECK(UmiAiTrainingPositionBoundCheckpointDecode(checkpoint,
              checkpointSize, sidecarBytes, sizeof(sidecarBytes), contexts[k],
              &loaded) == UMI_STATUS_OK);
        CHECK(loaded != NULL);
        CHECK(UmiAiTrainingPositionBoundGenerate(loaded, &sidecar, contexts[k],
              prompt, 3U, &sampling, 6U, UINT64_C(776),
              admitted, 6U, &admittedCount, &evidence) == UMI_STATUS_OK);
        CHECK(admittedCount == 6U && evidence.generatedTokens == 6U);
        CHECK(evidence.admittedContext == contexts[k]);
        CHECK(memcmp(evidence.checkpointSha256,
                     sidecar.checkpointSha256, UMI_SHA256_BYTES) == 0);
        if (contexts[k] == UMI_AI_POSITION_CONTEXT_BOS_LEFT_PADDED) {
            status = UmiAiTrainingDecoderGenerateNucleus(loaded,
                prompt, 3U, &sampling, 6U, UINT64_C(776),
                direct, 6U, &directCount);
            CHECK(evidence.cacheWorkAvailable == 0);
            CHECK(evidence.cache.computedRows == 0U);
            CHECK(evidence.cache.windowRebuilds == 0U);
            CHECK(evidence.cache.retainedTokens == 0U);
        } else {
            status = UmiAiTrainingDecoderGeneratePrefixNucleus(loaded,
                prompt, 3U, &sampling, 6U, UINT64_C(776),
                direct, 6U, &directCount, &directStats);
            CHECK(evidence.cacheWorkAvailable == 1);
            CHECK(evidence.cache.computedRows == directStats.computedRows);
            CHECK(evidence.cache.windowRebuilds == directStats.windowRebuilds);
            CHECK(UmiAiTrainingPositionBoundBenchmark(loaded, &sidecar,
                    contexts[k], prompt, 3U, &sampling, &plan, &benchmark)
                    == UMI_STATUS_OK);
            CHECK(benchmark.checkpointUnchanged == 1);
            CHECK(benchmark.verifiedPairs == 1U);
        }
        CHECK(status == UMI_STATUS_OK && directCount == admittedCount);
        CHECK(memcmp(admitted, direct, admittedCount * sizeof(size_t)) == 0);
        UmiAiTrainingDecoderDestroy(loaded);
        loaded = NULL;
    }
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, after,
        checkpointSize, &afterWritten) == UMI_STATUS_OK);
    CHECK(afterWritten == checkpointSize &&
          memcmp(checkpoint, after, checkpointSize) == 0);
    free(after);
    free(checkpoint);
    UmiAiTrainingDecoderDestroy(model);
    puts("position admission: bound load, both generators and benchmark PASS");
    return 0;
}
