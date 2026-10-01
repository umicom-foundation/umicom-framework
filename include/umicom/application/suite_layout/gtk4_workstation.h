/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/suite_layout/gtk4_workstation.h
 *
 * PURPOSE:
 *   Bind canonical Application Suite layout selection to the existing GTK4
 *   workstation renderer for any thin Umicom application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SUITE_LAYOUT_GTK4_WORKSTATION_H
#define UMICOM_APPLICATION_SUITE_LAYOUT_GTK4_WORKSTATION_H

#include "umicom/ui/workspace_library.h"
#include "umicom/ui/workspace_library_checkpoint.h"
#include <gtk/gtk.h>

#include "umicom/application/suite_layout/runtime.h"
#include "umicom/application/suite_layout/selector_model.h"
#include "umicom/ui/gtk4/workstation/appearance_editor.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/workstation/command_bar.h"
#include "umicom/ui/gtk4/workstation/shell_header.h"
#include "umicom/ui/gtk4/workstation/workspace_layout_host.h"
#include "umicom/ui/workspace_customisation.h"
#include "umicom/ui/workspace_customisation_persistence.h"
#include "umicom/ui/workspace_checkpoint.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Values required to create one native suite workstation.
 *
 * The workstation copies the application identifier and title. It borrows the
 * panel factory callback and opaque user data for its whole lifetime, so their
 * owner must keep them valid until destruction.
 */
typedef struct UmiApplicationSuiteGtk4WorkstationConfig {
    const char *application_id;
    const char *title;
    const char *mode_badge;
    UmiGtk4WorkspaceLayoutPanelFactory panel_factory;
    void *user_data;
} UmiApplicationSuiteGtk4WorkstationConfig;

/**
 * Owned observable facts about one rendered and customisable workstation.
 *
 * Frontends and tests can copy this value without gaining access to mutable
 * internal widgets or workspace services.
 */
typedef struct UmiApplicationSuiteGtk4WorkstationSnapshot {
    char application_id[UMI_UI_ID_CAPACITY];
    char active_layout_id[UMI_UI_ID_CAPACITY];
    char active_layout_name[UMI_UI_TEXT_CAPACITY];
    size_t layout_count;
    size_t rendered_panel_count;
    size_t placeholder_count;
    size_t available_window_count;
    size_t recent_window_count;
    size_t context_group_count;
    UmiGtk4WorkstationShellHeaderSnapshot identity;
    UmiGtk4AppearanceEditorSnapshot appearance;
    int layout_locked;
    int editing_layout;
    int has_saved_layout;
    uint64_t saved_layout_at_ns;
    /* Composite change observation: includes the host, appearance, identity
     * and command bar counters. One layout activation can advance several
     * components; use the library snapshot's customisation_revision when
     * checking an individual layout-model transaction. */
    uint64_t revision;
    /* Discover the shared command centre. Rebuild consumers when this public
     * structure changes; an appended field still changes its binary size. */
    UmiGtk4WorkstationCommandBarSnapshot command_bar;
    /* Canvas panels are independent rectangles, not dock tabs or native windows.
     * Rebuild clients with this header whenever the public snapshot grows. */
    size_t canvas_panel_count;
    uint64_t source_layout_revision;
    /* Storage evidence is separate from an in-memory recovery checkpoint.
     * Rebuild callers when this copied public structure changes. */
    int checkpoint_storage_bound;
    int checkpoint_storage_durable;
    UmiStatus checkpoint_storage_status;
    uint64_t checkpoint_storage_revision;
} UmiApplicationSuiteGtk4WorkstationSnapshot;

/**
 * Represent the application suite gtk4 workstation data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationSuiteGtk4Workstation
    UmiApplicationSuiteGtk4Workstation;

/**
 * Creates a native workstation backed by canonical application layouts.
 *
 * @param config Borrowed creation values and panel factory.
 * @param out_workstation Receives the newly allocated workstation.
 * @return `UMI_STATUS_OK` when runtime, selector, customisation and widgets are
 *         ready. On failure `out_workstation` remains `NULL`.
 */
