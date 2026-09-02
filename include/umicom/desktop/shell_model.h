/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/shell_model.h
 *
 * PURPOSE:
 *   Expose a Master Controller-owned presentation model for the federated
 *   desktop: application taskbar, bottom layout tabs, multi-monitor canvas,
 *   selectable application windows and the visual layout designer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_SHELL_MODEL_H
#define UMICOM_DESKTOP_SHELL_MODEL_H

#include "umicom/application/presentation.h"
#include "umicom/desktop/layout_designer.h"
#include "umicom/desktop/monitor_canvas.h"
#include "umicom/desktop/shell_actions.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_TASKBAR_MAX_APPLICATIONS 64U

/**
 * List the named desktop application state values accepted by this public contract.
 */
typedef enum UmiDesktopApplicationState {
    UMI_DESKTOP_APPLICATION_UNAVAILABLE = 1,
    UMI_DESKTOP_APPLICATION_STOPPED = 2,
    UMI_DESKTOP_APPLICATION_STARTING = 3,
    UMI_DESKTOP_APPLICATION_RUNNING = 4,
    UMI_DESKTOP_APPLICATION_ATTENTION = 5,
    UMI_DESKTOP_APPLICATION_FAILED = 6
} UmiDesktopApplicationState;

/**
 * Represent the desktop taskbar item data shared with callers of this public contract.
 */
typedef struct UmiDesktopTaskbarItem {
    char application_id[UMI_DESKTOP_ID_CAPACITY];
    char display_name[UMI_DESKTOP_TITLE_CAPACITY];
    char executable_name[UMI_DESKTOP_ID_CAPACITY];
    char icon_resource_id[UMI_DESKTOP_ID_CAPACITY];
    char icon_name[UMI_DESKTOP_ID_CAPACITY];
    char default_layout_id[UMI_DESKTOP_ID_CAPACITY];
    char taskbar_group[UMI_DESKTOP_ID_CAPACITY];
    UmiApplicationFamily family;
    UmiApplicationMaturity maturity;
    UmiApplicationEntryKind entry_kind;
    UmiDesktopApplicationState state;
    bool installed;
    bool compatible;
    bool enabled;
    bool visible;
    bool pinned;
    bool running;
    bool active;
    bool attention;
    uint64_t revision;
} UmiDesktopTaskbarItem;

/**
 * Represent the desktop shell tab data shared with callers of this public contract.
 */
typedef struct UmiDesktopShellTab {
    char tab_id[UMI_DESKTOP_ID_CAPACITY];
    char layout_id[UMI_DESKTOP_ID_CAPACITY];
    char label[UMI_DESKTOP_TITLE_CAPACITY];
    char category[UMI_DESKTOP_ID_CAPACITY];
    bool active;
    bool pinned;
    bool dirty;
    bool closable;
} UmiDesktopShellTab;

/**
 * Represent the desktop shell snapshot data shared with callers of this public contract.
 */
typedef struct UmiDesktopShellSnapshot {
    char active_application_id[UMI_DESKTOP_ID_CAPACITY];
    char active_layout_id[UMI_DESKTOP_ID_CAPACITY];
    char active_layout_name[UMI_DESKTOP_TITLE_CAPACITY];
    char selected_window_id[UMI_DESKTOP_ID_CAPACITY];
    size_t application_count;
    size_t pinned_application_count;
    size_t running_application_count;
    size_t tab_count;
    size_t monitor_count;
    size_t window_count;
    size_t context_group_count;
    bool designer_active;
    bool active_layout_locked;
    double viewport_width;
    double viewport_height;
    uint64_t revision;
} UmiDesktopShellSnapshot;

/**
 * Represent the desktop shell model data shared with callers of this public contract.
 */
typedef struct UmiDesktopShellModel UmiDesktopShellModel;

/**
 * Initialise desktop shell model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_desktop_shell_model_create(
    UmiDesktopRuntime *runtime,
    UmiDesktopShellModel **out_model);
/**
 * Release or reset state held by desktop shell model so the same storage can be reused
 * safely.
 */
void umi_desktop_shell_model_destroy(UmiDesktopShellModel *model);
/**
 * Provide the desktop shell model set viewport operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_shell_model_set_viewport(
    UmiDesktopShellModel *model,
    double width,
    double height);

/* Application taskbar and launcher state. */
UmiStatus umi_desktop_shell_model_set_application_presence(
    UmiDesktopShellModel *model,
    const char *application_id,
    bool installed,
    bool compatible,
    bool enabled);
/**
 * Provide the desktop shell model set application state operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_shell_model_set_application_state(
    UmiDesktopShellModel *model,
    const char *application_id,
    UmiDesktopApplicationState state);
/**
 * Provide the desktop shell model pin application operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_shell_model_pin_application(
    UmiDesktopShellModel *model,
    const char *application_id,
    bool pinned);
/**
 * Provide the desktop shell model activate application operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_shell_model_activate_application(
    UmiDesktopShellModel *model,
    const char *application_id);
/**
 * Find desktop shell model application while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_desktop_shell_model_application_at(
    const UmiDesktopShellModel *model,
    size_t visible_index,
    UmiDesktopTaskbarItem *out_item);
/**
 * Provide the desktop shell model find application operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_shell_model_find_application(
    const UmiDesktopShellModel *model,
    const char *application_id,
    UmiDesktopTaskbarItem *out_item);
/**
 * Provide the desktop application state text operation used by this module and its client
 * applications.
 */
const char *umi_desktop_application_state_text(
    UmiDesktopApplicationState state);

/* Semantic layout and desktop-canvas state. */
UmiStatus umi_desktop_shell_model_activate_layout(
    UmiDesktopShellModel *model,
    const char *layout_id);
/**
 * Provide the desktop shell model begin design operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_shell_model_begin_design(
    UmiDesktopShellModel *model,
    const char *working_layout_id,
    const char *working_name);
/**
 * Provide the desktop shell model end design operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_shell_model_end_design(
    UmiDesktopShellModel *model,
    bool commit);
/**
 * Find desktop shell model select while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_desktop_shell_model_select_at(
    UmiDesktopShellModel *model,
    double canvas_x,
    double canvas_y);
/**
 * Provide the desktop shell model move selected canvas operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_shell_model_move_selected_canvas(
    UmiDesktopShellModel *model,
    double canvas_delta_x,
    double canvas_delta_y);
/**
 * Provide the desktop shell model snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_shell_model_snapshot(
    const UmiDesktopShellModel *model,
    UmiDesktopShellSnapshot *out_snapshot);
/**
 * Find desktop shell model tab while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_desktop_shell_model_tab_at(
    const UmiDesktopShellModel *model,
    size_t index,
    UmiDesktopShellTab *out_tab);
/**
 * Find desktop shell model monitor while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_desktop_shell_model_monitor_at(
    const UmiDesktopShellModel *model,
    size_t index,
    UmiDesktopCanvasMonitor *out_monitor);
/**
 * Find desktop shell model window while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_desktop_shell_model_window_at(
    const UmiDesktopShellModel *model,
    size_t index,
    UmiDesktopCanvasWindow *out_window);
/**
 * Provide the desktop shell model designer operation used by this module and its client
 * applications.
 */
UmiDesktopLayoutDesigner *umi_desktop_shell_model_designer(
    UmiDesktopShellModel *model);
/**
 * Provide the desktop shell model runtime operation used by this module and its client
 * applications.
 */
UmiDesktopRuntime *umi_desktop_shell_model_runtime(UmiDesktopShellModel *model);

#ifdef __cplusplus
}
#endif

#endif
