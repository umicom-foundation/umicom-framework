/*-----------------------------------------------------------------------------
 * Umicom Framework | model and reviewed-suite change detection.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "benchmark_evidence_fixture.h"
int main(void)
{
    UmiAiTrainingDecoder *model=NULL;
    UmiAiTrainingBenchmarkCase cases[2];
    UmiAiTrainingBenchmarkCaseResult scores[2];
    UmiAiTrainingBenchmarkReport report;
    UmiAiTrainingBenchmarkEvidence original, changedSuite, updated;
    const uint8_t different[]="Change";
    size_t input[]={8U,9U,10U},target[]={9U,10U,11U};
    float loss=0.0F;
    Cases(cases);
    CHECK(CreateFixture(&model)==UMI_STATUS_OK);
    CHECK(EvidenceFixtureRun(model,cases,scores,&report,&original)==UMI_STATUS_OK);
    cases[0].expectedBytes=different;
    cases[0].expectedLength=sizeof(different)-1U;
    CHECK(EvidenceFixtureRun(model,cases,scores,&report,&changedSuite)==UMI_STATUS_OK);
    CHECK(strcmp(changedSuite.modelCheckpointSha256,original.modelCheckpointSha256)==0);
    CHECK(strcmp(changedSuite.benchmarkSuiteSha256,original.benchmarkSuiteSha256)!=0);
    CHECK(UmiAiTrainingDecoderTrainStep(model,input,target,3U,0.01F,&loss)
          ==UMI_STATUS_OK);
    CHECK(EvidenceFixtureRun(model,cases,scores,&report,&updated)==UMI_STATUS_OK);
    CHECK(strcmp(updated.modelCheckpointSha256,changedSuite.modelCheckpointSha256)!=0);
    CHECK(strcmp(updated.benchmarkSuiteSha256,changedSuite.benchmarkSuiteSha256)==0);
    CHECK(report.completedOptimiserSteps==1U);
    UmiAiTrainingDecoderDestroy(model);
    return 0;
}
