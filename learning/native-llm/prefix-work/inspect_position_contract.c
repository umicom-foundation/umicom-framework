/*-----------------------------------------------------------------------------
 * Umicom Framework — Native LLM Learning Laboratory
 * File: learning/native-llm/prefix-work/inspect_position_contract.c
 * PURPOSE: Demonstrate portable checkpoint-bound positional-encoding admission
 *          using the SAME decoder, SHA-256 and qualified prefix benchmark.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/position_contract.h"
#include "umicom/ai/training/prefix_benchmark.h"
#include "umicom/ai/training/byte_tokenizer.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    const UmiAiTrainingDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(20261024)
    };
    const size_t prompt[] = {109U, 114U, 120U}; /* int */
    const UmiAiTrainingNucleusOptions nucleus = {0.85F, 12U, 0.90F};
    const UmiAiTrainingPrefixBenchmarkPlan plan = {
        6U, 0U, 1U, UINT64_C(7801)
    };
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingPositionContract declared = {0}, parsed = {0};
    UmiAiTrainingPrefixBenchmarkResult result = {0};
    uint8_t sidecar[UMI_AI_POSITION_CONTRACT_RECORD_BYTES] = {0};
    char digest[UMI_SHA256_HEX_CAPACITY] = {0};
    size_t written = 0U;
    UmiStatus status;
    (void)argv;
    if (argc != 1) {
        fputs("usage: umicom-native-llm-position-contract\n", stderr);
        return 2;
    }
    status = UmiAiTrainingDecoderCreate(&config, &model);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingPositionContractBuild(model,
            UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED, &declared);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingPositionContractEncode(&declared,
            sidecar, sizeof(sidecar), &written);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingPositionContractDecode(sidecar, written, &parsed);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingPositionContractVerify(model, &parsed,
            UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED);
    if (status == UMI_STATUS_OK &&
        UmiAiTrainingPositionContractVerify(model, &parsed,
            UMI_AI_POSITION_CONTEXT_BOS_LEFT_PADDED) != UMI_STATUS_INVALID_STATE)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingPrefixBenchmarkRun(model, prompt, 3U,
            &nucleus, &plan, &result);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingPositionContractVerify(model, &parsed,
            UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED);
    if (status == UMI_STATUS_OK && !result.checkpointUnchanged)
        status = UMI_STATUS_INVALID_STATE;
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "position contract demonstration rejected (status=%d)\n",
                (int)status);
        UmiAiTrainingDecoderDestroy(model);
        return 1;
    }
    UmiSha256Hex(parsed.checkpointSha256, digest);
    printf("Umicom Native LLM — positional checkpoint compatibility\n"
           "Encoding: learned absolute positions\n"
           "Inference context: growing unpadded (explicit opt-in)\n"
           "Rollover policy: rebuild changed-position K/V\n"
           "Portable sidecar: %zu bytes\n"
           "Checkpoint content SHA-256: %s\n"
           "Completed optimiser steps: %" PRIu64 "\n"
           "Benchmark token parity: %zu/%zu\n"
           "Observed cache rows: %" PRIu64 "\n"
           "Checkpoint unchanged after benchmark: %s\n"
           "Legacy context rejected for this declaration: yes\n",
           written, digest, parsed.completedTrainingSteps,
           result.comparedTokens, result.comparedTokens,
           result.observedCache.computedRows,
           result.checkpointUnchanged ? "yes" : "no");
    puts("This sidecar is unsigned; SHA-256 is NOT a trusted publisher signature.");
    puts("No RoPE or relative-position inference has been enabled.");
    UmiAiTrainingDecoderDestroy(model);
    return 0;
}
