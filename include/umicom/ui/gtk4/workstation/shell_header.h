/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/shell_header.h
 *
 * PURPOSE:
 *   Render the reusable application identity shown in an Umicom workstation
 *   header and the reusable startup surface shown while a workstation is
 *   preparing its Framework-owned services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_SHELL_HEADER_H
#define UMICOM_UI_GTK4_WORKSTATION_SHELL_HEADER_H

#include <stdbool.h>
#include <stdint.h>

#include <gtk/gtk.h>

#include "umicom/ui/appearance.h"
#include "umicom/application/launch_selection.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Request canonical themed and operating-system icons for this native window.
 *
 * The shared resolver finds branding/umicom-icon.svg beside the executable
 * (or in a development working directory). Its directory is added to the
 * display's icon theme, and the window requests the name "umicom-icon".
 * On Windows, resource 101 from the shared branding template supplies both
 * HWND icon sizes after realization and mapping, without replacing a custom
 * titlebar. Calling this repeatedly is safe and never presents the window.
 * Returns OK when either themed identity or native assignment is configured
 * (an unrealized Windows window receives its icon later), INVALID_ARGUMENT for
 * an invalid window, UNAVAILABLE without the backend, or NOT_FOUND without a
 * usable canonical resource. Missing assets leave existing icons unchanged.
 * The operating system may choose not to display GTK window icons.
 */
UmiStatus umi_gtk4_ws_apply_window_identity(GtkWindow *window);

/**
 * Represent the gtk4 workstation shell header data shared with callers of this public
 * contract.
 */
typedef struct UmiGtk4WorkstationShellHeader
    UmiGtk4WorkstationShellHeader;

/** A managed native titlebar, installed in GtkWindow's titlebar slot rather
 * than appended to the document/content hierarchy. */
typedef struct UmiGtk4WorkstationWindowTitlebar UmiGtk4WorkstationWindowTitlebar;

/** Copied titlebar identity and document/project context for native acceptance. */
typedef struct UmiGtk4WorkstationWindowTitlebarSnapshot {
    char title[UMI_UI_TEXT_CAPACITY];
    char context[768];
    char icon_resource[UMI_UI_APPEARANCE_RESOURCE_CAPACITY];
    int icon_visible;
    int installed;
} UmiGtk4WorkstationWindowTitlebarSnapshot;

/**
 * Describe how a Framework host should open a selected Umicom application.
 *
 * STANDARD delegates placement to the active host policy. NEW_WINDOW asks for
 * an independent top-level application window. The default GTK4 handler starts
 * the canonical native GUI companion recorded by the Framework portfolio;
 * console verification tools are never graphical-launch fallbacks.
 */
typedef enum UmiGtk4WorkstationApplicationOpenMode {
    UMI_GTK4_WORKSTATION_APPLICATION_OPEN_STANDARD = 1,
    UMI_GTK4_WORKSTATION_APPLICATION_OPEN_NEW_WINDOW = 2
} UmiGtk4WorkstationApplicationOpenMode;

/**
 * Receive a stable application identifier selected from the Framework
 * portfolio. A universal host may replace the default process launcher with an
 * application-surface-session implementation without changing the header UI.
 */
typedef UmiStatus (*UmiGtk4WorkstationApplicationOpenHandler)(
    const char *application_id,
    UmiGtk4WorkstationApplicationOpenMode mode,
    void *user_data);

/**
 * Text and resource values used while creating one application identity.
 *
 * All strings are borrowed during creation and copied into Framework-owned
 * storage. `resource_root` is optional. When it is empty, relative resources
 * are looked up beside the running executable so installed applications do
 * not depend on the terminal's current directory.
 * A compact header shows one title line with an 18-logical-pixel SVG mark.
 * Its subtitle is retained in the snapshot, tooltip and accessible description
 * rather than adding another visible line. Noncompact headers show the subtitle
 * below the title and use a 24-logical-pixel mark.
 */
typedef struct UmiGtk4WorkstationShellHeaderConfig {
    const char *application_id;
    const char *title;
    const char *subtitle;
    const char *mode_badge;
    const char *resource_root;
    bool compact;
} UmiGtk4WorkstationShellHeaderConfig;

