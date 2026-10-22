/*-----------------------------------------------------------------------------
 * Umicom Framework — native benchmark evidence regression fixtures.
 * Reuse canonical model creation, sampled inference and result validation.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_BENCHMARK_EVIDENCE_FIXTURE_H
#define UMICOM_AI_BENCHMARK_EVIDENCE_FIXTURE_H
#include "umicom/ai/training/benchmark.h"
#include "umicom/ai/training/benchmark_evidence.h"
#include "umicom/ai/training/checkpoint.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr,"check failed %s:%d: %s\n", __FILE__, __LINE__, #x); return 1; \
} } while (0)
static const uint8_t SOURCE_A[] = "Account";
static const uint8_t TARGET_A[] = "Record";
static const uint8_t SOURCE_B[] = "Ledger";
static const uint8_t TARGET_B[] = "Entry";
static const UmiAiTrainingSamplingOptions DEFAULT_OPTIONS = {1.0F, 1U};
static const uint64_t DEFAULT_SEED = UINT64_C(20261022);
static UmiStatus CreateFixture(UmiAiTrainingDecoder **outModel)
{
    const UmiAiTrainingDecoderConfig cfg = {260U, 3U, 4U, 8U, 2U, UINT64_C(20261022)};
    return UmiAiTrainingDecoderCreate(&cfg, outModel);
}
static void Cases(UmiAiTrainingBenchmarkCase outCases[2])
{
    outCases[0] = (UmiAiTrainingBenchmarkCase){
        "Account field", SOURCE_A, sizeof(SOURCE_A)-1U,
        TARGET_A, sizeof(TARGET_A)-1U};
    outCases[1] = (UmiAiTrainingBenchmarkCase){
        "Ledger entry", SOURCE_B, sizeof(SOURCE_B)-1U,
        TARGET_B, sizeof(TARGET_B)-1U};
}
static UmiStatus EvidenceFixtureRun(
    UmiAiTrainingDecoder *model, const UmiAiTrainingBenchmarkCase cases[2],
    UmiAiTrainingBenchmarkCaseResult results[2],
    UmiAiTrainingBenchmarkReport *report,
    UmiAiTrainingBenchmarkEvidence *evidence)
{
    UmiStatus st = UmiAiTrainingBenchmarkRun(model,cases,2U,&DEFAULT_OPTIONS,
                                            DEFAULT_SEED,results,2U,report);
    if(st!=UMI_STATUS_OK)return st;
    return UmiAiTrainingBenchmarkEvidenceInspect(
        model,cases,results,2U,report,&DEFAULT_OPTIONS,DEFAULT_SEED,evidence);
}
#endif
