/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/regression_compare.c
 *
 * PURPOSE:
 *   score, blocker and fingerprint regression detection against a baseline.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/regression_compare.h"

/*
 * Perform fc regression compare through the module contract so client applications do not
 * duplicate its policy.
 */
void umi_fc_regression_compare_run(const UmiFcRegressionBaseline *baseline,double current_score,size_t current_blockers,uint64_t current_fingerprint,double allowed_drop,UmiFcRegressionComparison *out_comparison){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_comparison==NULL)return;*out_comparison=(UmiFcRegressionComparison){0};/* Protect caller-owned memory by checking that required state is available before it is used. */ if(baseline==NULL){out_comparison->regressed=true;return;}out_comparison->score_delta=current_score-baseline->score;out_comparison->blocker_delta=(int64_t)current_blockers-(int64_t)baseline->blockers;out_comparison->fingerprint_changed=current_fingerprint!=baseline->semantic_fingerprint;out_comparison->regressed=out_comparison->score_delta < -allowed_drop || out_comparison->blocker_delta>0;}
