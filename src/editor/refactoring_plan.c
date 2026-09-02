/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/refactoring_plan.c
 *
 * PURPOSE:
 *   Implement validated refactoring-plan lifecycle and canonical workspace
 *   edit ownership for language tools, AI providers and application shells.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/refactoring_plan.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorRefactoringPlan {
    UmiEditorRefactoringDescriptor descriptor;
    UmiEditorWorkspaceEditSet *edits;
    UmiEditorRefactoringPlanState state;
    uint64_t revision;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the terminated operation used by this module and its client applications. */
static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

/*
 * Provide the validate descriptor operation used by this module and its client
 * applications.
 */
static UmiStatus validate_descriptor(
    const UmiEditorRefactoringDescriptor *descriptor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL ||
        descriptor->struct_size != (uint32_t)sizeof(*descriptor) ||
        descriptor->api_version != UMI_EDITOR_REFACTORING_PLAN_API_VERSION ||
        !terminated(descriptor->id, sizeof(descriptor->id)) ||
        descriptor->id[0] == '\0' ||
        !terminated(descriptor->action_id, sizeof(descriptor->action_id)) ||
        descriptor->action_id[0] == '\0' ||
        !terminated(descriptor->provider_id,
                    sizeof(descriptor->provider_id)) ||
        descriptor->provider_id[0] == '\0' ||
        !terminated(descriptor->title, sizeof(descriptor->title)) ||
        descriptor->title[0] == '\0' ||
        !terminated(descriptor->document_uri,
                    sizeof(descriptor->document_uri)) ||
        descriptor->document_uri[0] == '\0' ||
        !terminated(descriptor->symbol_id, sizeof(descriptor->symbol_id)) ||
        descriptor->kind < UMI_EDITOR_REFACTORING_RENAME ||
        descriptor->kind > UMI_EDITOR_REFACTORING_CUSTOM ||
        descriptor->risk < UMI_EDITOR_REFACTORING_RISK_SAFE ||
        descriptor->risk > UMI_EDITOR_REFACTORING_RISK_UNSAFE ||
        descriptor->end_byte_offset < descriptor->start_byte_offset) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Initialise editor refactoring plan from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_refactoring_plan_create(
    UmiEditorRefactoringPlan **out_plan)
{
    UmiEditorRefactoringPlan *plan;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_plan = NULL;
    plan = (UmiEditorRefactoringPlan *)calloc(1U, sizeof(*plan));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_editor_workspace_edit_set_create(&plan->edits);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(plan);
        return status;
    }
    plan->state = UMI_EDITOR_REFACTORING_PLAN_EMPTY;
    plan->revision = 1U;
    *out_plan = plan;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor refactoring plan so the same storage can be reused
 * safely.
 */
void umi_editor_refactoring_plan_destroy(UmiEditorRefactoringPlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return;
    umi_editor_workspace_edit_set_destroy(plan->edits);
    plan->edits = NULL;
    free(plan);
}

/*
 * Provide the editor refactoring plan begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_refactoring_plan_begin(
    UmiEditorRefactoringPlan *plan,
    const UmiEditorRefactoringDescriptor *descriptor)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || validate_descriptor(descriptor) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_workspace_edit_set_clear(plan->edits);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    plan->descriptor = *descriptor;
    plan->descriptor.require_matching_revision =
        descriptor->require_matching_revision != 0;
    plan->descriptor.supports_preview = descriptor->supports_preview != 0;
    plan->state = UMI_EDITOR_REFACTORING_PLAN_COLLECTING;
    plan->revision = next_revision(plan->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor refactoring plan add edit operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_refactoring_plan_add_edit(
    UmiEditorRefactoringPlan *plan,
    const UmiEditorWorkspaceTextEdit *edit)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || edit == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->state != UMI_EDITOR_REFACTORING_PLAN_COLLECTING) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(plan->descriptor.provider_id, edit->provider_id) != 0) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    status = umi_editor_workspace_edit_set_upsert(plan->edits, edit);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) plan->revision = next_revision(plan->revision);
    return status;
}

/*
 * Provide the editor refactoring plan finalize operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_refactoring_plan_finalize(UmiEditorRefactoringPlan *plan)
{
    UmiEditorWorkspaceEditSnapshot edit_snapshot;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->state != UMI_EDITOR_REFACTORING_PLAN_COLLECTING) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_workspace_edit_set_count(plan->edits) == 0U) {
        return UMI_STATUS_NOT_FOUND;
    }
    status = umi_editor_workspace_edit_set_finalize(plan->edits);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_workspace_edit_set_snapshot(plan->edits, &edit_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    plan->state = edit_snapshot.conflict_count > 0U
        ? UMI_EDITOR_REFACTORING_PLAN_CONFLICT
        : UMI_EDITOR_REFACTORING_PLAN_READY;
    plan->revision = next_revision(plan->revision);
    return plan->state == UMI_EDITOR_REFACTORING_PLAN_READY
        ? UMI_STATUS_OK
        : UMI_STATUS_INVALID_STATE;
}

/*
 * Provide the editor refactoring plan mark applied operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_refactoring_plan_mark_applied(
    UmiEditorRefactoringPlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this operation only while the related capability or state is available. */
    if (plan->state != UMI_EDITOR_REFACTORING_PLAN_READY) {
        return UMI_STATUS_INVALID_STATE;
    }
    plan->state = UMI_EDITOR_REFACTORING_PLAN_APPLIED;
    plan->revision = next_revision(plan->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor refactoring plan cancel operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_refactoring_plan_cancel(UmiEditorRefactoringPlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->state == UMI_EDITOR_REFACTORING_PLAN_APPLIED) {
        return UMI_STATUS_INVALID_STATE;
    }
    plan->state = UMI_EDITOR_REFACTORING_PLAN_CANCELLED;
    plan->revision = next_revision(plan->revision);
    return UMI_STATUS_OK;
}

/*
 * Copy editor refactoring plan edit into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiEditorWorkspaceEditSet *umi_editor_refactoring_plan_edit_set(
    UmiEditorRefactoringPlan *plan)
{
    return plan != NULL ? plan->edits : NULL;
}

/*
 * Provide the editor refactoring plan edit set const operation used by this module and its
 * client applications.
 */
const UmiEditorWorkspaceEditSet *umi_editor_refactoring_plan_edit_set_const(
    const UmiEditorRefactoringPlan *plan)
{
    return plan != NULL ? plan->edits : NULL;
}

/*
 * Provide the editor refactoring plan snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_refactoring_plan_snapshot(
    const UmiEditorRefactoringPlan *plan,
    UmiEditorRefactoringPlanSnapshot *out_snapshot)
{
    UmiEditorWorkspaceEditSnapshot edit_snapshot;
    UmiEditorWorkspaceTextEdit edit;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_REFACTORING_PLAN_API_VERSION;
    out_snapshot->descriptor = plan->descriptor;
    out_snapshot->state = plan->state;
    out_snapshot->revision = plan->revision;
    status = umi_editor_workspace_edit_set_snapshot(plan->edits, &edit_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->edit_count = edit_snapshot.edit_count;
    out_snapshot->document_count = edit_snapshot.document_count;
    out_snapshot->conflict_count = edit_snapshot.conflict_count;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < edit_snapshot.edit_count; ++index) {
        status = umi_editor_workspace_edit_set_at(plan->edits, index, &edit);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Apply this branch only when its contract condition is satisfied. */
        if (edit.required) ++out_snapshot->required_edit_count;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the editor refactoring plan revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_refactoring_plan_revision(
    const UmiEditorRefactoringPlan *plan)
{
    return plan != NULL ? plan->revision : 0U;
}
