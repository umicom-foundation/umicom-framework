/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/terminal_tabs.h
 *
 * PURPOSE:
 *   Track terminal tab ordering/pinning independently from terminal processes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_TERMINAL_TABS_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_TERMINAL_TABS_H

#include "umicom/developer_productivity/types.h"
#include "umicom/developer_productivity/terminal_workspace.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer terminal tab data shared with callers of this public contract.
 */
typedef struct UmiDeveloperTerminalTab {
    char session_id[UMI_DEVELOPER_PRODUCTIVITY_ID_CAPACITY];
    char title[UMI_DEVELOPER_PRODUCTIVITY_NAME_CAPACITY];
    int pinned;
    int active;
} UmiDeveloperTerminalTab;

/**
 * Represent the developer terminal tab model data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperTerminalTabModel {
    UmiDeveloperTerminalTab tabs[UMI_DEVELOPER_TERMINAL_SESSION_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiDeveloperTerminalTabModel;

/**
 * Initialise developer terminal tabs from caller-provided values so later operations
 * receive a known state.
 */
void umi_developer_terminal_tabs_init(
    UmiDeveloperTerminalTabModel *model);

/**
 * Add developer terminal tabs only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_developer_terminal_tabs_add(
    UmiDeveloperTerminalTabModel *model,
    const char *session_id,
    const char *title);

/**
 * Provide the developer terminal tabs activate operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_terminal_tabs_activate(
    UmiDeveloperTerminalTabModel *model,
    const char *session_id);

/**
 * Remove developer terminal tabs while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_developer_terminal_tabs_remove(
    UmiDeveloperTerminalTabModel *model,
    const char *session_id);

#ifdef __cplusplus
}
#endif

#endif