/**
 * A value copy of the identity currently presented by the native header.
 *
 * Tests, status panels and application diagnostics can read this structure
 * without keeping pointers to GTK widgets or mutable Framework state.
 */
typedef struct UmiGtk4WorkstationShellHeaderSnapshot {
    char application_id[UMI_UI_ID_CAPACITY];
    char title[UMI_UI_TEXT_CAPACITY];
    char subtitle[UMI_UI_TEXT_CAPACITY];
    char mode_badge[UMI_UI_TEXT_CAPACITY];
    char icon_resource[UMI_UI_APPEARANCE_RESOURCE_CAPACITY];
    int icon_visible;
    int compact;
    uint64_t revision;
} UmiGtk4WorkstationShellHeaderSnapshot;

/**
 * Framework-owned startup surface shown before a workstation is ready.
 *
 * The controller stores only presentation state. Product startup, security,
 * connectivity and service initialisation remain with their owning services.
 */
typedef struct UmiGtk4WorkstationStartupSplash
    UmiGtk4WorkstationStartupSplash;

/**
 * Values used to create one branded Umicom startup surface.
 *
 * All strings are borrowed during creation and copied where they become part
 * of observable state. `resource_root` and `icon_resource` are optional.
 */
typedef struct UmiGtk4WorkstationStartupSplashConfig {
    const char *application_id;
    const char *title;
    const char *subtitle;
    const char *status;
    const char *mode_badge;
    const char *resource_root;
    const char *icon_resource;
} UmiGtk4WorkstationStartupSplashConfig;

/** Copied startup state suitable for tests and application diagnostics. */
typedef struct UmiGtk4WorkstationStartupSplashSnapshot {
    char application_id[UMI_UI_ID_CAPACITY];
    char title[UMI_UI_TEXT_CAPACITY];
    char subtitle[UMI_UI_TEXT_CAPACITY];
    char status[UMI_UI_TEXT_CAPACITY];
    char mode_badge[UMI_UI_TEXT_CAPACITY];
    char icon_resource[UMI_UI_APPEARANCE_RESOURCE_CAPACITY];
    double progress;
    int icon_visible;
    int progress_visible;
    uint64_t revision;
} UmiGtk4WorkstationStartupSplashSnapshot;

/** Install a compact topmost GtkHeaderBar before the window is realized.
 * Reuses the canonical SVG identity at the left, centres document/project
 * context, and places the existing application catalogue and new-window
 * actions at right before GTK-managed minimise/maximise/close controls.
 * GTK's window handle owns drag/double-click behaviour; platform close guards
 * still run through GtkWindow. Native OS-drawn controls depend on the backend.
 * The optional context_title_prefix names an established window-title product
 * prefix (for example "Umicom Studio"). notify::title strips only that exact
 * prefix or config->title plus an em-dash separator; the actual window title
 * is never modified. Config strings are copied; compact mode is forced and
 * subtitle and badge are omitted. Application actions reuse the same managed
 * identity controller; no second inner identity or launcher model is created.
 * The managed close button stays hidden because GTK supplies native Close.
 * The controller retains its widget and weakly observes the window. It must
 * be destroyed on the GTK owning thread. Existing titlebars/realized windows
 * return INVALID_STATE rather than being replaced. No window is presented.
 */
UmiStatus umi_gtk4_ws_window_titlebar_create(
    GtkWindow *window, const UmiGtk4WorkstationShellHeaderConfig *config,
    const char *context_title_prefix, UmiGtk4WorkstationWindowTitlebar **out_titlebar);
/** Transfer one existing managed identity into a topmost titlebar before
 * window realization. The header must be unparented or in a GtkBox belonging
 * to this un-realized window/unparented composition. No new catalogue,
 * selection, appearance or operational-mode state is created or reset.
 * Success transfers header ownership to the titlebar; callers may retain only
 * a borrowed alias and must not separately destroy it. Failure changes neither
 * the header nor its parent and retains caller ownership. A header may be
 * transferred only once; in-flight application operations return BUSY.
 * Existing compact presentation is preserved. No window is presented.
 */
