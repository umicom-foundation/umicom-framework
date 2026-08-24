/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/workflow_report.c
 *
 * PURPOSE:
 *   Implement bounded workflow gate accumulation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/workflow_report.h"

#include <string.h>

static UmiStatus copy_text(char *out, size_t capacity, const char *text)
{
    size_t length;

    if (out == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(text);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out, text, length + 1U);
    return UMI_STATUS_OK;
}

void umi_ide_workflow_report_init(UmiIdeWorkflowReport *report)
{
    if (report == NULL) return;
    (void)memset(report, 0, sizeof(*report));
    report->revision = 1U;
    report->ready = 1;
}

UmiStatus umi_ide_workflow_report_add(
    UmiIdeWorkflowReport *report,
    const char *gate_id,
    const char *label,
    const char *detail,
    UmiIdeWorkflowGateState state,
    int required)
{
    UmiIdeWorkflowGate *gate;
    UmiStatus status;

    if (report == NULL || gate_id == NULL ||
        label == NULL || detail == NULL ||
        state < UMI_IDE_GATE_UNKNOWN ||
        state > UMI_IDE_GATE_BLOCK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (report->gate_count >= UMI_IDE_INTEGRATION_GATE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    gate = &report->gates[report->gate_count];
    (void)memset(gate, 0, sizeof(*gate));

    status = copy_text(gate->gate_id, sizeof(gate->gate_id), gate_id);
    if (status == UMI_STATUS_OK)
        status = copy_text(gate->label, sizeof(gate->label), label);
    if (status == UMI_STATUS_OK)
        status = copy_text(gate->detail, sizeof(gate->detail), detail);
    if (status != UMI_STATUS_OK) return status;

    gate->state = state;
    gate->required = required != 0;
    gate->revision = report->revision + 1U;

    report->gate_count += 1U;

    if (state == UMI_IDE_GATE_PASS) {
        report->passed_count += 1U;
    } else if (state == UMI_IDE_GATE_WARN) {
        report->warning_count += 1U;
    } else if (state == UMI_IDE_GATE_BLOCK) {
        report->blocked_count += 1U;
        if (required) report->ready = 0;
    }

    report->revision += 1U;
    return UMI_STATUS_OK;
}
