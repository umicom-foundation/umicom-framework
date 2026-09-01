/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workspace_customisation_persistence.c
 *
 * PURPOSE:
 *   Implement validated and rollback-safe workspace import/export so thin
 *   applications never need their own layout parsing or replacement logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/workspace_customisation_persistence.h"

#include <stdlib.h>
#include <string.h>

/* Find a saved layout by its stable identifier. Returning the maximum count is
 * an internal not-found marker which can never be a valid array position. */
static size_t persistence_layout_index(
    const UmiUiWorkspaceCustomisation *customisation,
    const char *layout_id)
{
    size_t index;

    /* A missing model or identifier cannot name a valid layout. */
    if (customisation == NULL || layout_id == NULL) {
        return UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS;
    }
    /* Compare only populated entries because unused fixed-array elements do
     * not represent layouts even when their memory happens to contain zeros. */
    for (index = 0U; index < customisation->layout_count; ++index) {
        if (strcmp(customisation->layouts[index].layout_id, layout_id) == 0) {
            return index;
        }
    }
    return UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS;
}

/* Validate that the receiving application can construct every saved panel and
 * route every saved context link before any live state is changed. */
static UmiStatus validate_import_dependencies(
    const UmiUiWorkspaceCustomisation *customisation,
    const UmiUiWorkspaceLayout *layout,
    const UmiUiWorkspaceImportOptions *options,
    size_t *out_linked_context_count)
{
    size_t index;
    size_t linked = 0U;

    /* All arguments are required because dependency policy controls whether a
     * saved panel is safe to accept in this application. */
    if (customisation == NULL || layout == NULL || options == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < layout->window_count; ++index) {
        const UmiUiWorkspaceWindow *window = &layout->windows[index];

        /* A registered tool descriptor is the promise that the receiving
         * application can construct the panel named by the saved layout. */
        if (options->require_registered_tools &&
            umi_ui_window_catalogue_find(
                &customisation->windows, window->tool_id) == NULL) {
            return UMI_STATUS_NOT_FOUND;
        }
        /* Empty context text deliberately means an independent panel. */
        if (window->context_group_id[0] != '\0') {
            linked += 1U;
            /* A non-empty context must name an existing routing group so a
             * restored panel cannot silently listen to a missing channel. */
            if (options->require_registered_context_groups &&
                umi_ui_window_group_find(
                    &customisation->groups,
                    window->context_group_id) == NULL) {
                return UMI_STATUS_NOT_FOUND;
            }
        }
    }
    if (out_linked_context_count != NULL) {
        *out_linked_context_count = linked;
    }
    return UMI_STATUS_OK;
}

/* Rebuild reverse context membership for the imported layout. The layout text
 * stores the group ID; an existing member role is preserved when available. */
