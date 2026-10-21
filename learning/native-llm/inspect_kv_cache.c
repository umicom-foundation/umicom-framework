/*-----------------------------------------------------------------------------
 * Umicom Framework - Native AI learning laboratory
 * File: learning/native-llm/inspect_kv_cache.c
 * PURPOSE:
 *   Compare incremental key/value caching with reference full-context logits
 *   and report the work saved before absolute-position window rebasing.
 *   The mathematical operators and model are Framework-owned.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/kv_cache.h"
#include "umicom/ai/training/checkpoint.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

static int Inspect(UmiAiTrainingDecoder *model)
{
    UmiAiTrainingKvCache *cache = NULL;
    UmiAiTrainingDecoderConfig config;
    uint64_t trainingSteps = 0U;
    size_t history[UMI_AI_DECODER_MAX_CONTEXT] = {0};
    float cached[UMI_AI_DECODER_MAX_VOCABULARY];
    float reference[UMI_AI_DECODER_MAX_CONTEXT * UMI_AI_DECODER_MAX_VOCABULARY];
    const char *sample = "int accountBalance = 0;";
    UmiStatus status = UmiAiTrainingDecoderInspect(model, &config, &trainingSteps);
    if (status != UMI_STATUS_OK) return 1;
    status = UmiAiTrainingKvCacheCreate(model, &cache);
    if (status != UMI_STATUS_OK) return 1;
    printf("Model: %u context, %u channels, %u heads, %u vocabulary; "
           "completed optimiser steps=%llu\n", config.contextLength,
           config.channels, config.attentionHeads, config.vocabularySize,
           (unsigned long long)trainingSteps);
    for (size_t index = 0U; sample[index] != '\0'; ++index) {
        size_t retained = 0U;
        uint64_t computed = 0U, rebuilds = 0U;
        double maximumDifference = 0.0;
        size_t next = (size_t)(unsigned char)sample[index] + UMI_AI_BYTE_OFFSET;
        if (next >= (size_t)config.vocabularySize) {
            fprintf(stderr, "model vocabulary cannot represent the sample text\n");
            UmiAiTrainingKvCacheDestroy(cache);
            return 1;
        }
        const size_t context = (size_t)config.contextLength;
        if (index < context) {
            history[index] = next;
        } else {
            for (size_t shift = 1U; shift < context; ++shift)
                history[shift - 1U] = history[shift];
            history[context - 1U] = next;
        }
        const size_t populated = index + 1U < context ? index + 1U : context;
        status = UmiAiTrainingKvCacheAppend(cache, next, cached,
                                             (size_t)config.vocabularySize);
        if (status != UMI_STATUS_OK) break;
        for (size_t fill = populated; fill < context; ++fill)
            history[fill] = UMI_AI_BYTE_BOS;
        status = UmiAiTrainingDecoderLogits(model, history, context, reference,
            context * (size_t)config.vocabularySize);
        if (status != UMI_STATUS_OK) break;
        for (size_t col = 0U; col < (size_t)config.vocabularySize; ++col) {
            double delta = fabs((double)cached[col] -
                (double)reference[(populated - 1U) *
                    (size_t)config.vocabularySize + col]);
            if (delta > maximumDifference) maximumDifference = delta;
        }
        status = UmiAiTrainingKvCacheInspect(cache, &retained, &computed,
                                             &rebuilds);
        if (status != UMI_STATUS_OK) break;
        printf("token %2zu: retained=%2zu computedRows=%3llu "
               "rebases=%2llu maxLogitDelta=%g\n",
               index + 1U, retained, (unsigned long long)computed,
               (unsigned long long)rebuilds, maximumDifference);
        if (maximumDifference > 0.00004) {
            status = UMI_STATUS_INVALID_STATE;
            break;
        }
    }
    UmiAiTrainingKvCacheDestroy(cache);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "cache parity check failed (status=%d)\n", (int)status);
        return 1;
    }
    puts("Cache parity verified. This model uses absolute positions; "
         "a rolling full window must recompute its key/value rows.");
    return 0;
}

int main(int argc, char **argv)
{
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(20261021)
    };
    UmiStatus status;
    if (argc != 1 && argc != 2) {
        fprintf(stderr, "usage: umicom-native-llm-kv-inspect [approved-checkpoint]\n");
        return 2;
    }
    if (argc == 2) {
        status = UmiAiTrainingDecoderCheckpointLoadFile(argv[1], &model);
    } else {
        status = UmiAiTrainingDecoderCreate(&config, &model);
    }
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "model unavailable (status=%d)\n", (int)status);
        return 1;
    }
    int result = Inspect(model);
    UmiAiTrainingDecoderDestroy(model);
    return result;
}
