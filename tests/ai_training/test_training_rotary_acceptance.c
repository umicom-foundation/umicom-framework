/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_rotary_acceptance.c
 *
 * PURPOSE:
 *   Test the independent evidence interpretation contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_rotary_acceptance.c
 * PURPOSE: Test the independent evidence interpretation contract.
 * NOTE: Synthetic fixture observations test REJECTION LOGIC ONLY. These
 * are not evidence that the production rotary tape passed.
 * AUTHOR: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_acceptance.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do {if (!(x)) {fprintf(stderr,"line %d: %s\n", __LINE__, #x); return 1;}} while(0)
int main(void) {
    UmiAiTrainingRotaryAcceptanceObservation items[3] = {
        {0U,1,1,72U,0.0002},{1U,1,1,6U,0.0},{2U,1,1,48U,0.0}
    };
    UmiAiTrainingRotaryAcceptanceReport report = {0};
    UmiAiTrainingRotaryAcceptanceReport unchanged;
    CHECK(UmiAiTrainingRotaryAcceptanceEvaluate(items,3U,0.0008,&report)==UMI_STATUS_OK);
    CHECK(report.accepted==1 && report.executedTests==3U && report.passedTests==3U);
    CHECK(report.totalChecks==126U && fabs(report.maximumGradientError-0.0002)<1.e-12);
    items[0].maximumAbsoluteError=0.001;
    CHECK(UmiAiTrainingRotaryAcceptanceEvaluate(items,3U,0.0008,&report)==UMI_STATUS_OK);
    CHECK(report.accepted==0);
    items[0].maximumAbsoluteError=0.0002;
    items[1].passed=0;
    CHECK(UmiAiTrainingRotaryAcceptanceEvaluate(items,3U,0.0008,&report)==UMI_STATUS_OK);
    CHECK(report.accepted==0 && report.passedTests==2U);
    items[1].passed=1;items[1].executed=0;items[1].checks=0;
    unchanged=report;
    CHECK(UmiAiTrainingRotaryAcceptanceEvaluate(items,3U,0.0008,&report)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&report,&unchanged,sizeof(report))==0);
    items[1].passed=0;
    CHECK(UmiAiTrainingRotaryAcceptanceEvaluate(items,3U,0.0008,&report)==UMI_STATUS_OK);
    CHECK(report.accepted==0 && report.executedTests==2U);
    items[1].executed=1;items[1].passed=1;items[1].checks=6;
    items[2].testId=1U;unchanged=report;
    CHECK(UmiAiTrainingRotaryAcceptanceEvaluate(items,3U,0.0008,&report)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&report,&unchanged,sizeof(report))==0);
    items[2].testId=2U;items[2].maximumAbsoluteError=NAN;
    CHECK(UmiAiTrainingRotaryAcceptanceEvaluate(items,3U,0.0008,&report)==UMI_STATUS_INVALID_ARGUMENT);
    items[2].maximumAbsoluteError=0.0;
    CHECK(UmiAiTrainingRotaryAcceptanceEvaluate(items,3U,NAN,&report)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingRotaryAcceptanceEvaluate(items,3U,0.02,&report)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingRotaryAcceptanceEvaluate(items,2U,0.0008,&report)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingRotaryAcceptanceEvaluate(items,3U,0.0008,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    puts("rotary acceptance evidence interpretation: PASS (fixture-only)");return 0;
}
