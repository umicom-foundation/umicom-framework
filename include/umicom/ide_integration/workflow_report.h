/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/workflow_report.h
 *
 * PURPOSE:
 *   Retain explicit workflow readiness gates and overall launch/build/package
 *   readiness evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_WORKFLOW_REPORT_H
#define UMICOM_IDE_INTEGRATION_WORKFLOW_REPORT_H
#include "umicom/ide_integration/workflow_policy.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiIdeWorkflowReport {
    UmiIdeWorkflowGate gates[UMI_IDE_INTEGRATION_GATE_CAPACITY];
    size_t gate_count;
    size_t passed_count;
    size_t warning_count;
    size_t blocked_count;
    uint64_t revision;
    int ready;
} UmiIdeWorkflowReport;

void umi_ide_workflow_report_init(UmiIdeWorkflowReport *report);

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
