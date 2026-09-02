/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/workflow_report.h
 *
 * PURPOSE:
 *   Retain explicit workflow readiness gates and overall launch/build/package
 *   readiness evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_WORKFLOW_REPORT_H
#define UMICOM_IDE_INTEGRATION_WORKFLOW_REPORT_H
#include "umicom/ide_integration/workflow_policy.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ide workflow report data shared with callers of this public contract.
 */
typedef struct UmiIdeWorkflowReport {
    UmiIdeWorkflowGate gates[UMI_IDE_INTEGRATION_GATE_CAPACITY];
    size_t gate_count;
    size_t passed_count;
    size_t warning_count;
    size_t blocked_count;
    uint64_t revision;
    int ready;
} UmiIdeWorkflowReport;

/**
 * Initialise ide workflow report from caller-provided values so later operations receive a
 * known state.
 */
void umi_ide_workflow_report_init(UmiIdeWorkflowReport *report);

/**
 * Add ide workflow report only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ide_workflow_report_add(
    UmiIdeWorkflowReport *report,
    const char *gate_id,
    const char *label,
    const char *detail,
    UmiIdeWorkflowGateState state,
    int required);

#ifdef __cplusplus
}
#endif
#endif
