/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/rename_symbol.c
 *
 * PURPOSE:
 *   Implement provider-neutral symbol rename planning and conflict-safe
 *   application through the reusable Framework workspace edit engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/rename_symbol.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiEditorRenameSymbolPlan {
    UmiEditorWorkspaceEditSet *edits;
    char symbol_id[UMI_EDITOR_SYMBOL_ID_CAPACITY];
    char old_name[UMI_EDITOR_RENAME_NAME_CAPACITY];
    char new_name[UMI_EDITOR_RENAME_NAME_CAPACITY];
    UmiEditorRenameSymbolState state;
    uint64_t revision;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Check that editor rename symbol name satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_editor_rename_symbol_name_validate(
    const char *name,
    int allow_dollar,
    int allow_utf8_identifier_bytes)
{
    const unsigned char *cursor;
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (name == NULL || name[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(name);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= UMI_EDITOR_RENAME_NAME_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    cursor = (const unsigned char *)name;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!(isalpha((int)*cursor) || *cursor == (unsigned char)'_' ||
          (allow_dollar && *cursor == (unsigned char)'$') ||
          (allow_utf8_identifier_bytes && *cursor >= 0x80U))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    ++cursor;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != 0U) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (!(isalnum((int)*cursor) || *cursor == (unsigned char)'_' ||
              (allow_dollar && *cursor == (unsigned char)'$') ||
              (allow_utf8_identifier_bytes && *cursor >= 0x80U))) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        ++cursor;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the location already added operation used by this module and its client
 * applications.
 */
static int location_already_added(const UmiEditorWorkspaceEditSet *edits,
                                  const UmiEditorSourceLocation *location)
{
    size_t index;
    UmiEditorWorkspaceTextEdit edit;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_editor_workspace_edit_set_count(edits);
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_editor_workspace_edit_set_at(edits, index, &edit) !=
            UMI_STATUS_OK) {
            continue;
        }
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(edit.location.uri, location->uri) == 0 &&
            edit.location.byte_offset == location->byte_offset &&
            edit.location.end_byte_offset == location->end_byte_offset) {
            return 1;
        }
    }
    return 0;
}

