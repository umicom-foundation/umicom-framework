/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/regression_compare.c
 *
 * PURPOSE:
 *   score, blocker and fingerprint regression detection against a baseline.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/regression_compare.h"

void umi_fc_regression_compare_run(const UmiFcRegressionBaseline *baseline,double current_score,size_t current_blockers,uint64_t current_fingerprint,double allowed_drop,UmiFcRegressionComparison *out_comparison){if(out_comparison==NULL)return;*out_comparison=(UmiFcRegressionComparison){0};if(baseline==NULL){out_comparison->regressed=true;return;}out_comparison->score_delta=current_score-baseline->score;out_comparison->blocker_delta=(int64_t)current_blockers-(int64_t)baseline->blockers;out_comparison->fingerprint_changed=current_fingerprint!=baseline->semantic_fingerprint;out_comparison->regressed=out_comparison->score_delta < -allowed_drop || out_comparison->blocker_delta>0;}
