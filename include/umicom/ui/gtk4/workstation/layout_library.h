/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/layout_library.h
 *
 * PURPOSE:
 *   Expose a reusable native layout-library view over the existing Framework
 *   workspace owner, without adding another layout model or persistence store.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_WORKSTATION_LAYOUT_LIBRARY_H
#define UMICOM_UI_GTK4_WORKSTATION_LAYOUT_LIBRARY_H

#include <gtk/gtk.h>
#include "umicom/ui/workspace_library.h"
#include "umicom/ui/workspace_library_history.h"
#include "umicom/ui/workspace_library_checkpoint.h"

#include "umicom/ui/gtk4/workstation/library_exchange.h"
G_BEGIN_DECLS

/** Managed GTK projection; the caller retains its authoritative workspace. */
typedef struct UmiGtk4WorkspaceLayoutLibrary UmiGtk4WorkspaceLayoutLibrary;

/** Copy current library rows into caller-provided output. Called on the GTK
 * owning thread. The callback may request destruction of this controller. */
typedef UmiStatus (*UmiGtk4WorkspaceLayoutLibraryReadHandler)(
    UmiUiWorkspaceLibrarySnapshot *out_snapshot, void *context);

/** Apply one revision-checked request through the existing workspace owner.
 * Request strings are stable only during this callback. It may rebuild the
 * owning UI or destroy this controller; context must remain alive until return. */
typedef UmiStatus (*UmiGtk4WorkspaceLayoutLibraryApplyHandler)(
    const UmiUiWorkspaceLibraryRequest *request, void *context);

/** Optional whole-library storage actions, separate from active-layout Save. */
typedef enum UmiGtk4WorkspaceLayoutLibraryStorageAction {
    UMI_GTK4_WORKSPACE_LAYOUT_LIBRARY_STORAGE_SAVE = 1,
    UMI_GTK4_WORKSPACE_LAYOUT_LIBRARY_STORAGE_RESTORE = 2
} UmiGtk4WorkspaceLayoutLibraryStorageAction;

/** Copied owner evidence; GUI state never creates storage permissions. A known
 * revision of zero may describe an empty store. durable distinguishes restart
 * persistence from a memory-only service, not successful prior saving.
 * has_saved is informational: restore_enabled can permit an explicit retry
 * after an inconclusive initial probe or another writer creates a library. */
typedef struct UmiGtk4WorkspaceLayoutLibraryStorageState {
    bool supported;
    bool durable;
    bool revision_known;
    bool has_saved;
    bool save_enabled;
    bool restore_enabled;
    uint64_t storage_revision;
    uint64_t storage_generation;
    char message[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY];
} UmiGtk4WorkspaceLayoutLibraryStorageState;

/** The owner checks the click-time model revision and backend generation. Save
 * also compares the storage revision; confirmed Restore explicitly rereads the
 * current saved library rather than retrying a stale Save automatically.
 * Restore replaces the session's named layouts only after explicit confirmation.
 * This request contains no pointers and does not claim to save document drafts,
 * application files, shared context routing, credentials or all settings. */
typedef struct UmiGtk4WorkspaceLayoutLibraryStorageRequest {
    UmiGtk4WorkspaceLayoutLibraryStorageAction action;
    uint64_t expected_customisation_revision;
    uint64_t expected_storage_revision;
    uint64_t expected_storage_generation;
    bool restore_confirmed;
} UmiGtk4WorkspaceLayoutLibraryStorageRequest;

/** Read cached storage capability/revision evidence from the Framework owner.
 * This frequent view refresh must not read files or perform storage I/O.
 * Failure disables storage controls until a successful view refresh. */
typedef UmiStatus (*UmiGtk4WorkspaceLayoutLibraryStorageReadHandler)(
    UmiGtk4WorkspaceLayoutLibraryStorageState *out_state, void *context);

/** Execute the copied request through the owner's Framework storage service.
 * The callback may destroy this controller; context survives until return. */
typedef UmiStatus (*UmiGtk4WorkspaceLayoutLibraryStorageOperationHandler)(
    const UmiGtk4WorkspaceLayoutLibraryStorageRequest *request, void *context);

/** Native Duplicate accepts an empty New layout ID field. Framework then
 * suggests a collision-free identity from the displayed complete snapshot,
 * captures it before idle dispatch, and uses the ordinary revision-checked
 * apply callback. A nonempty ID remains an explicit manual request. Suggestion
 * failure submits nothing; a stale apply is never regenerated or retried. */

/** Create and initially read the native view. Callbacks/context are borrowed
 * until destruction. No model, files, application processes or database are
 * created by this component. All operations use the GTK owning thread. */
UmiStatus umi_gtk4_ws_layout_library_create(
    UmiGtk4WorkspaceLayoutLibraryReadHandler read_handler,
    UmiGtk4WorkspaceLayoutLibraryApplyHandler apply_handler,
    void *context, UmiGtk4WorkspaceLayoutLibrary **out_library);

