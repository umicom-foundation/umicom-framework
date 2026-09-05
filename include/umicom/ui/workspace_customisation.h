/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workspace_customisation.h
 *
 * PURPOSE:
 *   Publish the public workspace customisation contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_WORKSPACE_CUSTOMISATION_H
#define UMICOM_UI_WORKSPACE_CUSTOMISATION_H
#include "umicom/ui/window_catalogue.h"
#include "umicom/ui/window_group.h"
#include "umicom/ui/layout_library.h"
#include "umicom/ui/theme_profile.h"
#define UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS 16U
/**
 * Represent the ui workspace customisation data shared with callers of this public
 * contract.
 */
typedef struct UmiUiWorkspaceCustomisation {
    UmiUiWorkspaceLayout layouts[UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS];
    size_t layout_count;
    char active_layout_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    UmiUiWindowCatalogue windows;
    UmiUiWindowGroupStore groups;
    UmiUiLayoutLibrary library;
    UmiUiThemeProfile theme;
    /* Keep the complete layout and context routing graph so Cancel can undo
     * every visible and behavioural change made during one edit session. */
    UmiUiWorkspaceLayout edit_baseline;
    UmiUiWindowGroupStore edit_groups_baseline;
    bool edit_active;
    uint64_t edit_started_revision;
    uint64_t revision;
} UmiUiWorkspaceCustomisation;
/**
 * Represent the ui workspace customisation snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiUiWorkspaceCustomisationSnapshot {
    size_t layouts;
    size_t available_windows;
    size_t recent_windows;
    size_t groups;
    size_t presets;
    char active_layout_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char theme_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    bool active_layout_locked;
    bool editing;
    uint64_t revision;
} UmiUiWorkspaceCustomisationSnapshot;

/* Describe one complete panel-placement edit. Text pointers are borrowed only
 * for the duration of the apply call, so callers may safely use local buffers. */
typedef struct UmiUiWorkspacePanelSettings {
    const char *window_id;         /* Existing layout window to edit. */
    const char *placement_id;      /* Semantic dock region or floating. */
    const char *stack_id;          /* Tab stack used when the panel is docked. */
    const char *context_group_id;  /* Empty text selects no linked context. */
    UmiUiWindowGroupRole context_role; /* Source/destination routing role. */
    double x;                      /* Normalised floating left position. */
    double y;                      /* Normalised floating top position. */
    double width;                  /* Normalised floating width. */
    double height;                 /* Normalised floating height. */
    bool floating;                 /* True detaches; false docks. */
    bool auto_hidden;              /* True collapses a supported docked panel. */
} UmiUiWorkspacePanelSettings;

/**
 * Initialise ui workspace customisation from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_workspace_customisation_init(UmiUiWorkspaceCustomisation *customisation);
/**
 * Provide the ui workspace customisation add layout operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workspace_customisation_add_layout(UmiUiWorkspaceCustomisation *customisation,const UmiUiWorkspaceLayout *layout);
/**
 * Provide the ui workspace customisation activate operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workspace_customisation_activate(UmiUiWorkspaceCustomisation *customisation,const char *layout_id);
/**
 * Provide the ui workspace customisation active operation used by this module and its
 * client applications.
 */
UmiUiWorkspaceLayout *umi_ui_workspace_customisation_active(UmiUiWorkspaceCustomisation *customisation);
/**
 * Provide the ui workspace customisation active const operation used by this module and
 * its client applications.
 */
const UmiUiWorkspaceLayout *umi_ui_workspace_customisation_active_const(
    const UmiUiWorkspaceCustomisation *customisation);
/**
 * Provide the ui workspace customisation clone layout operation used by this module and
 * its client applications.
 */
UmiStatus umi_ui_workspace_customisation_clone_layout(
    UmiUiWorkspaceCustomisation *customisation,
    const char *source_layout_id,
    const char *layout_id,
    const char *name);
/**
 * Provide the ui workspace customisation remove layout operation used by this module and
 * its client applications.
 */
UmiStatus umi_ui_workspace_customisation_remove_layout(
    UmiUiWorkspaceCustomisation *customisation,
    const char *layout_id);
/**
 * Provide the ui workspace customisation rename active operation used by this module and
 * its client applications.
 */
UmiStatus umi_ui_workspace_customisation_rename_active(
    UmiUiWorkspaceCustomisation *customisation,
    const char *name);