UmiStatus umi_gtk4_ws_window_titlebar_create_from_header(
    GtkWindow *window, UmiGtk4WorkstationShellHeader *identity,
    const char *context_title_prefix, UmiGtk4WorkstationWindowTitlebar **out_titlebar);
/** Disconnect title/identity and moved application-control callbacks before
 * releasing owned references, including externally retained buttons. An
 * attached widget remains parent-owned; its GTK window controls keep their
 * ordinary window lifetime and never borrow this released controller. */
void umi_gtk4_ws_window_titlebar_destroy(UmiGtk4WorkstationWindowTitlebar *titlebar);
/** Borrow the actual GtkHeaderBar installed as the window's titlebar. */
GtkWidget *umi_gtk4_ws_window_titlebar_widget(UmiGtk4WorkstationWindowTitlebar *titlebar);
/** Follow the existing Framework appearance profile without changing artwork
 * geometry, user fonts, or the document hierarchy. Missing marks stay hidden. */
UmiStatus umi_gtk4_ws_window_titlebar_apply_appearance(
    UmiGtk4WorkstationWindowTitlebar *titlebar, const UmiUiAppearanceProfile *profile);
/** Copy observations without exposing mutable identity or native window state. */
UmiGtk4WorkstationWindowTitlebarSnapshot umi_gtk4_ws_window_titlebar_snapshot(
    const UmiGtk4WorkstationWindowTitlebar *titlebar);

/** Return safe creation values for one named Umicom application. */
UmiGtk4WorkstationShellHeaderConfig
umi_gtk4_ws_shell_header_config_default(
    const char *application_id,
    const char *title);

/**
 * Create an updateable identity controller and its GTK widget.
 *
 * The returned controller stores no business state. The GTK widget becomes
 * owned by its normal GTK parent after the caller appends it to a container.
 * The controller retains an independent reference until it is destroyed.
 */
UmiStatus umi_gtk4_ws_shell_header_create_managed(
    const UmiGtk4WorkstationShellHeaderConfig *config,
    UmiGtk4WorkstationShellHeader **out_header);

/** Release the controller after, or immediately before, its widget tree.
 * May be called from an application-open callback on the GTK owning thread.
 * The caller must immediately stop using the header. An active request may
 * finish; later batch requests are cancelled, and storage is released only
 * after the active dispatch has completed its report and unwound.
 */
void umi_gtk4_ws_shell_header_destroy(
    UmiGtk4WorkstationShellHeader *header);

/** Borrow the GTK root that the application places in its top-level bar. */
GtkWidget *umi_gtk4_ws_shell_header_widget(
    UmiGtk4WorkstationShellHeader *header);

/**
 * Select the SVG mark described by a validated appearance profile.
 *
 * The mark is a Framework-owned packaged asset. If the asset is missing, the
 * image remains hidden and the readable product title stays visible; packaging
 * conformance diagnostics must report the missing asset rather than drawing a
 * substitute logo.
 */
UmiStatus umi_gtk4_ws_shell_header_apply_appearance(
    UmiGtk4WorkstationShellHeader *header,
    const UmiUiAppearanceProfile *profile);

/** Update the readable title, workspace subtitle and optional mode badge. */
UmiStatus umi_gtk4_ws_shell_header_set_text(
    UmiGtk4WorkstationShellHeader *header,
    const char *title,
    const char *subtitle,
    const char *mode_badge);

/**
 * Replace the default executable launcher with a Framework host callback.
 *
 * Passing NULL restores the default process launcher. The callback is borrowed
 * and must remain valid until it is replaced or the header is destroyed.
 * Replacement refreshes availability and returns BUSY during batch dispatch.
 */
UmiStatus umi_gtk4_ws_shell_header_set_application_open_handler(
    UmiGtk4WorkstationShellHeader *header,
    UmiGtk4WorkstationApplicationOpenHandler handler,
    void *user_data);

/** Refresh native GUI discovery, or availability delegated to the active host.
 * All picker operations must run on the header's GTK owning thread.
 * Executable discovery does not establish startup health or feature readiness.
 * Existing selections survive availability changes so failed requests can be
 * retried after an application becomes available, or cleared explicitly.
 * Returns BUSY during batch dispatch.
 */
