/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/gap_analysis.h
 *
 * PURPOSE:
 *   required, optional and quality gap analysis between canonical surface and renderer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_GAP_ANALYSIS_H
#define UMICOM_FRONTEND_CONFORMANCE_GAP_ANALYSIS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcGapAnalysis { uint64_t required_missing; uint64_t optional_missing; size_t required_count; size_t optional_count; double coverage; } UmiFcGapAnalysis;
void umi_fc_gap_analysis_run(uint64_t required,uint64_t optional,uint64_t actual,UmiFcGapAnalysis *out_analysis);

#ifdef __cplusplus
}
#endif
#endif