UmiStatus umi_application_suite_gtk4_workstation_create(
    const UmiApplicationSuiteGtk4WorkstationConfig *config,
    UmiApplicationSuiteGtk4Workstation **out_workstation);
/** Move this workstation's existing identity and catalogue into the native
 * GtkWindow titlebar before realization/presentation. Standalone create/widget
 * use remains unchanged unless this explicit binding succeeds. No panel,
 * selection, appearance or checkpoint state is recreated. Active layout text
 * is projected to the native title's centred context; mode remains visible.
 * The window is weakly observed and may be destroyed first. The workstation
 * owns the resulting titlebar controller. Binding the same live window twice
 * is harmless; another window, existing titlebar or realized window is rejected.
 * GTK owning thread only. This operation never presents a window.
 */
UmiStatus umi_application_suite_gtk4_workstation_bind_window(
    UmiApplicationSuiteGtk4Workstation *workstation, GtkWindow *window);
/** Focus an existing visible panel, reveal an auto-hide tool, or reopen a
 * hidden ordinary dock tool at its existing position. This does not add a new
 * instance, change its placement, save a checkpoint or unlock the layout.
 * Reopening uses the existing customisation owner and its protection checks.
 * Hidden canvas/detached instances return PERMISSION_DENIED; an unknown ID
 * returns NOT_FOUND. An existing detached window may be presented by focus.
 * GTK owning thread only. The caller's ID is copied before model publication.
 */
UmiStatus umi_application_suite_gtk4_workstation_show_window(
    UmiApplicationSuiteGtk4Workstation *workstation, const char *window_id);
/**
 * Releases workstation widgets, services and owned memory.
 *
 * @param workstation Workstation returned by the create function, or `NULL`.
 */
void umi_application_suite_gtk4_workstation_destroy(
    UmiApplicationSuiteGtk4Workstation *workstation);
/**
 * Selects and renders a product preset or a named user layout.
 *
 * @param workstation Live workstation to update.
 * @param layout_id A preset's short or application-qualified ID, or a named
 * layout's complete application-qualified ID from the layout list.
 * @return `UMI_STATUS_OK` when projection and rendering both succeed.
 */
UmiStatus umi_application_suite_gtk4_workstation_select_layout(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *layout_id);
/**
 * Add and select a locked, empty user layout without changing existing presets.
 * The ID must start with the application ID followed by a dot, so the layout
 * retains its product's panel permissions. The ID and name are copied.
 * An active edit returns BUSY. Rendering errors are
 * reported without deleting the new model, which remains available for retry.
 */
UmiStatus umi_application_suite_gtk4_workstation_create_blank_layout(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *layout_id,
    const char *name);
/**
 * Remove unpinned, closable panel instances during Edit Layout. Product data is
 * untouched; Cancel restores the previous arrangement. The optional result
 * reports removed and retained instance counts. No files are deleted.
 */
UmiStatus umi_application_suite_gtk4_workstation_clear_canvas(
    UmiApplicationSuiteGtk4Workstation *workstation,
    UmiUiWorkspaceCanvasClearResult *out_result);
/**
 * Selects one Framework appearance preset for the whole workstation.
 *
 * @param workstation Live workstation to restyle.
 * @param profile_id Stable profile identifier from the appearance catalogue.
 * @return `UMI_STATUS_OK` when the profile is valid, visible and persisted.
 */
UmiStatus umi_application_suite_gtk4_workstation_select_appearance(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *profile_id);
/**
 * Applies a complete user-owned appearance after Framework validation.
 *
 * @param workstation Live workstation to restyle.
 * @param profile Complete `umicom-custom` profile with fonts and colours.
 * @return `UMI_STATUS_OK` when the custom profile is accepted and visible.
 */
UmiStatus umi_application_suite_gtk4_workstation_apply_custom_appearance(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const UmiUiAppearanceProfile *profile);
/**
 * Copies the active appearance without exposing GTK or mutable storage.
 *
 * @param workstation Live workstation to inspect.
 * @param out_profile Receives the independent profile value.
 * @return `UMI_STATUS_OK` when an active appearance is available.
 */
