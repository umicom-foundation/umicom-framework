/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_position_admission_identity.c
 * PURPOSE: Prove that identical shapes and valid generation options do not
 *          permit a different checkpoint, initial seed or declared policy.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/position_admission.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d %s\n", __FILE__, __LINE__, #x); return 1; \
} } while (0)

int main(void)
{
    const UmiAiTrainingDecoderConfig configA = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(711)
    };
    const UmiAiTrainingDecoderConfig configB = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(712)
    };
    const UmiAiTrainingNucleusOptions sampling = {1.0F, 1U, 1.0F};
    const size_t prompt[] = {109U, 114U, 120U};
    const UmiAiTrainingPrefixBenchmarkPlan plan = {3U, 0U, 1U, 81U};
    UmiAiTrainingDecoder *first = NULL, *second = NULL;
    UmiAiTrainingPositionContract firstContract = {0};
    UmiAiTrainingPositionContract secondContract = {0};
    UmiAiTrainingPositionGenerationEvidence evidence;
    UmiAiTrainingPrefixBenchmarkResult benchmark;
    unsigned char unchangedEvidence[sizeof(evidence)];
    unsigned char unchangedBenchmark[sizeof(benchmark)];
    size_t tokens[4] = {71U, 72U, 73U, 74U};
    size_t tokenCopy[4] = {71U, 72U, 73U, 74U};
    size_t count = 101U;

    CHECK(UmiAiTrainingDecoderCreate(&configA, &first) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&configB, &second) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingPositionContractBuild(first,
          UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED,
          &firstContract) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingPositionContractBuild(second,
          UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED,
          &secondContract) == UMI_STATUS_OK);
    CHECK(memcmp(firstContract.checkpointSha256,
          secondContract.checkpointSha256, UMI_SHA256_BYTES) != 0);

    memset(&evidence, 0xC3, sizeof(evidence));
    memcpy(unchangedEvidence, &evidence, sizeof(evidence));
    memset(&benchmark, 0x9C, sizeof(benchmark));
    memcpy(unchangedBenchmark, &benchmark, sizeof(benchmark));

    CHECK(UmiAiTrainingPositionBoundGenerate(second, &firstContract,
          UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED,
          prompt, 3U, &sampling, 4U, 33U, tokens, 4U,
          &count, &evidence) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiAiTrainingPositionBoundBenchmark(second, &firstContract,
          UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED,
          prompt, 3U, &sampling, &plan,
          &benchmark) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(tokens, tokenCopy, sizeof(tokens)) == 0);
    CHECK(count == 101U);
    CHECK(memcmp(&evidence, unchangedEvidence, sizeof(evidence)) == 0);
    CHECK(memcmp(&benchmark, unchangedBenchmark, sizeof(benchmark)) == 0);

    /* An unknown position encoding cannot be relabelled as equivalent. */
    firstContract.positionEncoding = UINT32_C(2);
    CHECK(UmiAiTrainingPositionBoundGenerate(first, &firstContract,
          UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED,
          prompt, 3U, &sampling, 4U, 33U, tokens, 4U,
          &count, &evidence) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(tokens, tokenCopy, sizeof(tokens)) == 0);
    CHECK(count == 101U);
    CHECK(memcmp(&evidence, unchangedEvidence, sizeof(evidence)) == 0);
    UmiAiTrainingDecoderDestroy(second);
    UmiAiTrainingDecoderDestroy(first);
    puts("position admission: foreign checkpoint and strategy rejection PASS");
    return 0;
}
