/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading_ui/gtk4/trading_suite_workstation.h
 *
 * PURPOSE:
 *   Compose a complete GTK4 professional trading workstation from canonical
 *   Application Suite layouts, guarded trading actions and simulation market
 *   data without moving reusable behaviour into a product repository.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_UI_GTK4_TRADING_SUITE_WORKSTATION_H
#define UMICOM_TRADING_UI_GTK4_TRADING_SUITE_WORKSTATION_H

#include "umicom/ui/workspace_library_exchange.h"
#include <stddef.h>

#include "umicom/ui/workspace_library.h"
#include "umicom/ui/workspace_library_checkpoint.h"
#include <gtk/gtk.h>

#include "umicom/application/suite_layout/gtk4_workstation.h"
#include "umicom/trading_ui/action_controller.h"
#include "umicom/trading/chart_persistence.h"
#include "umicom/trading_ui/simulation_market.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the gtk4 trading suite workstation config data shared with callers of this
 * public contract.
 */
typedef struct UmiGtk4TradingSuiteWorkstationConfig {
    UmiTradingWorkspace *workspace;
    const char *application_id;
    const char *title;
    int seed_simulation_market;
    int animate_simulation_market;
    int allow_live_environment;
    unsigned int simulation_step_interval_ms;
} UmiGtk4TradingSuiteWorkstationConfig;

/**
 * Represent the gtk4 trading suite workstation snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiGtk4TradingSuiteWorkstationSnapshot {
    UmiApplicationSuiteGtk4WorkstationSnapshot layout;
    UmiTradingWorkspaceSnapshot trading;
    UmiTradingUiControllerSnapshot controller;
    size_t simulation_instrument_count;
    uint64_t simulation_sequence;
    int simulation_running;
} UmiGtk4TradingSuiteWorkstationSnapshot;

/**
 * Represent the gtk4 trading suite workstation data shared with callers of this public
 * contract.
 */
typedef struct UmiGtk4TradingSuiteWorkstation UmiGtk4TradingSuiteWorkstation;

/**
 * Provide the gtk4 trading suite workstation config default operation used by this module
 * and its client applications.
 */
UmiGtk4TradingSuiteWorkstationConfig
umi_gtk4_trading_suite_workstation_config_default(
    UmiTradingWorkspace *workspace);
/**
 * Initialise gtk4 trading suite workstation from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_gtk4_trading_suite_workstation_create(
    const UmiGtk4TradingSuiteWorkstationConfig *config,
    UmiGtk4TradingSuiteWorkstation **out_workstation);
/** Bind the existing shared product identity to a native window before its
 * first realization. The Framework owns titlebar composition and lifetime;
 * no application catalogue or appearance state is duplicated. */
UmiStatus umi_gtk4_trading_suite_workstation_bind_window(
    UmiGtk4TradingSuiteWorkstation *workstation, GtkWindow *window);
/** Explicitly enable shared user-local SQLite layout checkpoints.
 * Native launchers opt in after construction; constructors do no checkpoint I/O.
 * A failed restore leaves the current layout visible and reports its error. */
UmiStatus umi_gtk4_trading_suite_workstation_enable_checkpoint_storage(
    UmiGtk4TradingSuiteWorkstation *workstation, int restore_saved);

/**
 * Release or reset state held by gtk4 trading suite workstation so the same storage can be
 * reused safely.
 */
void umi_gtk4_trading_suite_workstation_destroy(
    UmiGtk4TradingSuiteWorkstation *workstation);
/**
 * Provide the gtk4 trading suite workstation widget operation used by this module and its
 * client applications.
 */
GtkWidget *umi_gtk4_trading_suite_workstation_widget(
    UmiGtk4TradingSuiteWorkstation *workstation);
/**
 * Provide the gtk4 trading suite workstation select layout operation used by this module
 * and its client applications.
 */
UmiStatus umi_gtk4_trading_suite_workstation_select_layout(
    UmiGtk4TradingSuiteWorkstation *workstation,
    const char *layout_id);
/* Apply a shared appearance preset without adding product-specific CSS. */
UmiStatus umi_gtk4_trading_suite_workstation_select_appearance(
    UmiGtk4TradingSuiteWorkstation *workstation,
    const char *profile_id);
/* Validate and apply the user-owned custom fonts, density and colours. */
UmiStatus umi_gtk4_trading_suite_workstation_apply_custom_appearance(
    UmiGtk4TradingSuiteWorkstation *workstation,
    const UmiUiAppearanceProfile *profile);
