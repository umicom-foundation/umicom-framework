/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/regression_compare.h
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
#ifndef UMICOM_FRONTEND_CONFORMANCE_REGRESSION_COMPARE_H
#define UMICOM_FRONTEND_CONFORMANCE_REGRESSION_COMPARE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"
#include "umicom/frontend/conformance/regression_baseline.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcRegressionComparison { double score_delta; int64_t blocker_delta; bool fingerprint_changed; bool regressed; } UmiFcRegressionComparison;
void umi_fc_regression_compare_run(const UmiFcRegressionBaseline *baseline,double current_score,size_t current_blockers,uint64_t current_fingerprint,double allowed_drop,UmiFcRegressionComparison *out_comparison);

#ifdef __cplusplus
}
#endif
#endif
