/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_shell/state.h
 *
 * PURPOSE:
 *   Track active application-shell areas independently from GTK, Qt or Wt.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SHELL_STATE_H
#define UMICOM_APPLICATION_SHELL_STATE_H

#include "umicom/application_shell/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationShellState {
    char active_activity_id[UMI_APPLICATION_SHELL_ID_CAPACITY];
    char active_view_id[UMI_APPLICATION_SHELL_ID_CAPACITY];
    char focused_view_id[UMI_APPLICATION_SHELL_ID_CAPACITY];
    char active_perspective_id[UMI_APPLICATION_SHELL_ID_CAPACITY];
    int primary_sidebar_visible;
    int secondary_sidebar_visible;
    int bottom_panel_visible;
    int status_bar_visible;
    int zen_mode;
    uint64_t revision;
} UmiApplicationShellState;

void umi_application_shell_state_init(UmiApplicationShellState *state);

UmiStatus umi_application_shell_state_activate(
    UmiApplicationShellState *state,
    const char *activity_id,
    const char *view_id);

UmiStatus umi_application_shell_state_focus(
    UmiApplicationShellState *state,
    const char *view_id);

UmiStatus umi_application_shell_state_set_perspective(
    UmiApplicationShellState *state,
    const char *perspective_id);

void umi_application_shell_state_set_regions(
    UmiApplicationShellState *state,
    int primary_sidebar_visible,
    int secondary_sidebar_visible,
    int bottom_panel_visible,
    int status_bar_visible);

void umi_application_shell_state_set_zen(
    UmiApplicationShellState *state,
    int zen_mode);

#ifdef __cplusplus
}
#endif

#endif