/**
 * Provide the ui workspace customisation begin edit operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workspace_customisation_begin_edit(
    UmiUiWorkspaceCustomisation *customisation);
/**
 * Provide the ui workspace customisation commit edit operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workspace_customisation_commit_edit(
    UmiUiWorkspaceCustomisation *customisation);
/**
 * Provide the ui workspace customisation cancel edit operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workspace_customisation_cancel_edit(
    UmiUiWorkspaceCustomisation *customisation);
/**
 * Provide the ui workspace customisation open window operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workspace_customisation_open_window(
    UmiUiWorkspaceCustomisation *customisation,
    const char *tool_id,
    const char *group_id,
    bool floating,
    uint64_t opened_at_ms,
    char *out_window_id,
    size_t out_window_id_capacity);
/* Dock a window into a named region and tab stack during an edit session. */
UmiStatus umi_ui_workspace_customisation_dock_window(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    const char *placement_id,
    const char *stack_id);
/* Undock a window into bounded floating geometry during an edit session. */
UmiStatus umi_ui_workspace_customisation_float_window(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    double x,
    double y,
    double width,
    double height);
/* Collapse or restore a docked panel without removing it from the layout. */
UmiStatus umi_ui_workspace_customisation_set_auto_hidden(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    bool auto_hidden);
/* Report whether a window is currently represented by an auto-hide strip. */
bool umi_ui_workspace_customisation_window_is_auto_hidden(
    const UmiUiWorkspaceCustomisation *customisation,
    const char *window_id);
/* Return safe centre-panel defaults which a caller can adjust before apply. */
UmiUiWorkspacePanelSettings umi_ui_workspace_panel_settings_default(
    const char *window_id);
/* Apply placement, floating, auto-hide and context as one rollback-safe edit. */
UmiStatus umi_ui_workspace_customisation_apply_panel_settings(
    UmiUiWorkspaceCustomisation *customisation,
    const UmiUiWorkspacePanelSettings *settings);
/* Close a non-critical window while preserving the catalogue definition. */
UmiStatus umi_ui_workspace_customisation_close_window(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id);
/**
 * Provide the ui workspace customisation assign context group operation used by this
 * module and its client applications.
 */
UmiStatus umi_ui_workspace_customisation_assign_context_group(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    const char *context_group_id,
    UmiUiWindowGroupRole role);
/**
 * Provide the ui workspace customisation clear context group operation used by this module
 * and its client applications.
 */
UmiStatus umi_ui_workspace_customisation_clear_context_group(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id);
/**
 * Provide the ui workspace customisation set theme operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workspace_customisation_set_theme(UmiUiWorkspaceCustomisation *customisation,const UmiUiThemeProfile *theme);
/**
 * Provide the ui workspace customisation snapshot operation used by this module and its
 * client applications.
 */
void umi_ui_workspace_customisation_snapshot(const UmiUiWorkspaceCustomisation *customisation,UmiUiWorkspaceCustomisationSnapshot *out_snapshot);

/* In-canvas floating is distinct from floating=true, which existing adapters
 * interpret as a detached native window. No existing record or enum changes. */
#define UMI_UI_WORKSPACE_CANVAS_PLACEMENT "canvas"

/* A clear operation reports protected instances instead of silently removing
 * them. Counts are written only on success; this object must not alias the
 * customisation object. Definitions and product data are never removed. */
typedef struct UmiUiWorkspaceCanvasClearResult {
    size_t removed;
    size_t retained;
} UmiUiWorkspaceCanvasClearResult;

/* Create and activate a locked, empty layout without replacing any existing
 * layout. Rejects duplicate IDs, empty/overlong names or IDs, full capacity and an
 * active edit. On failure the customisation is unchanged. Borrowed strings
 * may reference the customisation. Call begin_edit separately to arrange it. */
UmiStatus umi_ui_workspace_customisation_create_blank_layout(
    UmiUiWorkspaceCustomisation *customisation,
    const char *layout_id,
    const char *name);

/* Within the existing edit transaction, remove every closable, unpinned
 * instance from the active layout, including its detached instances. Keep
 * protected instances and all catalogue entries. Remove only orphaned context
 * membership: an instance ID referenced in another stored layout retains its
 * membership. Cancel restores the layout and groups using the existing
 * baselines. out_result is optional. Failure publishes no state or result. */
UmiStatus umi_ui_workspace_customisation_clear_canvas(
    UmiUiWorkspaceCustomisation *customisation,
    UmiUiWorkspaceCanvasClearResult *out_result);

/* Place an existing instance freely inside the canvas, in normalised units.
 * Requires an active edit and an unpinned instance. A non-resizable instance
 * may move but may not change size. Invalid, non-finite or out-of-bounds
 * rectangles fail without mutation. Preserves linked context and instance ID.
 * This records canvas placement; adapters must explicitly render that mode. */
UmiStatus umi_ui_workspace_customisation_place_canvas_window(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    double x,
    double y,
    double width,
    double height);

#endif
