/*-----------------------------------------------------------------------------
 * Umicom Framework — Native LLM Educational Laboratory
 * File: learning/native-llm/prefix-work/benchmark_prefix_generation.c
 * PURPOSE:
 *   Demonstrate measured, checkpoint-safe, seeded native prefix inference
 *   against the canonical decoder; no model weights or files are changed.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/prefix_benchmark.h"
#include "umicom/ai/training/byte_tokenizer.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    const UmiAiTrainingDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(20261024)
    };
    const UmiAiTrainingNucleusOptions policy = {0.85F, 12U, 0.9F};
    const UmiAiTrainingPrefixBenchmarkPlan plan = {
        12U, 1U, 5U, UINT64_C(7801)
    };
    /* 'int' uses the reversible byte tokenizer: each byte ID = byte + 4. */
    const size_t prompt[] = {109U, 114U, 120U};
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingPrefixBenchmarkResult report = {0};
    UmiStatus status;

    if (argc != 1) {
        if (argc == 2 && strcmp(argv[1], "--help") == 0) {
            puts("usage: umicom-native-llm-prefix-benchmark\n"
                 "Runs a read-only decoder parity and repeated timing experiment.");
            return 0;
        }
        fputs("usage: umicom-native-llm-prefix-benchmark [--help]\n", stderr);
        return 2;
    }
    status = UmiAiTrainingDecoderCreate(&config, &model);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingPrefixBenchmarkRun(model,
            prompt, sizeof prompt / sizeof prompt[0],
            &policy, &plan, &report);
    UmiAiTrainingDecoderDestroy(model);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "native decoder benchmark qualification refused: %d\n",
                (int)status);
        return 1;
    }

    printf("Umicom native prefix-generation benchmark (same decoder)\n"
           "parity-verified pairs: %zu (including %zu warmup)\n"
           "matching token comparisons: %zu\n"
           "output fingerprint (non-cryptographic): %016" PRIx64 "\n"
           "canonical reference rows per pair: %" PRIu64 "\n"
           "incremental cached rows per pair: %" PRIu64 "\n"
           "rows avoided per pair: %" PRIu64 "\n"
           "KV full-window rebuilds: %" PRIu64 "\n"
           "model checkpoint verified unchanged: %zu bytes\n",
           report.verifiedPairs, plan.warmupPairs,
           report.comparedTokens, report.tokenFingerprint,
           report.estimate.canonicalComputedRows,
           report.observedCache.computedRows,
           report.estimate.computedRowsAvoided,
           report.observedCache.windowRebuilds, report.checkpointBytes);

    if (report.wallClockAvailable) {
        printf("canonical wall seconds (min/median/max): %.6f / %.6f / %.6f\n"
               "KV session wall seconds (min/median/max): %.6f / %.6f / %.6f\n",
               report.canonical.minimumWallSeconds,
               report.canonical.medianWallSeconds,
               report.canonical.maximumWallSeconds,
               report.incremental.minimumWallSeconds,
               report.incremental.medianWallSeconds,
               report.incremental.maximumWallSeconds);
        puts(report.wallClockMonotonic ?
             "Wall clock: monotonic system timer." :
             "Wall clock: C23 fallback (not guaranteed monotonic).");
    } else {
        puts("Wall clock: unavailable or insufficient resolution.");
    }
    if (report.cpuClockAvailable) {
        printf("canonical median process CPU seconds: %.6f\n"
               "KV session median process CPU seconds: %.6f\n",
               report.canonical.medianProcessCpuSeconds,
               report.incremental.medianProcessCpuSeconds);
    } else {
        puts("Process CPU time: unavailable or insufficient resolution.");
    }
    puts("A smaller row count is NOT proof of faster end-to-end execution.\n"
         "Report the platform, compiler, build, load, repetitions and variance.\n"
         "This unpadded mode is NOT equivalent to historical BOS-left-padding.\n"
         "Do not use this educational model for financial decisions.");
    return 0;
}
