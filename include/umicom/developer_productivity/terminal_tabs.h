/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/terminal_tabs.h
 *
 * PURPOSE:
 *   Track terminal tab ordering/pinning independently from terminal processes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_TERMINAL_TABS_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_TERMINAL_TABS_H

#include "umicom/developer_productivity/types.h"
#include "umicom/developer_productivity/terminal_workspace.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeveloperTerminalTab {
    char session_id[UMI_DEVELOPER_PRODUCTIVITY_ID_CAPACITY];
    char title[UMI_DEVELOPER_PRODUCTIVITY_NAME_CAPACITY];
    int pinned;
    int active;
} UmiDeveloperTerminalTab;

typedef struct UmiDeveloperTerminalTabModel {
    UmiDeveloperTerminalTab tabs[UMI_DEVELOPER_TERMINAL_SESSION_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiDeveloperTerminalTabModel;

void umi_developer_terminal_tabs_init(
    UmiDeveloperTerminalTabModel *model);

UmiStatus umi_developer_terminal_tabs_add(
    UmiDeveloperTerminalTabModel *model,
    const char *session_id,
    const char *title);

UmiStatus umi_developer_terminal_tabs_activate(
    UmiDeveloperTerminalTabModel *model,
    const char *session_id);

UmiStatus umi_developer_terminal_tabs_remove(
    UmiDeveloperTerminalTabModel *model,
    const char *session_id);

#ifdef __cplusplus
}
#endif

#endif
