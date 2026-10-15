/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/workflow_report.c
 *
 * PURPOSE:
 *   Implement bounded workflow gate accumulation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "umicom/ide_integration/workflow_report.h"

#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *out, size_t capacity, const char *text)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out, text, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Initialise ide workflow report from caller-provided values so later operations receive a
 * known state.
 */
void umi_ide_workflow_report_init(UmiIdeWorkflowReport *report)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report == NULL) return;
    (void)memset(report, 0, sizeof(*report));
    report->revision = 1U;
    report->ready = 1;
}

/* Add ide workflow report only after its inputs and available capacity have been checked. */
/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report == NULL || gate_id == NULL ||
        label == NULL || detail == NULL ||
        state < UMI_IDE_GATE_UNKNOWN ||
        state > UMI_IDE_GATE_BLOCK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (report->gate_count >= UMI_IDE_INTEGRATION_GATE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    gate = &report->gates[report->gate_count];
    (void)memset(gate, 0, sizeof(*gate));

    status = copy_text(gate->gate_id, sizeof(gate->gate_id), gate_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(gate->label, sizeof(gate->label), label);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(gate->detail, sizeof(gate->detail), detail);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    gate->state = state;
    gate->required = required != 0;
    gate->revision = report->revision + 1U;

    report->gate_count += 1U;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (state == UMI_IDE_GATE_PASS) {
        report->passed_count += 1U;
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (state == UMI_IDE_GATE_WARN) {
        report->warning_count += 1U;
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (state == UMI_IDE_GATE_BLOCK) {
        report->blocked_count += 1U;
        /* Apply this operation only while the related capability or state is available. */
        if (required) report->ready = 0;
    }

    report->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_ide_workflow_report_add(
    UmiIdeWorkflowReport *report,
    const char *gate_id,
    const char *label,
    const char *detail,
    UmiIdeWorkflowGateState state,
    int required)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (report != NULL && report->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    UmiIdeWorkflowGate *gate;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report == NULL || gate_id == NULL ||
        label == NULL || detail == NULL ||
        state < UMI_IDE_GATE_UNKNOWN ||
        state > UMI_IDE_GATE_BLOCK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (report->gate_count >= UMI_IDE_INTEGRATION_GATE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    gate = &report->gates[report->gate_count];
    (void)memset(gate, 0, sizeof(*gate));

    status = copy_text(gate->gate_id, sizeof(gate->gate_id), gate_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(gate->label, sizeof(gate->label), label);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(gate->detail, sizeof(gate->detail), detail);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    gate->state = state;
    gate->required = required != 0;
    gate->revision = report->revision + 1U;

    report->gate_count += 1U;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (state == UMI_IDE_GATE_PASS) {
        report->passed_count += 1U;
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (state == UMI_IDE_GATE_WARN) {
        report->warning_count += 1U;
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (state == UMI_IDE_GATE_BLOCK) {
        report->blocked_count += 1U;
        /* Apply this operation only while the related capability or state is available. */
        if (required) report->ready = 0;
    }

    report->revision += 1U;
    return UMI_STATUS_OK;
}