/** Borrow a GtkPopover for gtk_menu_button_set_popover(). The controller keeps
 * an independent native reference; ordinary GTK parenting remains supported. */
GtkWidget *umi_gtk4_ws_layout_library_popover(UmiGtk4WorkspaceLayoutLibrary *library);

/** Refresh copied rows while retaining search, input drafts and selection by
 * ID. Returns BUSY during a queued/applying request. Read errors retain the last
 * displayed rows but disable actions until a successful explicit refresh. */
UmiStatus umi_gtk4_ws_layout_library_refresh(UmiGtk4WorkspaceLayoutLibrary *library);

/** Bind optional storage controls, disabled by default. Pass both handlers as
 * NULL to unbind; mismatched handlers are invalid. Context is borrowed until
 * replacement/destruction. Returns BUSY during queued or active operations.
 * Binding reads status but never saves/restores. A read failure leaves the new
 * binding installed, with storage actions disabled and explicit feedback. */
UmiStatus umi_gtk4_ws_layout_library_set_storage_handlers(
    UmiGtk4WorkspaceLayoutLibrary *library,
    UmiGtk4WorkspaceLayoutLibraryStorageReadHandler read_handler,
    UmiGtk4WorkspaceLayoutLibraryStorageOperationHandler operation_handler,
    void *context);

/** Cancel queued requests and disconnect every borrowed callback, including
 * retained old widgets. Destruction inside an owner callback is deferred until
 * that callback returns. A parent-retained popover becomes inert. */
void umi_gtk4_ws_layout_library_destroy(UmiGtk4WorkspaceLayoutLibrary *library);

/** An explicit read captures owner identity at click time. Preview performs
 * storage I/O but never adopts Save CAS evidence or authorizes Restore. */
typedef struct UmiGtk4WorkspaceLayoutLibraryPreviewRequest {
    uint64_t expected_customisation_revision;
    uint64_t expected_storage_generation;
} UmiGtk4WorkspaceLayoutLibraryPreviewRequest;
typedef UmiStatus (*UmiGtk4WorkspaceLayoutLibraryPreviewHandler)(
    const UmiGtk4WorkspaceLayoutLibraryPreviewRequest *request,
    UmiUiWorkspaceLibraryPreview *out_preview, void *context);

/** Bind an optional explicit preview callback; NULL disables it. Context is
 * borrowed until replacement/destruction, and the callback may destroy this
 * controller. No read happens while binding. Replacing storage handlers clears
 * this binding, so install the preview handler afterwards. Pending work is BUSY.
 * The displayed comparison is cleared on refresh or an action; later Restore
 * retains its existing confirmed reread semantics rather than applying a preview. */
UmiStatus umi_gtk4_ws_layout_library_set_preview_handler(
    UmiGtk4WorkspaceLayoutLibrary *library,
    UmiGtk4WorkspaceLayoutLibraryPreviewHandler handler, void *context);

/* Add file export and immutable import review to this existing library view.
 * The three callbacks share a borrowed owner context. Binding is one-time;
 * destroy the library to cancel dialogs and release retained review bytes. */
UmiStatus umi_gtk4_ws_layout_library_set_exchange_handlers(
    UmiGtk4WorkspaceLayoutLibrary *library,
    UmiGtk4WorkspaceLibraryExportHandler export_handler,
    UmiGtk4WorkspaceLibraryImportHandler import_handler,
    UmiGtk4WorkspaceLibraryImportApplyHandler apply_handler, void *context);


/* History is supplied by the authoritative workspace owner. Reads perform no
 * I/O. Apply receives the click-time revision and must refuse stale requests.
 * Both callbacks may destroy this view; their context survives until return. */
typedef UmiStatus (*UmiGtk4WorkspaceLayoutLibraryHistoryReadHandler)(
    UmiUiWorkspaceLibraryHistoryState *out_state, void *context);
typedef UmiStatus (*UmiGtk4WorkspaceLayoutLibraryHistoryApplyHandler)(
    UmiUiWorkspaceLibraryHistoryDirection direction, uint64_t expected_revision, void *context);
/* Pass both handlers or neither. Binding refreshes copied history evidence;
 * failed reads disable history controls. Pending operations return BUSY. */
UmiStatus umi_gtk4_ws_layout_library_set_history_handlers(UmiGtk4WorkspaceLayoutLibrary *library,
    UmiGtk4WorkspaceLayoutLibraryHistoryReadHandler read_handler,
    UmiGtk4WorkspaceLayoutLibraryHistoryApplyHandler apply_handler, void *context);

/* Bind after set_exchange_handlers. Add a complete frozen saved-library
 * comparison to the existing file-review controls without changing the older
 * confirmed-reread Restore action. NULL unbinds; no storage read occurs here. */
UmiStatus umi_gtk4_ws_layout_library_set_saved_review_handler(
    UmiGtk4WorkspaceLayoutLibrary *library,
    UmiGtk4WorkspaceLibrarySavedReviewHandler handler, void *context);

G_END_DECLS
#endif
