/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/regression_baseline.c
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
#include "umicom/frontend/conformance/regression_baseline.h"

UmiStatus umi_fc_regression_baseline_make(const char *frontend_id,double score,size_t blockers,uint64_t fingerprint,uint64_t revision,UmiFcRegressionBaseline *out_baseline){UmiStatus st;if(out_baseline==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_baseline=(UmiFcRegressionBaseline){0};st=umi_fc_copy_text(out_baseline->frontend_id,sizeof(out_baseline->frontend_id),frontend_id);if(st!=UMI_STATUS_OK)return st;out_baseline->score=umi_fc_clamp_score(score);out_baseline->blockers=blockers;out_baseline->semantic_fingerprint=fingerprint;out_baseline->revision=revision;return UMI_STATUS_OK;}