/* Provide the add rename edit operation used by this module and its client applications. */
static UmiStatus add_rename_edit(UmiEditorRenameSymbolPlan *plan,
                                 const UmiEditorSourceLocation *source_location,
                                 const char *provider_id,
                                 size_t sequence)
{
    UmiEditorWorkspaceTextEdit edit;
    UmiEditorSourceLocation location;
    size_t old_length = strlen(plan->old_name);
    int written;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source_location == NULL ||
        umi_editor_source_location_validate(source_location) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    location = *source_location;
    /* Apply this branch only when its contract condition is satisfied. */
    if (location.end_byte_offset == location.byte_offset) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (old_length > UINT64_MAX - location.byte_offset) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        location.end_byte_offset = location.byte_offset + (uint64_t)old_length;
        /* Apply this branch only when its contract condition is satisfied. */
        if (location.end_line == location.line &&
            old_length <= UINT64_MAX - location.column) {
            location.end_column = location.column + (uint64_t)old_length;
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (location_already_added(plan->edits, &location)) return UMI_STATUS_OK;
    (void)memset(&edit, 0, sizeof(edit));
    edit.struct_size = (uint32_t)sizeof(edit);
    edit.api_version = UMI_EDITOR_WORKSPACE_EDIT_API_VERSION;
    written = snprintf(edit.id, sizeof(edit.id), "rename-%zu", sequence);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(edit.id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = copy_text(edit.provider_id, sizeof(edit.provider_id),
                       provider_id != NULL && provider_id[0] != '\0'
                           ? provider_id
                           : "symbol-index");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(edit.expected_text, sizeof(edit.expected_text),
                       plan->old_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(edit.replacement_text, sizeof(edit.replacement_text),
                       plan->new_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    edit.location = location;
    edit.location.kind = UMI_EDITOR_SOURCE_LOCATION_EDIT;
    edit.state = UMI_EDITOR_WORKSPACE_EDIT_READY;
    edit.required = 1;
    return umi_editor_workspace_edit_set_upsert(plan->edits, &edit);
}

/*
 * Initialise editor rename symbol plan from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_rename_symbol_plan_create(
    UmiEditorRenameSymbolPlan **out_plan)
{
    UmiEditorRenameSymbolPlan *plan;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_plan = NULL;
    plan = (UmiEditorRenameSymbolPlan *)calloc(1U, sizeof(*plan));
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
    plan->revision = 1U;
    *out_plan = plan;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor rename symbol plan so the same storage can be
 * reused safely.
 */
void umi_editor_rename_symbol_plan_destroy(UmiEditorRenameSymbolPlan *plan)
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
 * Provide the editor rename symbol plan prepare operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_rename_symbol_plan_prepare(
    UmiEditorRenameSymbolPlan *plan,
    const UmiEditorSymbolIndex *symbol_index,
    const UmiEditorNavigationResultSet *references,
    const UmiEditorRenameSymbolRequest *request)
{
    UmiEditorIndexedSymbol symbol;
    UmiEditorNavigationResult reference;
    UmiEditorWorkspaceEditSnapshot edit_snapshot;
    size_t index;
    size_t sequence = 1U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || symbol_index == NULL || request == NULL ||
        request->struct_size != (uint32_t)sizeof(*request) ||
        request->api_version != UMI_EDITOR_RENAME_SYMBOL_API_VERSION ||
        request->symbol_id == NULL || request->symbol_id[0] == '\0' ||
        request->new_name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_rename_symbol_name_validate(
        request->new_name, request->allow_dollar,
        request->allow_utf8_identifier_bytes);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (references != NULL &&
        umi_editor_navigation_result_set_query_kind(references) !=
            UMI_EDITOR_NAVIGATION_QUERY_REFERENCE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_symbol_index_find(symbol_index, request->symbol_id,
                                          &symbol);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if ((symbol.flags & UMI_EDITOR_INDEXED_SYMBOL_FLAG_READ_ONLY) != 0U) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(symbol.name, request->new_name) == 0) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    status = umi_editor_workspace_edit_set_clear(plan->edits);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(plan->symbol_id, sizeof(plan->symbol_id), symbol.id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(plan->old_name, sizeof(plan->old_name), symbol.name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(plan->new_name, sizeof(plan->new_name), request->new_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = add_rename_edit(plan, &symbol.selection_location,
                             symbol.provider_id, sequence++);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (references != NULL) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U;
             index < umi_editor_navigation_result_set_count(references);
             ++index) {
            status = umi_editor_navigation_result_set_at(references, index,
                                                          &reference);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
            status = add_rename_edit(plan, &reference.location,
                                     reference.provider_id, sequence++);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
        }
    }
    status = umi_editor_workspace_edit_set_finalize(plan->edits);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_workspace_edit_set_snapshot(plan->edits,
                                                     &edit_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    plan->state = edit_snapshot.conflict_count > 0U
        ? UMI_EDITOR_RENAME_SYMBOL_CONFLICT
        : UMI_EDITOR_RENAME_SYMBOL_PREPARED;
    plan->revision = next_revision(plan->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor rename symbol plan apply document operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_rename_symbol_plan_apply_document(
    UmiEditorRenameSymbolPlan *plan,
    const char *document_uri,
    UmiEditorTextBuffer *buffer,
    int require_matching_revision,
    size_t *out_applied_count)
{
    UmiEditorWorkspaceEditSnapshot snapshot;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || (plan->state != UMI_EDITOR_RENAME_SYMBOL_PREPARED &&
                         plan->state != UMI_EDITOR_RENAME_SYMBOL_APPLYING)) {
        return UMI_STATUS_INVALID_STATE;
    }
    plan->state = UMI_EDITOR_RENAME_SYMBOL_APPLYING;
    plan->revision = next_revision(plan->revision);
    status = umi_editor_workspace_edit_set_apply_document(
        plan->edits, document_uri, buffer, require_matching_revision,
        out_applied_count);
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_workspace_edit_set_snapshot(plan->edits, &snapshot) ==
        UMI_STATUS_OK) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (snapshot.conflict_count > 0U) {
            plan->state = UMI_EDITOR_RENAME_SYMBOL_CONFLICT;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (snapshot.applied_count == snapshot.edit_count) {
            plan->state = UMI_EDITOR_RENAME_SYMBOL_APPLIED;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            plan->state = UMI_EDITOR_RENAME_SYMBOL_PREPARED;
        }
    }
    plan->revision = next_revision(plan->revision);
    return status;
}

/*
 * Provide the editor rename symbol plan cancel operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_rename_symbol_plan_cancel(
    UmiEditorRenameSymbolPlan *plan)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->state == UMI_EDITOR_RENAME_SYMBOL_APPLIED) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_editor_workspace_edit_set_clear(plan->edits);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    plan->state = UMI_EDITOR_RENAME_SYMBOL_CANCELLED;
    plan->revision = next_revision(plan->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor rename symbol plan snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_rename_symbol_plan_snapshot(
    const UmiEditorRenameSymbolPlan *plan,
    UmiEditorRenameSymbolSnapshot *out_snapshot)
{
    UmiEditorWorkspaceEditSnapshot edit_snapshot;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_workspace_edit_set_snapshot(plan->edits,
                                                     &edit_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_RENAME_SYMBOL_API_VERSION;
    (void)copy_text(out_snapshot->symbol_id, sizeof(out_snapshot->symbol_id),
                    plan->symbol_id);
    (void)copy_text(out_snapshot->old_name, sizeof(out_snapshot->old_name),
                    plan->old_name);
    (void)copy_text(out_snapshot->new_name, sizeof(out_snapshot->new_name),
                    plan->new_name);
    out_snapshot->state = plan->state;
    out_snapshot->edit_count = edit_snapshot.edit_count;
    out_snapshot->document_count = edit_snapshot.document_count;
    out_snapshot->conflict_count = edit_snapshot.conflict_count;
    out_snapshot->revision = plan->revision;
    out_snapshot->can_apply = plan->state == UMI_EDITOR_RENAME_SYMBOL_PREPARED &&
                              edit_snapshot.applicable;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor rename symbol plan edits operation used by this module and its client
 * applications.
 */
UmiEditorWorkspaceEditSet *umi_editor_rename_symbol_plan_edits(
    UmiEditorRenameSymbolPlan *plan)
{
    return plan != NULL ? plan->edits : NULL;
}
