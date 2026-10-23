/*-----------------------------------------------------------------------------
 * Umicom Framework | native benchmark evidence identity regression.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * File: tests/ai_training/test_training_benchmark_evidence_identity.c
 * PURPOSE: Verify stable identities for native benchmark evidence.
 * LICENCE: MIT
 */
#include "benchmark_evidence_fixture.h"
#include "umicom/base/sha256.h"
#include <stdlib.h>
int main(void)
{
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingBenchmarkCase cases[2];
    UmiAiTrainingBenchmarkCaseResult scores[2];
    UmiAiTrainingBenchmarkReport report;
    UmiAiTrainingBenchmarkEvidence first, second;
    uint8_t *encoded = NULL;
    size_t size=0U,written=0U;
    char hash[UMI_SHA256_HEX_CAPACITY];
    Cases(cases);
    CHECK(CreateFixture(&model)==UMI_STATUS_OK);
    CHECK(EvidenceFixtureRun(model,cases,scores,&report,&first)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingBenchmarkEvidenceInspect(model,cases,scores,2U,&report,
          &DEFAULT_OPTIONS,DEFAULT_SEED,&second)==UMI_STATUS_OK);
    CHECK(memcmp(&first,&second,sizeof(first))==0);
    CHECK(UmiAiTrainingDecoderCheckpointSize(model,&size)==UMI_STATUS_OK);
    CHECK(size>0U);
    encoded=malloc(size);
    CHECK(encoded!=NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model,encoded,size,&written)
          ==UMI_STATUS_OK);
    CHECK(written==size);
    CHECK(UmiSha256Buffer(encoded,written,hash)==UMI_STATUS_OK);
    CHECK(strcmp(hash,first.modelCheckpointSha256)==0);
    CHECK(strlen(first.benchmarkSuiteSha256)==64U);
    CHECK(strlen(first.scoredResultsSha256)==64U);
    CHECK(first.temperatureBits==UINT32_C(0x3f800000));
    CHECK(report.completedOptimiserSteps==0U);
    free(encoded);
    UmiAiTrainingDecoderDestroy(model);
    return 0;
}
