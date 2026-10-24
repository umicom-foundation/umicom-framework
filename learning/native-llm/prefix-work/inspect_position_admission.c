/*-----------------------------------------------------------------------------
 * Umicom Framework — Native LLM Learning Laboratory
 * File: learning/native-llm/prefix-work/inspect_position_admission.c
 * PURPOSE: Demonstrate checkpoint-bound loading and explicit policy-admitted
 *          generation using the unchanged native decoder and KV cache.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/position_admission.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    const UmiAiTrainingDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(20261024)
    };
    const UmiAiTrainingNucleusOptions policy = {1.0F, 1U, 1.0F};
    const UmiAiTrainingPrefixBenchmarkPlan plan = {3U, 0U, 1U, UINT64_C(79)};
    const size_t prompt[] = {109U, 114U, 120U}; /* 'i', 'n', 't' */
    UmiAiTrainingDecoder *model = NULL, *restored = NULL;
    UmiAiTrainingPositionContract contract = {0};
    UmiAiTrainingPositionGenerationEvidence evidence = {0};
    UmiAiTrainingPrefixBenchmarkResult result = {0};
    uint8_t sidecar[UMI_AI_POSITION_CONTRACT_RECORD_BYTES] = {0};
    uint8_t *checkpoint = NULL;
    size_t checkpointSize = 0U, count = 0U, written = 0U, recordBytes = 0U;
    size_t tokens[6] = {0};
    UmiStatus status = UMI_STATUS_OK;
    (void)argv;
    if (argc != 1) {
        fputs("usage: umicom-native-llm-position-admission\n", stderr);
        return 2;
    }
    status = UmiAiTrainingDecoderCreate(&config, &model);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingPositionContractBuild(model,
            UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED, &contract);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingPositionContractEncode(&contract,
            sidecar, sizeof(sidecar), &recordBytes);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingDecoderCheckpointSize(model, &checkpointSize);
    if (status == UMI_STATUS_OK) {
        checkpoint = malloc(checkpointSize);
        if (checkpoint == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingDecoderCheckpointEncode(model, checkpoint,
            checkpointSize, &written);
    if (status == UMI_STATUS_OK && written != checkpointSize)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingPositionBoundCheckpointDecode(
            checkpoint, checkpointSize, sidecar, recordBytes,
            UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED, &restored);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingPositionBoundGenerate(restored, &contract,
            UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED,
            prompt, 3U, &policy, 6U, UINT64_C(79),
            tokens, 6U, &count, &evidence);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingPositionBoundBenchmark(restored, &contract,
            UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED,
            prompt, 3U, &policy, &plan, &result);

    if (status == UMI_STATUS_OK) {
        char digestHex[UMI_SHA256_HEX_CAPACITY] = {0};
        UmiSha256Hex(contract.checkpointSha256, digestHex);
        printf("Umicom native checkpoint-admitted generation\n"
               "Position policy: learned absolute, growing unpadded\n"
               "Checkpoint SHA-256: %s\n"
               "Generated tokens: %zu\n"
               "Cache computed rows: %" PRIu64 "\n"
               "Context rebuilds: %" PRIu64 "\n"
               "Paired benchmark checks: %zu\n"
               "Model checkpoint unchanged: %s\n",
               digestHex, count, evidence.cache.computedRows,
               evidence.cache.windowRebuilds, result.verifiedPairs,
               result.checkpointUnchanged ? "yes" : "no");
    } else {
        fprintf(stderr, "native positional admission rejected (status=%d)\n",
                (int)status);
    }
    UmiAiTrainingDecoderDestroy(restored);
    UmiAiTrainingDecoderDestroy(model);
    free(checkpoint);
    return status == UMI_STATUS_OK ? 0 : 1;
}