UmiStatus umi_application_suite_gtk4_workstation_active_appearance(
    const UmiApplicationSuiteGtk4Workstation *workstation,
    UmiUiAppearanceProfile *out_profile);
/**
 * Update the short operational mode shown beside the application identity.
 *
 * @param workstation Live workstation whose header should change.
 * @param mode_badge Short readable mode, or an empty string to hide it.
 * @return `UMI_STATUS_OK` when copied into the shared identity component.
 */
UmiStatus umi_application_suite_gtk4_workstation_set_mode_badge(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *mode_badge);
/**
 * Begins a reversible workspace editing session.
 *
 * @param workstation Live workstation whose current state becomes the base.
 * @return `UMI_STATUS_OK` when editing starts and a rollback point exists.
 */
UmiStatus umi_application_suite_gtk4_workstation_begin_layout_edit(
    UmiApplicationSuiteGtk4Workstation *workstation);
/**
 * Commits all changes made during the active workspace editing session.
 *
 * @param workstation Workstation currently in editing mode.
 * @return `UMI_STATUS_OK` when the edit is valid and becomes current state.
 */
UmiStatus umi_application_suite_gtk4_workstation_commit_layout_edit(
    UmiApplicationSuiteGtk4Workstation *workstation);
/**
 * Cancels an edit and restores the state captured when editing began.
 *
 * @param workstation Workstation currently in editing mode.
 * @return `UMI_STATUS_OK` when the rollback state is rendered again.
 */
UmiStatus umi_application_suite_gtk4_workstation_cancel_layout_edit(
    UmiApplicationSuiteGtk4Workstation *workstation);
/**
 * Exports the current locked layout for durable application-owned storage.
 *
 * @param workstation Live workstation containing the layout to serialize.
 * @param saved_at_ns Caller-provided save time in nanoseconds.
 * @param out_text Caller-owned buffer that receives portable text.
 * @param capacity Total number of writable bytes in `out_text`.
 * @return `UMI_STATUS_OK` when the complete document fits in the buffer.
 */
UmiStatus umi_application_suite_gtk4_workstation_export_layout(
    const UmiApplicationSuiteGtk4Workstation *workstation,
    uint64_t saved_at_ns,
    char *out_text,
    size_t capacity);
/**
 * Imports portable layout text through Framework validation.
 *
 * @param workstation Live workstation that will receive the imported state.
 * @param text Read-only serialized layout document.
 * @param activate Non-zero to render the imported layout immediately.
 * @param out_report Receives copied validation and migration information.
 * @return `UMI_STATUS_OK` when the whole document is accepted.
 */
UmiStatus umi_application_suite_gtk4_workstation_import_layout(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *text,
    int activate,
    UmiUiWorkspaceImportReport *out_report);
/** Bind a borrowed Data Server for the last explicitly saved active layout.
 * Reads and validates any existing checkpoint without changing the live model.
 * The server must outlive this workstation or be unbound with NULL. Binding
 * memory storage is supported for tests and is never reported as durable.
 * Active edits reject binding with BUSY. Existing constructors perform no
 * checkpoint storage I/O. An unknown storage revision disables Save.
 */
UmiStatus umi_application_suite_gtk4_workstation_bind_checkpoint_storage(
    UmiApplicationSuiteGtk4Workstation *workstation, UmiDataServer *server);

/** Explicitly open user-config SQLite storage through the shared GTK helper.
 * The workstation owns that server until destruction. If restore_saved is
 * nonzero, restore the last explicitly saved active checkpoint after validation.
 * No saved checkpoint is a normal first run. Errors never fall back to a
 * successful memory save; defaults remain visible and the status is displayed.
 */
UmiStatus umi_application_suite_gtk4_workstation_enable_checkpoint_storage(
    UmiApplicationSuiteGtk4Workstation *workstation, int restore_saved);
/** Open this local profile's independent SQLite layout store. The profile name
 * is canonicalised and validated as a single safe identifier. Existing global
 * checkpoints remain untouched. Authentication belongs to the native host;
 * this API provides storage separation, not a security boundary against other
 * processes running as the same operating-system user. */
