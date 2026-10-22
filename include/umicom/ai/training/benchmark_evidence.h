/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/benchmark_evidence.h
 *
 * PURPOSE:
 *   Bind a reviewed completion report to canonical decoder checkpoint bytes,
 *   the ordered benchmark cases, sampler configuration and observed scores.
 *
 * OWNERSHIP AND TRUST:
 *   Reuse Framework SHA-256 and checkpoint services. Never execute generated
 *   text or change model weights. A digest identifies bytes; it does NOT
 *   authenticate their publisher or demonstrate source-code competence.
 *   Callers still approve the model and independent benchmark suite. Every
 *   rejected call preserves all caller-owned output buffers.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_BENCHMARK_EVIDENCE_H
#define UMICOM_AI_TRAINING_BENCHMARK_EVIDENCE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/benchmark.h"
#include "umicom/base/sha256.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_BENCHMARK_EVIDENCE_MAX_JSON_BYTES 24576U
#define UMI_AI_BENCHMARK_EVIDENCE_MAX_TITLE_BYTES 127U

typedef struct UmiAiTrainingBenchmarkEvidence {
    char modelCheckpointSha256[UMI_SHA256_HEX_CAPACITY];
    char benchmarkSuiteSha256[UMI_SHA256_HEX_CAPACITY];
    char scoredResultsSha256[UMI_SHA256_HEX_CAPACITY];
    uint64_t samplingSeed;
    uint32_t temperatureBits;
    size_t topK;
} UmiAiTrainingBenchmarkEvidence;

/* Canonical, domain-separated SHA-256 digests:
 * - modelCheckpointSha256: exact portable decoder checkpoint bytes.
 * - benchmarkSuiteSha256: ordered case labels, prompt/reference lengths
 *   and their raw binary data. NO file paths or parser formatting.
 * - scoredResultsSha256: score fields, reported steps and sampler settings.
 *
 * This verifies INTERNAL STRUCTURAL CONSISTENCY, not whether the supplied
 * scores were genuinely obtained from this model: use BenchmarkRun first.
 * No input bytes or generated source are written to disk or executed.
 * The caller serialises access to the decoder during checkpoint encoding.
 * The output remains unchanged on all failures. */
UmiStatus UmiAiTrainingBenchmarkEvidenceInspect(
    const UmiAiTrainingDecoder *model,
    const UmiAiTrainingBenchmarkCase *cases,
    const UmiAiTrainingBenchmarkCaseResult *results,
    size_t caseCount,
    const UmiAiTrainingBenchmarkReport *report,
    const UmiAiTrainingSamplingOptions *options,
    uint64_t seed,
    UmiAiTrainingBenchmarkEvidence *outEvidence);

/* Format a deterministic, UTF-8/ASCII JSON evidence record. Case labels are
 * escaped. All uint64 values are JSON strings to avoid JavaScript precision
 * loss. The buffer is NUL-terminated; outWritten excludes the terminator.
 * This is a read-only export, not a digital signature or certificate check.
 * The caller should pass an EvidenceInspect result matching these cases.
 * Failed calls leave BOTH output buffer and length unchanged. */
UmiStatus UmiAiTrainingBenchmarkEvidenceJson(
    const UmiAiTrainingBenchmarkCase *cases,
    const UmiAiTrainingBenchmarkCaseResult *results,
    size_t caseCount,
    const UmiAiTrainingBenchmarkReport *report,
    const UmiAiTrainingBenchmarkEvidence *evidence,
    char *outJson,
    size_t capacity,
    size_t *outWritten);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_BENCHMARK_EVIDENCE_H */
