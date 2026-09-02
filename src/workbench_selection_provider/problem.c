/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection_provider/problem.c
 *
 * PURPOSE:
 *   Preserve problem ID, source, code, URI, line, column, severity and resolution state without parsing rendered text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection_provider/problem.h"

#include "umicom/workbench_selection/builders.h"

#include <stdio.h>

/* Provide the severity text operation used by this module and its client applications. */
static const char *severity_text(int severity)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (severity) {
    case 1: return "error";
    case 2: return "warning";
    case 3: return "information";
    case 4: return "hint";
    default: return "unknown";
    }
}

/* Provide the add boolean operation used by this module and its client applications. */
static UmiStatus add_boolean(
    UmiWorkbenchSelection *selection,
    const char *name,
    bool value)
{
    UmiWorkbenchSelectionField field;
    UmiStatus status;
    umi_workbench_selection_field_init(&field, name);
    status = umi_workbench_selection_field_set_boolean(&field, value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_selection_add_field(selection, &field);
}

/* Provide the add text operation used by this module and its client applications. */
static UmiStatus add_text(
    UmiWorkbenchSelection *selection,
    const char *name,
    const char *value)
{
    UmiWorkbenchSelectionField field;
    UmiStatus status;
    umi_workbench_selection_field_init(&field, name);
    status = umi_workbench_selection_field_set_text(
        &field, value != NULL ? value : "");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_selection_add_field(selection, &field);
}

/*
 * Provide the workbench selection provider from problem operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_provider_from_problem(
    const UmiUiProblemSnapshot *problem,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection)
{
    char selection_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (problem == NULL || application_id == NULL ||
        panel_id == NULL || out_selection == NULL ||
        problem->id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_selection_provider_make_id(
        selection_id,
        sizeof(selection_id),
        "problem",
        problem->id,
        problem->revision);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_workbench_selection_build_diagnostic(
        out_selection,
        selection_id,
        application_id,
        panel_id,
        workspace_id,
        problem->id,
        problem->uri,
        problem->line,
        problem->column,
        severity_text(problem->severity),
        problem->code,
        problem->message,
        timestamp_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = add_text(out_selection, "source", problem->source);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = add_boolean(
        out_selection, "resolved", problem->resolved != 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    out_selection->boolean_value = problem->resolved != 0;
    (void)umi_workbench_selection_refresh_hash(out_selection);
    return UMI_STATUS_OK;
}

/*
 * Find workbench selection provider problem while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_workbench_selection_provider_problem_at(
    const UmiUiProblemRegistry *registry,
    size_t index,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection)
{
    UmiUiProblemSnapshot problem;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_problem_registry_at(registry, index, &problem);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_selection_provider_from_problem(
        &problem,
        application_id,
        panel_id,
        workspace_id,
        timestamp_ms,
        out_selection);
}

/*
 * Find workbench selection provider problem while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_workbench_selection_provider_problem_find(
    const UmiUiProblemRegistry *registry,
    const char *problem_id,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection)
{
    UmiUiProblemSnapshot problem;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || problem_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ui_problem_registry_find(
        registry, problem_id, &problem);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_selection_provider_from_problem(
        &problem,
        application_id,
        panel_id,
        workspace_id,
        timestamp_ms,
        out_selection);
}