UmiStatus UmiApplicationSuiteEnableProfileStorage(UmiApplicationSuiteGtk4Workstation *workstation,
                                                 const char *profile, int restore_saved);


/**
 * Saves the committed active layout to bound storage, or memory when unbound.
 *
 * @param workstation Live workstation to checkpoint.
 * @param saved_at_ns Caller-provided checkpoint time in nanoseconds.
 * @return `UMI_STATUS_OK` when the checkpoint is ready for restoration.
 */
UmiStatus umi_application_suite_gtk4_workstation_save_checkpoint(
    UmiApplicationSuiteGtk4Workstation *workstation,
    uint64_t saved_at_ns);
/**
 * Restores the validated stored checkpoint, or in-memory checkpoint when unbound.
 *
 * @param workstation Live workstation with an existing checkpoint.
 * @return `UMI_STATUS_OK` when restoration and rendering succeed.
 */
UmiStatus umi_application_suite_gtk4_workstation_restore_checkpoint(
    UmiApplicationSuiteGtk4Workstation *workstation);
/**
 * Opens another instance of a catalogue tool in the chosen group.
 *
 * @param workstation Live workstation that owns the new window.
 * @param tool_id Stable catalogue panel identifier.
 * @param group_id Existing or new tab-group identifier.
 * @param floating Non-zero to start as a detached window.
 * @param opened_at_ms Caller-provided event time in milliseconds.
 * @param out_window_id Buffer receiving the generated instance identifier.
 * @param out_window_id_capacity Writable size of `out_window_id`.
 * @return `UMI_STATUS_OK` when policy allows another instance and it renders.
 */
UmiStatus umi_application_suite_gtk4_workstation_open_window(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *tool_id,
    const char *group_id,
    int floating,
    uint64_t opened_at_ms,
    char *out_window_id,
    size_t out_window_id_capacity);
/**
 * Moves one window to a group and updates its normalized geometry.
 *
 * @param workstation Live workstation containing the window.
 * @param window_id Stable window-instance identifier.
 * @param group_id Target tab-group identifier.
 * @param x Horizontal start position between zero and one.
 * @param y Vertical start position between zero and one.
 * @param width Normalized width.
 * @param height Normalized height.
 * @return `UMI_STATUS_OK` when movement is permitted and rendered.
 */
UmiStatus umi_application_suite_gtk4_workstation_move_window(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *window_id,
    const char *group_id,
    double x,
    double y,
    double width,
    double height);
/**
 * Closes one window instance without removing its catalogue tool.
 *
 * @param workstation Live workstation containing the instance.
 * @param window_id Stable instance identifier to close.
 * @return `UMI_STATUS_OK` when the window is removable and state is rebuilt.
 */
UmiStatus umi_application_suite_gtk4_workstation_close_window(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *window_id);
/**
 * Changes whether one window remains visible while its group changes.
 *
 * @param workstation Live workstation containing the instance.
 * @param window_id Stable instance identifier.
 * @param pinned Non-zero to pin the window, or zero to unpin it.
 * @return `UMI_STATUS_OK` when the state change succeeds.
 */
UmiStatus umi_application_suite_gtk4_workstation_set_window_pinned(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *window_id,
    int pinned);
/**
 * Docks or detaches one window using its saved geometry.
 *
 * @param workstation Live workstation containing the instance.
 * @param window_id Stable instance identifier.
 * @param floating Non-zero to detach, or zero to dock.
 * @return `UMI_STATUS_OK` when the panel policy permits the transition.
 */
UmiStatus umi_application_suite_gtk4_workstation_set_window_floating(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *window_id,
    int floating);
/**
 * Maximises or restores one window inside its current presentation mode.
 *
 * @param workstation Live workstation containing the instance.
 * @param window_id Stable instance identifier.
 * @param maximised Non-zero to maximise, or zero to restore.
 * @return `UMI_STATUS_OK` when the visible state is updated.
 */
UmiStatus umi_application_suite_gtk4_workstation_set_window_maximised(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *window_id,
    int maximised);
