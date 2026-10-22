/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/benchmark_suite.h
 *
 * PURPOSE:
 *   Load a bounded, explicitly reviewed byte-completion benchmark suite.
 *   The suite is data; it cannot launch a compiler, script, subprocess or
 *   execute generated source code.
 *
 * SECURITY AND OWNERSHIP:
 *   Local paths are chosen by a trusted caller and are not a filesystem
 *   sandbox (symlinks can escape). An opaque suite owns its prompt/reference
 *   buffers. Case pointers are BORROWED until the suite is destroyed.
 *   Loading and parsing are failure-atomic. Validate source licences and
 *   training/benchmark separation before accepting a suite.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_BENCHMARK_SUITE_H
#define UMICOM_AI_TRAINING_BENCHMARK_SUITE_H

#include <stddef.h>
#include "umicom/ai/training/benchmark.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_BENCHMARK_MAX_SUITE_BYTES 600000U
#define UMI_AI_BENCHMARK_MAX_TITLE_BYTES 63U

typedef struct UmiAiTrainingBenchmarkSuite UmiAiTrainingBenchmarkSuite;

/* Strict, non-executable UTF-8 ASCII subset, LF or CRLF, no BOM:
 *
 *   UMICOM-AI-BENCHMARK-1
 *   # Comment lines begin with a literal # in column zero.
 *   case sample_account 74797065 20202020
 *
 * Each case is FOUR whitespace-separated tokens: 'case', a unique identifier
 * made of [A-Za-z0-9_.-], hex-encoded prompt bytes (or '-' for empty), and
 * hex-encoded EXPECTED bytes. Every expected value must have 1..256 bytes.
 * Prompts have 0..4096 bytes. Hex pairs preserve arbitrary binary source and
 * disallow ambiguous backslash escapes. No environment or shell expansion.
 * At least one and at most 64 cases. No executable or reference-code imports.
 * Rejected inputs leave *outSuite unchanged; caller owns a successful suite. */
UmiStatus UmiAiTrainingBenchmarkSuiteParse(
    const char *text, size_t textLength,
    UmiAiTrainingBenchmarkSuite **outSuite);

/* Read one explicitly approved local file with the same grammar and limit.
 * Not a path-security sandbox: caller verifies file provenance separately. */
UmiStatus UmiAiTrainingBenchmarkSuiteLoadFile(
    const char *filePath, UmiAiTrainingBenchmarkSuite **outSuite);

/* Borrow an indexed case until suite destruction. This does not expose mutable
 * storage or modify the caller-owned destination on failure. */
UmiStatus UmiAiTrainingBenchmarkSuiteCase(
    const UmiAiTrainingBenchmarkSuite *suite, size_t index,
    UmiAiTrainingBenchmarkCase *outCase);
UmiStatus UmiAiTrainingBenchmarkSuiteCount(
    const UmiAiTrainingBenchmarkSuite *suite, size_t *outCount);
void UmiAiTrainingBenchmarkSuiteDestroy(UmiAiTrainingBenchmarkSuite *suite);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_BENCHMARK_SUITE_H */