UmiStatus umi_gtk4_ws_shell_header_catalogue_refresh(
    UmiGtk4WorkstationShellHeader *header);

/** Select one application through the shared launch-selection model.
 * Filtering changes only row visibility and never changes this selection.
 * Returns BUSY during batch dispatch.
 */
UmiStatus umi_gtk4_ws_shell_header_catalogue_set_selected(
    UmiGtk4WorkstationShellHeader *header,
    const char *application_id,
    bool selected);

/** Copy the complete selection, including applications hidden by the search. */
UmiStatus umi_gtk4_ws_shell_header_catalogue_snapshot(
    const UmiGtk4WorkstationShellHeader *header,
    UmiApplicationLaunchSelectionSnapshot *out_snapshot);

/** Dispatch every selected request without closing the picker.
 * Accepted requests are deselected; failed requests remain selected for retry.
 * Acceptance does not imply a running process or a ready application window.
 * Reentrant calls return BUSY without changing the caller's report.
 */
UmiStatus umi_gtk4_ws_shell_header_catalogue_dispatch(
    UmiGtk4WorkstationShellHeader *header,
    UmiApplicationLaunchDispatchReport *out_report);

/**
 * Select which universal application controls are visible.
 *
 * This operation changes presentation only. Application authorisation and
 * launch availability remain governed by the canonical portfolio and caller
 * policy.
 */
UmiStatus umi_gtk4_ws_shell_header_set_application_controls(
    UmiGtk4WorkstationShellHeader *header,
    bool show_catalogue,
    bool show_new_window,
    bool show_close);

/** Copy observable identity state without exposing mutable GTK objects. */
UmiGtk4WorkstationShellHeaderSnapshot
umi_gtk4_ws_shell_header_snapshot(
    const UmiGtk4WorkstationShellHeader *header);

/**
 * Create the original label-only header API retained for source compatibility.
 *
 * New application workstations should use `create_managed` so their icon can
 * follow appearance changes. The returned widget has a floating GTK reference
 * which a normal GTK parent consumes, preserving the original ownership rules.
 */
GtkWidget *umi_gtk4_ws_shell_header_create(
    const char *title,
    const char *subtitle,
    const char *mode_badge,
    bool compact);

/** Return safe startup values for one named Umicom application. */
UmiGtk4WorkstationStartupSplashConfig
umi_gtk4_ws_startup_splash_config_default(
    const char *application_id,
    const char *title);

/**
 * Create the reusable startup presentation and its update controller.
 *
 * The returned root is borrowed. A normal GTK parent presents the widget
 * tree, while the small controller retains the reference required for safe
 * startup-to-workspace replacement and is later destroyed by the caller.
 */
UmiStatus umi_gtk4_ws_startup_splash_create(
    const UmiGtk4WorkstationStartupSplashConfig *config,
    UmiGtk4WorkstationStartupSplash **out_splash);

/** Release the startup controller and its retained widget reference. */
void umi_gtk4_ws_startup_splash_destroy(
    UmiGtk4WorkstationStartupSplash *splash);

/** Borrow the startup root for placement in an application window. */
GtkWidget *umi_gtk4_ws_startup_splash_widget(
    UmiGtk4WorkstationStartupSplash *splash);

/** Update readable startup status without rebuilding the widget tree. */
UmiStatus umi_gtk4_ws_startup_splash_set_status(
    UmiGtk4WorkstationStartupSplash *splash,
    const char *status,
    const char *mode_badge);

/**
 * Update bounded startup progress.
 *
 * `progress` is clamped to the inclusive range zero to one. Set
 * `show_progress` to zero when startup has no meaningful measurable progress.
 */
UmiStatus umi_gtk4_ws_startup_splash_set_progress(
    UmiGtk4WorkstationStartupSplash *splash,
    double progress,
    int show_progress);

/** Copy startup presentation state without exposing mutable GTK objects. */
UmiGtk4WorkstationStartupSplashSnapshot
umi_gtk4_ws_startup_splash_snapshot(
    const UmiGtk4WorkstationStartupSplash *splash);

#ifdef __cplusplus
}
#endif

#endif