/**
 * Joins or leaves a cross-panel context group.
 *
 * @param workstation Live workstation containing the instance.
 * @param window_id Stable instance identifier.
 * @param context_group_id Stable link-group identifier, or an empty string to
 *        clear the existing link.
 * @return `UMI_STATUS_OK` when the panel supports linked context.
 */
UmiStatus umi_application_suite_gtk4_workstation_set_window_context_group(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *window_id,
    const char *context_group_id);
/**
 * Applies a complete Framework panel-editor request and rebuilds on success.
 *
 * @param workstation Live workstation containing the panel instance.
 * @param settings Borrowed atomic settings request.
 * @return `UMI_STATUS_OK` when all requested changes are valid and visible.
 */
UmiStatus umi_application_suite_gtk4_workstation_apply_panel_settings(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const UmiUiWorkspacePanelSettings *settings);
/**
 * Applies several panel edits as one policy-checked workstation transaction.
 *
 * @param workstation Live suite workstation containing the panel instances.
 * @param settings Borrowed placement requests.
 * @param setting_count Number of requests, bounded by the Framework contract.
 * @return `UMI_STATUS_OK` when the visible layout was rebuilt successfully.
 */
UmiStatus umi_application_suite_gtk4_workstation_apply_panel_batch(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const UmiUiWorkspacePanelSettings *settings,
    size_t setting_count);
/**
 * Returns the native root widget owned by the workstation.
 *
 * @param workstation Live workstation.
 * @return A borrowed widget, or `NULL` for invalid input. The caller must not
 *         destroy it separately from the workstation.
 */
GtkWidget *umi_application_suite_gtk4_workstation_widget(
    UmiApplicationSuiteGtk4Workstation *workstation);
/**
 * Copy the live UAT driver for this shared workstation. The interface borrows
 * the workstation and remains valid until the workstation is destroyed.
 */
UmiStatus umi_application_suite_gtk4_workstation_automation(
    UmiApplicationSuiteGtk4Workstation *workstation,
    UmiUiAutomationDriver *out_driver);
/**
 * Rebuilds visible panels from the current layout and panel factory state.
 *
 * Product hosts call this after a controller changes its toolkit-neutral view
 * state. The layout itself is not changed and no business command is repeated.
 */
UmiStatus umi_application_suite_gtk4_workstation_refresh(
    UmiApplicationSuiteGtk4Workstation *workstation);
/**
 * Copies observable layout, window and editing state.
 *
 * @param workstation Live workstation to inspect.
 * @return Owned snapshot value. Invalid input produces a zeroed snapshot.
 */
UmiApplicationSuiteGtk4WorkstationSnapshot
umi_application_suite_gtk4_workstation_snapshot(
    const UmiApplicationSuiteGtk4Workstation *workstation);


/** Copy the current ordered layout list on the GTK owning thread. No live
 * pointers escape; a failed read leaves output unchanged. This performs no I/O. */
UmiStatus umi_application_suite_gtk4_workstation_library_snapshot(
    UmiApplicationSuiteGtk4Workstation *workstation, UmiUiWorkspaceLibrarySnapshot *out_snapshot);

/** Apply a revision-checked library action through the same staged native
 * publication path as Layout Library. Product scope and edit gates remain in
 * Framework. A move preserves active panels and does not execute trades, save
 * documents or persist the library. Use explicit Save library for persistence.
 * Inputs are borrowed for this synchronous owner-thread call only. */
UmiStatus umi_application_suite_gtk4_workstation_library_apply(
    UmiApplicationSuiteGtk4Workstation *workstation, const UmiUiWorkspaceLibraryRequest *request);

/** Read the saved library into owned preview data without changing the live
 * layout, document/trading state, storage or cached Save revision. Call on the
 * GTK owner thread. Failure leaves output unchanged. Later Restore rereads the
 * store; this preview is not a reservation. The connected server is borrowed. */
UmiStatus umi_application_suite_gtk4_workstation_library_preview(
    UmiApplicationSuiteGtk4Workstation *workstation, UmiUiWorkspaceLibraryPreview *out_preview);

#ifdef __cplusplus
}
#endif
#endif
