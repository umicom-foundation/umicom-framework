/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/shell_actions.h
 *
 * PURPOSE:
 *   Publish the public shell actions contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_SHELL_ACTIONS_H
#define UMICOM_DESKTOP_SHELL_ACTIONS_H

#include "umicom/desktop/layout_designer.h"

#define UMI_DESKTOP_ACTION_NEW_LAYOUT      "umicom.desktop.layout.new"
#define UMI_DESKTOP_ACTION_SAVE_LAYOUT     "umicom.desktop.layout.save"
#define UMI_DESKTOP_ACTION_CANCEL_LAYOUT   "umicom.desktop.layout.cancel"
#define UMI_DESKTOP_ACTION_UNDO            "umicom.desktop.layout.undo"
#define UMI_DESKTOP_ACTION_REDO            "umicom.desktop.layout.redo"
#define UMI_DESKTOP_ACTION_DOCK_LEFT       "umicom.desktop.window.dock-left"
#define UMI_DESKTOP_ACTION_DOCK_RIGHT      "umicom.desktop.window.dock-right"
#define UMI_DESKTOP_ACTION_DOCK_TOP        "umicom.desktop.window.dock-top"
#define UMI_DESKTOP_ACTION_DOCK_BOTTOM     "umicom.desktop.window.dock-bottom"
#define UMI_DESKTOP_ACTION_FLOAT           "umicom.desktop.window.float"
#define UMI_DESKTOP_ACTION_MONITORS        "umicom.desktop.monitors.manage"

#define UMI_DESKTOP_SHELL_ACTION_COUNT 11U

typedef struct UmiDesktopShellAction {
    char action_id[UMI_DESKTOP_ID_CAPACITY];
    char label[UMI_DESKTOP_TITLE_CAPACITY];
    char tooltip[UMI_DESKTOP_DESCRIPTION_CAPACITY];
    bool enabled;
    bool destructive;
} UmiDesktopShellAction;

size_t umi_desktop_shell_action_count(void);
UmiStatus umi_desktop_shell_action_at(
    const UmiDesktopLayoutDesigner *designer,
    size_t index,
    UmiDesktopShellAction *out_action);

#endif
