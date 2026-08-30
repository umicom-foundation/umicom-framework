/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/regression_baseline.h
 *
 * PURPOSE:
 *   stored score and semantic fingerprint baseline for a frontend release.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_REGRESSION_BASELINE_H
#define UMICOM_FRONTEND_CONFORMANCE_REGRESSION_BASELINE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcRegressionBaseline { char frontend_id[UMI_FC_ID_CAPACITY]; double score; size_t blockers; uint64_t semantic_fingerprint; uint64_t revision; } UmiFcRegressionBaseline;
UmiStatus umi_fc_regression_baseline_make(const char *frontend_id,double score,size_t blockers,uint64_t fingerprint,uint64_t revision,UmiFcRegressionBaseline *out_baseline);

#ifdef __cplusplus
}
#endif
#endif
