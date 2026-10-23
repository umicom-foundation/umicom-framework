/*-----------------------------------------------------------------------------
 * Umicom Framework | deterministic machine-readable evidence regression.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * File: tests/ai_training/test_training_benchmark_evidence_json.c
 * PURPOSE: Verify deterministic JSON reporting of native benchmark evidence.
 * LICENCE: MIT
 */
#include "benchmark_evidence_fixture.h"
#include <string.h>
int main(void)
{
    UmiAiTrainingDecoder *model=NULL;
    UmiAiTrainingBenchmarkCase cases[2];
    UmiAiTrainingBenchmarkCaseResult scores[2];
    UmiAiTrainingBenchmarkReport report;
    UmiAiTrainingBenchmarkEvidence evidence;
    char json[UMI_AI_BENCHMARK_EVIDENCE_MAX_JSON_BYTES];
    char second[UMI_AI_BENCHMARK_EVIDENCE_MAX_JSON_BYTES];
    size_t written=999U,again=0U;
    Cases(cases);
    cases[0].title="Account \"record\" \\review";
    CHECK(CreateFixture(&model)==UMI_STATUS_OK);
    CHECK(EvidenceFixtureRun(model,cases,scores,&report,&evidence)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingBenchmarkEvidenceJson(cases,scores,2U,&report,&evidence,
          json,sizeof(json),&written)==UMI_STATUS_OK);
    CHECK(written>400U && written<sizeof(json));
    CHECK(json[written]=='\0');
    CHECK(strstr(json,"Account \\\"record\\\" \\\\review")!=NULL);
    CHECK(strstr(json,"\"model_checkpoint_sha256\"")!=NULL);
    CHECK(strstr(json,"\"exact_cases\"")!=NULL);
    CHECK(strstr(json,"\"completed_steps\": \"0\"")!=NULL);
    CHECK(strstr(json,"does not compile or execute C23")!=NULL);
    CHECK(UmiAiTrainingBenchmarkEvidenceJson(cases,scores,2U,&report,&evidence,
          second,sizeof(second),&again)==UMI_STATUS_OK);
    CHECK(again==written && strcmp(json,second)==0);
    UmiAiTrainingDecoderDestroy(model);
    return 0;
}