/* Copy the active appearance for status views and automated verification. */
UmiStatus umi_gtk4_trading_suite_workstation_active_appearance(
    const UmiGtk4TradingSuiteWorkstation *workstation,
    UmiUiAppearanceProfile *out_profile);
/**
 * Provide the gtk4 trading suite workstation begin layout edit operation used by this
 * module and its client applications.
 */
UmiStatus umi_gtk4_trading_suite_workstation_begin_layout_edit(
    UmiGtk4TradingSuiteWorkstation *workstation);
/**
 * Provide the gtk4 trading suite workstation commit layout edit operation used by this
 * module and its client applications.
 */
UmiStatus umi_gtk4_trading_suite_workstation_commit_layout_edit(
    UmiGtk4TradingSuiteWorkstation *workstation);
/**
 * Provide the gtk4 trading suite workstation cancel layout edit operation used by this
 * module and its client applications.
 */
UmiStatus umi_gtk4_trading_suite_workstation_cancel_layout_edit(
    UmiGtk4TradingSuiteWorkstation *workstation);
/* Forward portable layout persistence to the shared application workstation. */
UmiStatus umi_gtk4_trading_suite_workstation_export_layout(
    const UmiGtk4TradingSuiteWorkstation *workstation,
    uint64_t saved_at_ns,
    char *out_text,
    size_t capacity);
/**
 * Provide the gtk4 trading suite workstation import layout operation used by this module
 * and its client applications.
 */
UmiStatus umi_gtk4_trading_suite_workstation_import_layout(
    UmiGtk4TradingSuiteWorkstation *workstation,
    const char *text,
    int activate,
    UmiUiWorkspaceImportReport *out_report);
/* Save or restore the session recovery checkpoint used by the header buttons. */
UmiStatus umi_gtk4_trading_suite_workstation_save_checkpoint(
    UmiGtk4TradingSuiteWorkstation *workstation,
    uint64_t saved_at_ns);
/**
 * Provide the gtk4 trading suite workstation restore checkpoint operation used by this
 * module and its client applications.
 */
UmiStatus umi_gtk4_trading_suite_workstation_restore_checkpoint(
    UmiGtk4TradingSuiteWorkstation *workstation);
/**
 * Provide the gtk4 trading suite workstation open window operation used by this module and
 * its client applications.
 */
UmiStatus umi_gtk4_trading_suite_workstation_open_window(
    UmiGtk4TradingSuiteWorkstation *workstation,
    const char *tool_id,
    const char *region_id,
    int floating,
    uint64_t opened_at_ms,
    char *out_window_id,
    size_t out_window_id_capacity);
/**
 * Provide the gtk4 trading suite workstation move window operation used by this module and
 * its client applications.
 */
UmiStatus umi_gtk4_trading_suite_workstation_move_window(
    UmiGtk4TradingSuiteWorkstation *workstation,
    const char *window_id,
    const char *region_id,
    double x,
    double y,
    double width,
    double height);
/**
 * Provide the gtk4 trading suite workstation close window operation used by this module
 * and its client applications.
 */
UmiStatus umi_gtk4_trading_suite_workstation_close_window(
    UmiGtk4TradingSuiteWorkstation *workstation,
    const char *window_id);
/* Delegate one complete panel edit to the Framework suite workstation. */
UmiStatus umi_gtk4_trading_suite_workstation_apply_panel_settings(
    UmiGtk4TradingSuiteWorkstation *workstation,
    const UmiUiWorkspacePanelSettings *settings);
/** Apply several panel edits as one policy-checked, all-or-nothing operation. */
UmiStatus umi_gtk4_trading_suite_workstation_apply_panel_batch(
    UmiGtk4TradingSuiteWorkstation *workstation,
    const UmiUiWorkspacePanelSettings *settings,
    size_t setting_count);
/**
 * Provide the gtk4 trading suite workstation refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_gtk4_trading_suite_workstation_refresh(
    UmiGtk4TradingSuiteWorkstation *workstation);
/**
 * Provide the gtk4 trading suite workstation snapshot operation used by this module and
 * its client applications.
 */
UmiGtk4TradingSuiteWorkstationSnapshot
umi_gtk4_trading_suite_workstation_snapshot(
    UmiGtk4TradingSuiteWorkstation *workstation);
