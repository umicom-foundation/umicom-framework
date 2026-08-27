/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/acceptance/report.h
 *
 * PURPOSE:
 *   Summarise multiple acceptance gate results without changing capability maturity metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEVELOPER_ACCEPTANCE_REPORT_H
#define UMICOM_DEVELOPER_ACCEPTANCE_REPORT_H

#include "umicom/developer/acceptance/gate.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeveloperAcceptanceReport {
    size_t gate_count;
    size_t passed_gate_count;
    size_t failed_gate_count;
    size_t missing_requirement_count;
    unsigned completion_percent;
} UmiDeveloperAcceptanceReport;

/*
 * Aggregate focused gate results into a compact completion report. A zero-gate
 * report is valid and reports zero percent instead of inventing completeness.
 */
UmiStatus umi_developer_acceptance_report_build(
    const UmiDeveloperAcceptanceGateResult *results,
    size_t result_count,
    UmiDeveloperAcceptanceReport *out_report);

#ifdef __cplusplus
}
#endif
#endif
