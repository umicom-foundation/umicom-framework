/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/coverage_summary.h
 *
 * PURPOSE:
 *   required/optional semantic capability coverage summary for a renderer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_COVERAGE_SUMMARY_H
#define UMICOM_FRONTEND_CONFORMANCE_COVERAGE_SUMMARY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcCoverageSummary { size_t required_total; size_t required_covered; size_t optional_total; size_t optional_covered; double required_score; double total_score; } UmiFcCoverageSummary;
void umi_fc_coverage_summary_calculate(uint64_t required,uint64_t optional,uint64_t actual,UmiFcCoverageSummary *out_summary);

#ifdef __cplusplus
}
#endif
#endif