static UmiStatus reconcile_imported_contexts(
    UmiUiWorkspaceCustomisation *customisation,
    const UmiUiWorkspaceLayout *layout,
    const UmiUiWorkspaceLayout *replaced_layout)
{
    char previous_group_ids[UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS]
                           [UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    UmiUiWindowGroupRole previous_roles[UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS];
    size_t index;

    if (customisation == NULL || layout == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(previous_group_ids, 0, sizeof(previous_group_ids));
    /* Capture roles for panels which survive replacement before removing any
     * old memberships from the reverse group store. */
    for (index = 0U; index < layout->window_count; ++index) {
        const UmiUiWorkspaceWindow *window = &layout->windows[index];
        const UmiUiWindowGroup *previous_group;

        previous_roles[index] = UMI_UI_WINDOW_GROUP_BIDIRECTIONAL;
        previous_group = umi_ui_window_group_for_window(
            &customisation->groups,
            window->window_id,
            &previous_roles[index]);
        if (previous_group != NULL) {
            (void)memcpy(
                previous_group_ids[index],
                previous_group->group_id,
                sizeof(previous_group_ids[index]));
        }
    }

    /* Replacement must forget panels removed from the new record; otherwise
     * they would keep receiving context events despite no longer being open. */
    if (replaced_layout != NULL) {
        for (index = 0U; index < replaced_layout->window_count; ++index) {
            UmiStatus clear_status = umi_ui_window_group_unassign(
                &customisation->groups,
                replaced_layout->windows[index].window_id);
            if (clear_status != UMI_STATUS_OK &&
                clear_status != UMI_STATUS_NOT_FOUND) {
                return clear_status;
            }
        }
    }

    for (index = 0U; index < layout->window_count; ++index) {
        const UmiUiWorkspaceWindow *window = &layout->windows[index];
        UmiUiWindowGroupRole previous_role = previous_roles[index];
        UmiStatus status = umi_ui_window_group_unassign(
            &customisation->groups, window->window_id);

        /* Capture is complete, so each imported window can now replace any
         * membership left by another layout with the same stable window ID. */
        /* Not found simply means this panel had no previous context link. */
        if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) {
            return status;
        }
        if (window->context_group_id[0] == '\0') {
            continue;
        }
        /* Preserve the role only when the panel remains in the same group;
         * otherwise use the safe bidirectional default for a newly linked panel. */
        if (previous_group_ids[index][0] == '\0' ||
            strcmp(previous_group_ids[index], window->context_group_id) != 0) {
            previous_role = UMI_UI_WINDOW_GROUP_BIDIRECTIONAL;
        }
        status = umi_ui_window_group_assign(
            &customisation->groups,
            window->context_group_id,
            window->window_id,
            previous_role);
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }
    return UMI_STATUS_OK;
}

/* Build a zero-initialised conservative policy so future structure fields do
 * not inherit indeterminate data in callers compiled against this header. */
UmiUiWorkspaceImportOptions umi_ui_workspace_import_options_default(void)
{
    UmiUiWorkspaceImportOptions options;

    (void)memset(&options, 0, sizeof(options));
    options.conflict_policy = UMI_UI_WORKSPACE_IMPORT_REJECT_CONFLICT;
    options.activate_imported_layout = true;
    options.allow_new_layout = false;
    options.require_registered_tools = true;
    options.require_registered_context_groups = true;
    return options;
}

/* Export the active layout through the established portable layout codec. */
UmiStatus umi_ui_workspace_customisation_export_active(
    const UmiUiWorkspaceCustomisation *customisation,
    uint64_t saved_at_ns,
    char *out_text,
    size_t capacity)
{
    const UmiUiWorkspaceLayout *layout;
    UmiUiLayoutPersistenceRecord record;

    /* The destination must be writable and have at least room for one byte so
     * the lower-level encoder can always maintain a terminated string. */
    if (customisation == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    layout = umi_ui_workspace_customisation_active_const(customisation);
    if (layout == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    (void)memset(&record, 0, sizeof(record));
    record.schema_version = UMI_UI_LAYOUT_PERSISTENCE_SCHEMA_VERSION;
    record.saved_at_ns = saved_at_ns;
    record.layout = *layout;
    return umi_ui_layout_persistence_encode(&record, out_text, capacity);
}

/* Import a saved layout into a heap-backed candidate and publish it atomically. */
UmiStatus umi_ui_workspace_customisation_import(
    UmiUiWorkspaceCustomisation *customisation,
    const char *text,
    const UmiUiWorkspaceImportOptions *options,
    UmiUiWorkspaceImportReport *out_report)
{
    UmiUiWorkspaceImportOptions default_options;
    const UmiUiWorkspaceImportOptions *effective_options = options;
    UmiUiLayoutPersistenceRecord record;
    UmiUiWorkspaceCustomisation *candidate;
    size_t existing_index;
    size_t linked_context_count = 0U;
    bool added = false;
    bool replaced = false;
    UmiUiWorkspaceLayout replaced_layout;
    bool has_replaced_layout = false;
    UmiStatus status;
    char validation_reason[192U];

    /* Clear output first so a failed import never leaves stale success data. */
    if (out_report != NULL) {
        (void)memset(out_report, 0, sizeof(*out_report));
    }
    if (customisation == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Importing during an edit would invalidate the user's rollback baseline. */
    if (customisation->edit_active) {
        return UMI_STATUS_BUSY;
    }
    if (effective_options == NULL) {
        default_options = umi_ui_workspace_import_options_default();
        effective_options = &default_options;
    }
    /* Reject unknown enum values instead of accidentally treating them as a
     * future policy which this implementation does not understand. */
    if (effective_options->conflict_policy !=
            UMI_UI_WORKSPACE_IMPORT_REJECT_CONFLICT &&
        effective_options->conflict_policy !=
            UMI_UI_WORKSPACE_IMPORT_REPLACE_CONFLICT) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ui_layout_persistence_decode(text, &record);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = umi_ui_workspace_layout_validate(
        &record.layout, validation_reason, sizeof(validation_reason));
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = validate_import_dependencies(
        customisation,
        &record.layout,
        effective_options,
        &linked_context_count);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    existing_index = persistence_layout_index(
        customisation, record.layout.layout_id);
    if (existing_index < UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS &&
        effective_options->conflict_policy ==
            UMI_UI_WORKSPACE_IMPORT_REJECT_CONFLICT) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Some hosts expose only a fixed application-owned layout selector. They
     * can disable additions while still allowing a saved canonical layout to
     * replace its older copy. */
    if (existing_index == UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS &&
        !effective_options->allow_new_layout) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (existing_index == UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS &&
        customisation->layout_count >= UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* The candidate is allocated on the heap because one customisation owns
     * multiple full layouts and can exceed conservative desktop stack limits. */
    candidate = (UmiUiWorkspaceCustomisation *)malloc(sizeof(*candidate));
    if (candidate == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    *candidate = *customisation;

    if (existing_index < UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS) {
        /* Keep the previous layout long enough to clear memberships belonging
         * to panels which the imported replacement no longer contains. */
        replaced_layout = candidate->layouts[existing_index];
        has_replaced_layout = true;
        candidate->layouts[existing_index] = record.layout;
        candidate->revision += 1U;
        replaced = true;
    } else {
        status = umi_ui_workspace_customisation_add_layout(
            candidate, &record.layout);
        if (status != UMI_STATUS_OK) {
            free(candidate);
            return status;
        }
        added = true;
    }

    status = reconcile_imported_contexts(
        candidate,
        &record.layout,
        has_replaced_layout ? &replaced_layout : NULL);
    if (status == UMI_STATUS_OK &&
        effective_options->activate_imported_layout) {
        status = umi_ui_workspace_customisation_activate(
            candidate, record.layout.layout_id);
    }
    if (status != UMI_STATUS_OK) {
        free(candidate);
        return status;
    }

    /* A single assignment is the transaction boundary: no caller can observe
     * the candidate until parsing, validation and context repair all succeed. */
    *customisation = *candidate;
    free(candidate);

    if (out_report != NULL) {
        (void)memcpy(
            out_report->layout_id,
            record.layout.layout_id,
            sizeof(out_report->layout_id));
        out_report->imported_window_count = record.layout.window_count;
        out_report->linked_context_count = linked_context_count;
        out_report->added = added;
        out_report->replaced = replaced;
        out_report->activated = effective_options->activate_imported_layout;
        out_report->saved_at_ns = record.saved_at_ns;
        out_report->customisation_revision = customisation->revision;
    }
    return UMI_STATUS_OK;
}
