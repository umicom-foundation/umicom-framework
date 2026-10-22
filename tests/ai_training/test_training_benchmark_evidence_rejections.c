/*-----------------------------------------------------------------------------
 * Umicom Framework | failure-atomic evidence and JSON rejections.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "benchmark_evidence_fixture.h"
#include <string.h>
int main(void)
{
    UmiAiTrainingDecoder *model=NULL;
    UmiAiTrainingBenchmarkCase cases[2];
    UmiAiTrainingBenchmarkCaseResult scores[2];
    UmiAiTrainingBenchmarkReport report,modified;
    UmiAiTrainingBenchmarkEvidence evidence,old;
    char buffer[UMI_AI_BENCHMARK_EVIDENCE_MAX_JSON_BYTES];
    size_t written=123U;
    Cases(cases);
    CHECK(CreateFixture(&model)==UMI_STATUS_OK);
    CHECK(EvidenceFixtureRun(model,cases,scores,&report,&evidence)==UMI_STATUS_OK);
    old=evidence;
    modified=report;
    modified.matchingPositionBytes++;
    CHECK(UmiAiTrainingBenchmarkEvidenceInspect(model,cases,scores,2U,&modified,
          &DEFAULT_OPTIONS,DEFAULT_SEED,&evidence)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&old,&evidence,sizeof(old))==0);
    modified=report;
    modified.completedOptimiserSteps++;
    CHECK(UmiAiTrainingBenchmarkEvidenceInspect(model,cases,scores,2U,&modified,
          &DEFAULT_OPTIONS,DEFAULT_SEED,&evidence)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&old,&evidence,sizeof(old))==0);
    cases[0].title="Invalid\nlabel";
    CHECK(UmiAiTrainingBenchmarkEvidenceInspect(model,cases,scores,2U,&report,
          &DEFAULT_OPTIONS,DEFAULT_SEED,&evidence)==UMI_STATUS_INVALID_ARGUMENT);
    Cases(cases);
    memset(buffer,'?',sizeof(buffer));
    CHECK(UmiAiTrainingBenchmarkEvidenceJson(cases,scores,2U,&report,&evidence,
          buffer,8U,&written)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(buffer[0]=='?' && buffer[8]=='?' && written==123U);
    old=evidence;
    evidence.modelCheckpointSha256[0]='x';
    CHECK(UmiAiTrainingBenchmarkEvidenceJson(cases,scores,2U,&report,&evidence,
          buffer,sizeof(buffer),&written)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(buffer[0]=='?' && written==123U);
    evidence=old;
    CHECK(UmiAiTrainingBenchmarkEvidenceJson(cases,scores,2U,&report,&evidence,
          buffer,sizeof(buffer),&written)==UMI_STATUS_OK);
    UmiAiTrainingDecoderDestroy(model);
    return 0;
}
