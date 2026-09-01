/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workspace_customisation_persistence.h
 *
 * PURPOSE:
 *   Save and restore one active customisable workspace through a validated,
 *   rollback-safe Framework service shared by every Umicom application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_WORKSPACE_CUSTOMISATION_PERSISTENCE_H
#define UMICOM_UI_WORKSPACE_CUSTOMISATION_PERSISTENCE_H

#include "umicom/ui/layout_persistence.h"
#include "umicom/ui/workspace_customisation.h"

/* Choose whether an imported layout may replace an existing layout with the
 * same stable identifier. Reject is the safest default for general callers. */
typedef enum UmiUiWorkspaceImportConflictPolicy {
    UMI_UI_WORKSPACE_IMPORT_REJECT_CONFLICT = 1,
    UMI_UI_WORKSPACE_IMPORT_REPLACE_CONFLICT = 2
} UmiUiWorkspaceImportConflictPolicy;

/* Describe import policy without exposing the customisation object's storage.
 * Registered tools and context groups are required by default so a restored
 * layout cannot contain panels which the receiving application cannot build. */
typedef struct UmiUiWorkspaceImportOptions {
    UmiUiWorkspaceImportConflictPolicy conflict_policy;
    bool activate_imported_layout;
    bool allow_new_layout;
    bool require_registered_tools;
    bool require_registered_context_groups;
} UmiUiWorkspaceImportOptions;

/* Report the exact result of an import so graphical and command-line clients
 * can explain whether the layout was added, replaced and activated. */
typedef struct UmiUiWorkspaceImportReport {
    char layout_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    size_t imported_window_count;
    size_t linked_context_count;
    bool added;
    bool replaced;
    bool activated;
    uint64_t saved_at_ns;
    uint64_t customisation_revision;
} UmiUiWorkspaceImportReport;

/* Return the conservative import policy suitable for untrusted saved text. */
UmiUiWorkspaceImportOptions umi_ui_workspace_import_options_default(void);

/* Encode the active layout using the current Framework persistence schema. */
UmiStatus umi_ui_workspace_customisation_export_active(
    const UmiUiWorkspaceCustomisation *customisation,
    uint64_t saved_at_ns,
    char *out_text,
    size_t capacity);

/* Decode, validate and atomically publish a saved layout. On failure the live
 * customisation object and its active selection remain completely unchanged. */
UmiStatus umi_ui_workspace_customisation_import(
    UmiUiWorkspaceCustomisation *customisation,
    const char *text,
    const UmiUiWorkspaceImportOptions *options,
    UmiUiWorkspaceImportReport *out_report);

#endif
