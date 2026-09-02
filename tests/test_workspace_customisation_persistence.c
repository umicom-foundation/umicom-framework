/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_customisation_persistence.c
 *
 * PURPOSE:
 *   Verify portable workspace export, dependency validation, conflict policy,
 *   context repair and rollback-safe import for every Umicom application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/ui/workspace_customisation_persistence.h"

/* Register the minimum panel catalogue and context group used by the fixture. */
static void seed_dependencies(UmiUiWorkspaceCustomisation *customisation)
{
    UmiUiWindowDescriptor descriptor;

    (void)memset(&descriptor, 0, sizeof(descriptor));
    (void)snprintf(descriptor.tool_id, sizeof(descriptor.tool_id), "editor");
    (void)snprintf(descriptor.title, sizeof(descriptor.title), "Editor");
    descriptor.category = UMI_UI_WINDOW_CATEGORY_DEVELOPMENT;
    descriptor.supports_multiple = true;
    assert(umi_ui_window_catalogue_register(
               &customisation->windows, &descriptor) == UMI_STATUS_OK);
    assert(umi_ui_window_group_define(
               &customisation->groups,
               "project-blue",
               "blue",
               UMI_UI_WINDOW_CONTEXT_PROJECT) == UMI_STATUS_OK);
}

/* Create one locked workspace with a context-linked editor panel. */
static void seed_layout(UmiUiWorkspaceCustomisation *customisation)
{
    UmiUiWorkspaceLayout layout;
    UmiUiWorkspaceWindow editor;

    (void)memset(&editor, 0, sizeof(editor));
    (void)snprintf(editor.window_id, sizeof(editor.window_id), "editor-main");
    (void)snprintf(editor.title, sizeof(editor.title), "Editor");
    (void)snprintf(editor.tool_id, sizeof(editor.tool_id), "editor");
    (void)snprintf(editor.group_id, sizeof(editor.group_id), "centre");
    (void)snprintf(editor.placement_id,
                   sizeof(editor.placement_id), "centre");
    (void)snprintf(editor.stack_id, sizeof(editor.stack_id), "documents");
    (void)snprintf(editor.context_group_id,
                   sizeof(editor.context_group_id), "project-blue");
    editor.width = 1.0;
    editor.height = 1.0;
    editor.visible = true;
    editor.closable = true;
    editor.resizable = true;

    assert(umi_ui_workspace_layout_init(
               &layout, "development", "Development") == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_add_window(
               &layout, &editor) == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_set_locked(
               &layout, true) == UMI_STATUS_OK);
    assert(umi_ui_workspace_customisation_add_layout(
               customisation, &layout) == UMI_STATUS_OK);
    assert(umi_ui_window_group_assign(
               &customisation->groups,
               "project-blue",
               "editor-main",
               UMI_UI_WINDOW_GROUP_SOURCE) == UMI_STATUS_OK);
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiUiWorkspaceCustomisation *source =
        (UmiUiWorkspaceCustomisation *)malloc(sizeof(*source));
    UmiUiWorkspaceCustomisation *target =
        (UmiUiWorkspaceCustomisation *)malloc(sizeof(*target));
    UmiUiWorkspaceCustomisation *invalid_target =
        (UmiUiWorkspaceCustomisation *)malloc(sizeof(*invalid_target));
    UmiUiWorkspaceImportOptions options =
        umi_ui_workspace_import_options_default();
    UmiUiWorkspaceImportReport report;
    UmiUiWorkspaceCustomisation *unchanged =
        (UmiUiWorkspaceCustomisation *)malloc(sizeof(*unchanged));
    UmiUiWindowGroupRole restored_role;
    UmiUiWorkspaceWindow removed_panel;
    char encoded[UMI_UI_LAYOUT_ENCODED_CAPACITY];

    /* Customisation models contain multiple complete layouts, so tests keep
     * them on the heap rather than depending on a platform-specific stack. */
    assert(source != NULL);
    assert(target != NULL);
    assert(invalid_target != NULL);
    assert(unchanged != NULL);

    umi_ui_workspace_customisation_init(source);
    seed_dependencies(source);
    seed_layout(source);
    assert(umi_ui_workspace_customisation_export_active(
               source, UINT64_C(9000), encoded, sizeof(encoded)) ==
           UMI_STATUS_OK);
    assert(strstr(encoded, "UMILAYOUT3") != NULL);

    umi_ui_workspace_customisation_init(target);
    seed_dependencies(target);
    options.allow_new_layout = true;
    assert(umi_ui_workspace_customisation_import(
               target, encoded, &options, &report) == UMI_STATUS_OK);
    assert(report.added);
    assert(!report.replaced);
    assert(report.activated);
    assert(report.imported_window_count == 1U);
    assert(report.linked_context_count == 1U);
    assert(strcmp(target->active_layout_id, "development") == 0);
    assert(umi_ui_window_group_for_window(
               &target->groups, "editor-main", &restored_role) != NULL);
    assert(restored_role == UMI_UI_WINDOW_GROUP_BIDIRECTIONAL);

    /* Default policy prevents accidental overwrite of a user's layout. */
    *unchanged = *target;
    assert(umi_ui_workspace_customisation_import(
               target, encoded, &options, &report) ==
           UMI_STATUS_ALREADY_EXISTS);
    assert(memcmp(target, unchanged, sizeof(*target)) == 0);

    /* Explicit replacement updates the existing slot rather than duplicating
     * a layout identifier and preserves the existing context member role. */
    (void)memset(&removed_panel, 0, sizeof(removed_panel));
    (void)snprintf(
        removed_panel.window_id,
        sizeof(removed_panel.window_id),
        "removed-panel");
    (void)snprintf(
        removed_panel.title, sizeof(removed_panel.title), "Removed Panel");
    (void)snprintf(
        removed_panel.tool_id, sizeof(removed_panel.tool_id), "editor");
    removed_panel.width = 0.5;
    removed_panel.height = 0.5;
    removed_panel.visible = true;
    removed_panel.closable = true;
    removed_panel.resizable = true;
    assert(umi_ui_workspace_customisation_begin_edit(target) ==
           UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_add_window(
               umi_ui_workspace_customisation_active(target),
               &removed_panel) == UMI_STATUS_OK);
    assert(umi_ui_window_group_assign(
               &target->groups,
               "project-blue",
               "removed-panel",
               UMI_UI_WINDOW_GROUP_DESTINATION) == UMI_STATUS_OK);
    assert(umi_ui_workspace_customisation_commit_edit(target) ==
           UMI_STATUS_OK);
    options.conflict_policy = UMI_UI_WORKSPACE_IMPORT_REPLACE_CONFLICT;
    assert(umi_ui_window_group_assign(
               &target->groups,
               "project-blue",
               "editor-main",
               UMI_UI_WINDOW_GROUP_SOURCE) == UMI_STATUS_OK);
    assert(umi_ui_workspace_customisation_import(
               target, encoded, &options, &report) == UMI_STATUS_OK);
    assert(report.replaced);
    assert(!report.added);
    assert(target->layout_count == 1U);
    assert(umi_ui_window_group_for_window(
               &target->groups, "editor-main", &restored_role) != NULL);
    assert(restored_role == UMI_UI_WINDOW_GROUP_SOURCE);
    assert(umi_ui_window_group_for_window(
               &target->groups, "removed-panel", NULL) == NULL);

    /* A receiving application without the required panel catalogue rejects
     * the import without publishing a partially decoded layout. */
    umi_ui_workspace_customisation_init(invalid_target);
    *unchanged = *invalid_target;
    assert(umi_ui_workspace_customisation_import(
               invalid_target, encoded, NULL, &report) ==
           UMI_STATUS_NOT_FOUND);
    assert(memcmp(invalid_target, unchanged, sizeof(*invalid_target)) == 0);

    /* An active edit owns a rollback baseline, so importing must wait. */
    assert(umi_ui_workspace_customisation_begin_edit(target) ==
           UMI_STATUS_OK);
    *unchanged = *target;
    assert(umi_ui_workspace_customisation_import(
               target, encoded, &options, &report) == UMI_STATUS_BUSY);
    assert(memcmp(target, unchanged, sizeof(*target)) == 0);
    assert(umi_ui_workspace_customisation_cancel_edit(target) ==
           UMI_STATUS_OK);
    free(unchanged);
    free(invalid_target);
    free(target);
    free(source);
    return 0;
}