/**
 * Provide the gtk4 trading suite workstation controller operation used by this module and
 * its client applications.
 */
UmiTradingUiController *umi_gtk4_trading_suite_workstation_controller(
    UmiGtk4TradingSuiteWorkstation *workstation);


/** Copy the current ordered layout list on the GTK owning thread. No live
 * pointers escape; a failed read leaves output unchanged. This performs no I/O. */
UmiStatus umi_gtk4_trading_suite_workstation_library_snapshot(
    UmiGtk4TradingSuiteWorkstation *workstation, UmiUiWorkspaceLibrarySnapshot *out_snapshot);

/** Apply a revision-checked library action through the same staged native
 * publication path as Layout Library. Product scope and edit gates remain in
 * Framework. A move preserves active panels and does not execute trades, save
 * documents or persist the library. Use explicit Save library for persistence.
 * Inputs are borrowed for this synchronous owner-thread call only. */
UmiStatus umi_gtk4_trading_suite_workstation_library_apply(
    UmiGtk4TradingSuiteWorkstation *workstation, const UmiUiWorkspaceLibraryRequest *request);

/** Bind an existing borrowed Data Server for explicit layout/library saves.
 * The server outlives the workstation or is unbound with NULL. This probes saved
 * evidence but never restores automatically, opens a path or enables trading.
 * Memory backends remain visibly non-durable. Active edits return BUSY. */
UmiStatus umi_gtk4_trading_suite_workstation_bind_checkpoint_storage(
    UmiGtk4TradingSuiteWorkstation *workstation, UmiDataServer *server);

/** Read the saved library into owned preview data without changing the live
 * layout, document/trading state, storage or cached Save revision. Call on the
 * GTK owner thread. Failure leaves output unchanged. Later Restore rereads the
 * store; this preview is not a reservation. The connected server is borrowed. */
UmiStatus umi_gtk4_trading_suite_workstation_library_preview(
    UmiGtk4TradingSuiteWorkstation *workstation, UmiUiWorkspaceLibraryPreview *out_preview);

/* Use Framework-owned profile-specific layout persistence after local sign-in. */
UmiStatus UmiGtk4TradingSuiteEnableProfileStorage(UmiGtk4TradingSuiteWorkstation *workstation, const char *profile, int restore_saved);

/* Chart saves share the established profile database through a separate
 * owned connection. They remain explicit and never restore with layout loads.
 * Borrowed binding below performs no I/O and does not change layout storage.
 * The caller retains server ownership until unbound or workstation destroyed. */
UmiStatus UmiGtk4TradingSuiteBindChartStorage(UmiGtk4TradingSuiteWorkstation *workstation,
    UmiDataServer *server, const char *scope);
/* Borrow the toolkit-neutral coordinator for thin host adapters and tests. */
UmiTradingChartPersistence *UmiGtk4TradingSuiteChartPersistence(UmiGtk4TradingSuiteWorkstation *workstation);

/* Portable library exchange uses the same owner and scope as the native
 * Layout Library. Review owns copied bytes; apply refuses a stale workspace.
 * Calls run on the GTK owner thread. They never save storage or place orders.
 * The caller destroys the review after apply or cancellation. */
UmiStatus umi_gtk4_trading_suite_workstation_library_export(
    UmiGtk4TradingSuiteWorkstation *workstation, char *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_gtk4_trading_suite_workstation_library_import_review(
    UmiGtk4TradingSuiteWorkstation *workstation, const void *bytes, size_t size, UmiUiWorkspaceLibraryImport **out_review);
UmiStatus umi_gtk4_trading_suite_workstation_library_import_apply(
    UmiGtk4TradingSuiteWorkstation *workstation, const UmiUiWorkspaceLibraryImport *review);

/* Layout recovery stays with Framework. This owner-thread call neither
 * saves product data nor runs a trade, payment or project command. */
UmiStatus umi_gtk4_trading_suite_workstation_library_history_read(UmiGtk4TradingSuiteWorkstation *workstation,
    UmiUiWorkspaceLibraryHistoryState *out_state);
/* Layout recovery stays with Framework. This owner-thread call neither
 * saves product data nor runs a trade, payment or project command. */
UmiStatus umi_gtk4_trading_suite_workstation_library_history_navigate(UmiGtk4TradingSuiteWorkstation *workstation,
    UmiUiWorkspaceLibraryHistoryDirection direction, uint64_t expected_revision);

#ifdef __cplusplus
}
#endif
#endif
