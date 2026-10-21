/*-----------------------------------------------------------------------------
 * Umicom Framework — Native LLM Learning Laboratory
 * File: learning/native-llm/benchmark_matrix.c
 *
 * PURPOSE:
 *   Measure and compare the shared portable matrix reference and tiled paths.
 *   The Framework owns all computation; this application reports timings only.
 *
 * MEASUREMENT NOTES:
 *   C23 clock() observes process CPU time, not wall-clock time. Different
 *   machines, compiler optimisations, process contention and small input
 *   sizes change the results. No speedup is assumed or asserted by tests.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/matrix_kernel.h"

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static volatile double measurementSink = 0.0;

static int RunCase(size_t rows, size_t inner, size_t columns,
                   size_t iterations)
{
    float left[UMI_AI_MATRIX_MAX_ELEMENTS];
    float right[UMI_AI_MATRIX_MAX_ELEMENTS];
    float reference[UMI_AI_MATRIX_MAX_ELEMENTS];
    float tiled[UMI_AI_MATRIX_MAX_ELEMENTS];
    UmiAiTrainingMatrixEstimate estimate;
    UmiStatus status;
    clock_t start, finish;
    double referenceSeconds, tiledSeconds;

    status = UmiAiTrainingMatrixEstimateWork(rows, inner, columns, &estimate);
    if (status != UMI_STATUS_OK) return 1;
    for (size_t i = 0U; i < rows * inner; ++i)
        left[i] = (float)((int)(i % 11U) - 5) * 0.0625F;
    for (size_t i = 0U; i < inner * columns; ++i)
        right[i] = (float)((int)(i % 17U) - 8) * 0.0625F;

    /* Compare exact numerical outputs BEFORE recording any timing. */
    status = UmiAiTrainingMatrixMultiply(left, rows, inner, right, columns,
        UMI_AI_MATRIX_REFERENCE, reference, rows * columns);
    if (status != UMI_STATUS_OK) return 1;
    status = UmiAiTrainingMatrixMultiply(left, rows, inner, right, columns,
        UMI_AI_MATRIX_TILED, tiled, rows * columns);
    if (status != UMI_STATUS_OK ||
        memcmp(reference, tiled, rows * columns * sizeof(float)) != 0)
        return 1;

    start = clock();
    for (size_t i = 0U; i < iterations; ++i) {
        if (UmiAiTrainingMatrixMultiply(left, rows, inner, right, columns,
            UMI_AI_MATRIX_REFERENCE, reference, rows * columns) != UMI_STATUS_OK)
            return 1;
        measurementSink += (double)reference[i % (rows * columns)];
    }
    finish = clock();
    if (start == (clock_t)-1 || finish == (clock_t)-1 || finish < start)
        return 1;
    referenceSeconds = (double)(finish - start) / (double)CLOCKS_PER_SEC;

    start = clock();
    for (size_t i = 0U; i < iterations; ++i) {
        if (UmiAiTrainingMatrixMultiply(left, rows, inner, right, columns,
            UMI_AI_MATRIX_TILED, tiled, rows * columns) != UMI_STATUS_OK)
            return 1;
        measurementSink += (double)tiled[i % (rows * columns)];
    }
    finish = clock();
    if (start == (clock_t)-1 || finish == (clock_t)-1 || finish < start)
        return 1;
    tiledSeconds = (double)(finish - start) / (double)CLOCKS_PER_SEC;
    printf("[rows=%zu, inner=%zu, columns=%zu] repetitions=%zu\n",
           rows, inner, columns, iterations);
    printf("  estimated multiply-adds/run=%" PRIu64
           ", nominal FP operations/run=%" PRIu64 "\n",
           estimate.multiplyAccumulateCount,
           estimate.nominalFloatingOperations);
    printf("  logical bytes: input=%" PRIu64 ", weights=%" PRIu64
           ", output=%" PRIu64 ", staged=%" PRIu64 "\n",
           estimate.inputBytes, estimate.weightBytes,
           estimate.outputBytes, estimate.temporaryBytes);
    printf("  reference CPU seconds=%.6f; tiled CPU seconds=%.6f\n",
           referenceSeconds, tiledSeconds);
    if (referenceSeconds > 0.0 && tiledSeconds > 0.0) {
        printf("  observed reference/tiled time ratio=%.3f (not guaranteed)\n",
               referenceSeconds / tiledSeconds);
    }
    return 0;
}

int main(int argc, char **argv)
{
    size_t iterations = 1000U;
    if (argc > 2) {
        fputs("usage: umicom-native-llm-matrix-benchmark [iterations 1..10000]\n",
              stderr);
        return 2;
    }
    if (argc == 2) {
        char *end = NULL;
        unsigned long value;
        if (argv[1][0] == '-') return 2;
        errno = 0;
        value = strtoul(argv[1], &end, 10);
        if (errno != 0 || end == argv[1] || *end != '\0' ||
            value < 1UL || value > 10000UL) return 2;
        iterations = (size_t)value;
    }
    puts("Umicom Native AI: qualified C23 matrix projection measurements");
    puts("These are CPU-time observations, NOT promises of faster model inference.");
    if (RunCase(1U, 8U, 260U, iterations) != 0 ||
        RunCase(8U, 16U, 64U, iterations) != 0) return 1;
    if (measurementSink != measurementSink) return 1;
    return 0;
}
