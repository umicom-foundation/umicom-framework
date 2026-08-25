/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_score.c
 *
 * PURPOSE:
 *   Focused regression coverage for weighted percentage scoring with blocker-aware grade classification.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/score.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    CHECK(umi_fc_score_weighted(9.0,10.0)>0.89); CHECK(umi_fc_score_grade(0.96,0U)==UMI_FC_GRADE_A); CHECK(umi_fc_score_grade(1.0,1U)==UMI_FC_GRADE_FAIL);
    return 0;
}
