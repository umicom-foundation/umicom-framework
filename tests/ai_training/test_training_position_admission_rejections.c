/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_position_admission_rejections.c
 * PURPOSE: Reject mismatched identity, truncated/altered model bytes, wrong
 *          generation context and invalid output buffers without publication.
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
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(79)
    };
    const UmiAiTrainingNucleusOptions sampling = {1.0F, 1U, 1.0F};
    const size_t prompt[] = {109U, 114U, 120U};
    const size_t badPrompt[] = {UMI_AI_BYTE_VOCABULARY};
    UmiAiTrainingDecoder *model = NULL, *sentinel = NULL;
    UmiAiTrainingPositionContract approved = {0};
    UmiAiTrainingPositionGenerationEvidence evidence;
    UmiAiTrainingPrefixBenchmarkResult result;
    const UmiAiTrainingPrefixBenchmarkPlan plan = {3U, 0U, 1U, 55U};
    uint8_t record[UMI_AI_POSITION_CONTRACT_RECORD_BYTES];
    uint8_t *checkpoint = NULL;
    size_t checkpointSize = 0U, written = 0U, sidecarWritten = 0U;
    size_t ids[4], originals[4], count;
    unsigned char evidenceBefore[sizeof(evidence)];
    unsigned char resultBefore[sizeof(result)];
    UmiStatus status;

    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingPositionContractBuild(model,
          UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED, &approved) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingPositionContractEncode(&approved, record,
          sizeof(record), &sidecarWritten) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointSize(model, &checkpointSize)
          == UMI_STATUS_OK);
    checkpoint = malloc(checkpointSize);
    CHECK(checkpoint != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, checkpoint,
          checkpointSize, &written) == UMI_STATUS_OK);
    CHECK(written == checkpointSize);

    /* Existing owned handle is a sentinel; rejected loads never replace it. */
    CHECK(UmiAiTrainingDecoderCreate(&config, &sentinel) == UMI_STATUS_OK);
    UmiAiTrainingDecoder *unchanged = sentinel;
    CHECK(UmiAiTrainingPositionBoundCheckpointDecode(checkpoint,
          checkpointSize - 1U, record, sizeof(record),
          UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED,
          &unchanged) != UMI_STATUS_OK);
    CHECK(unchanged == sentinel);
    CHECK(UmiAiTrainingPositionBoundCheckpointDecode(checkpoint,
          checkpointSize, record, sizeof(record) - 1U,
          UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED,
          &unchanged) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(unchanged == sentinel);
    CHECK(UmiAiTrainingPositionBoundCheckpointDecode(checkpoint,
          checkpointSize, record, sizeof(record),
          UMI_AI_POSITION_CONTEXT_BOS_LEFT_PADDED,
          &unchanged) == UMI_STATUS_INVALID_STATE);
    CHECK(unchanged == sentinel);

    checkpoint[checkpointSize - 1U] ^= UINT8_C(0x40);
    CHECK(UmiAiTrainingPositionBoundCheckpointDecode(checkpoint,
          checkpointSize, record, sizeof(record),
          UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED,
          &unchanged) == UMI_STATUS_INVALID_STATE);
    CHECK(unchanged == sentinel);
    checkpoint[checkpointSize - 1U] ^= UINT8_C(0x40);
    record[96U] ^= UINT8_C(0x80);
    CHECK(UmiAiTrainingPositionBoundCheckpointDecode(checkpoint,
          checkpointSize, record, sizeof(record),
          UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED,
          &unchanged) != UMI_STATUS_OK);
    CHECK(unchanged == sentinel);
    record[96U] ^= UINT8_C(0x80);

    ids[0] = 77U; ids[1] = 88U; ids[2] = 99U; ids[3] = 111U;
    memcpy(originals, ids, sizeof(ids));
    count = 4242U;
    memset(&evidence, 0xA5, sizeof(evidence));
    memcpy(evidenceBefore, &evidence, sizeof(evidence));
    memset(&result, 0x5A, sizeof(result));
    memcpy(resultBefore, &result, sizeof(result));

    /* Wrong policy, too-small capacity and invalid prompt must not mutate. */
    status = UmiAiTrainingPositionBoundGenerate(model, &approved,
        UMI_AI_POSITION_CONTEXT_BOS_LEFT_PADDED, prompt, 3U, &sampling,
        4U, 1U, ids, 4U, &count, &evidence);
    CHECK(status == UMI_STATUS_INVALID_STATE);
    status = UmiAiTrainingPositionBoundGenerate(model, &approved,
        UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED, prompt, 3U, &sampling,
        4U, 1U, ids, 3U, &count, &evidence);
    CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED);
    status = UmiAiTrainingPositionBoundGenerate(model, &approved,
        UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED, badPrompt, 1U,
        &sampling, 4U, 1U, ids, 4U, &count, &evidence);
    CHECK(status != UMI_STATUS_OK);
    CHECK(UmiAiTrainingPositionBoundGenerate(model, &approved,
        UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED, prompt, 3U,
        &sampling, 0U, 1U, ids, 4U, &count, &evidence)
        == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(ids, originals, sizeof(ids)) == 0);
    CHECK(count == 4242U);
    CHECK(memcmp(&evidence, evidenceBefore, sizeof(evidence)) == 0);

    CHECK(UmiAiTrainingPositionBoundBenchmark(model, &approved,
         UMI_AI_POSITION_CONTEXT_BOS_LEFT_PADDED, prompt, 3U,
         &sampling, &plan, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingPositionBoundBenchmark(model, &approved,
         UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED, NULL, 3U,
         &sampling, &plan, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&result, resultBefore, sizeof(result)) == 0);
    free(checkpoint);
    UmiAiTrainingDecoderDestroy(sentinel);
    UmiAiTrainingDecoderDestroy(model);
    puts("position admission: invalid loads and output preservation PASS");
    return 0;
}
