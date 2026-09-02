/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/conformance_report.h
 *
 * PURPOSE:
 *   aggregate result collection, counts and score calculation for one frontend run.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_CONFORMANCE_REPORT_H
#define UMICOM_FRONTEND_CONFORMANCE_CONFORMANCE_REPORT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"
#include "umicom/frontend/conformance/conformance_result.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc conformance report data shared with callers of this public contract.
 */
typedef struct UmiFcConformanceReport { UmiFcConformanceResult items[UMI_FC_MAX_ITEMS]; size_t count; size_t passed; size_t degraded; size_t failed; } UmiFcConformanceReport;
/**
 * Initialise fc conformance report from caller-provided values so later operations receive
 * a known state.
 */
void umi_fc_conformance_report_init(UmiFcConformanceReport *report);
/**
 * Add fc conformance report only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_fc_conformance_report_add(UmiFcConformanceReport *report,const UmiFcConformanceResult *result);
/**
 * Provide the fc conformance report score operation used by this module and its client
 * applications.
 */
double umi_fc_conformance_report_score(const UmiFcConformanceReport *report);

#ifdef __cplusplus
}
#endif
#endif
