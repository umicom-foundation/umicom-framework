/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_shell/state.h
 *
 * PURPOSE:
 *   Track active application-shell areas independently from GTK, Qt or Wt.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SHELL_STATE_H
#define UMICOM_APPLICATION_SHELL_STATE_H

#include "umicom/application_shell/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application shell state data shared with callers of this public contract.
 */
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

/**
 * Initialise application shell state from caller-provided values so later operations
 * receive a known state.
 */
void umi_application_shell_state_init(UmiApplicationShellState *state);

/**
 * Provide the application shell state activate operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_shell_state_activate(
    UmiApplicationShellState *state,
    const char *activity_id,
    const char *view_id);

/**
 * Provide the application shell state focus operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_shell_state_focus(
    UmiApplicationShellState *state,
    const char *view_id);

/**
 * Provide the application shell state set perspective operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_shell_state_set_perspective(
    UmiApplicationShellState *state,
    const char *perspective_id);

/**
 * Provide the application shell state set regions operation used by this module and its
 * client applications.
 */
void umi_application_shell_state_set_regions(
    UmiApplicationShellState *state,
    int primary_sidebar_visible,
    int secondary_sidebar_visible,
    int bottom_panel_visible,
    int status_bar_visible);

/**
 * Provide the application shell state set zen operation used by this module and its client
 * applications.
 */
void umi_application_shell_state_set_zen(
    UmiApplicationShellState *state,
    int zen_mode);

#ifdef __cplusplus
}
#endif

#endif
