/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/command_bar.h
 *
 * PURPOSE:
 *   Define a unified workstation command/search bar model for commands, panels, layouts, settings, symbols and AI actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_COMMAND_BAR_H
#define UMICOM_UI_WORKSTATION_COMMAND_BAR_H

#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiWsCommandScope {
    UMI_WS_COMMAND_SCOPE_ALL = 0,
    UMI_WS_COMMAND_SCOPE_COMMAND = 1,
    UMI_WS_COMMAND_SCOPE_SYMBOL = 2,
    UMI_WS_COMMAND_SCOPE_TEXT = 3,
    UMI_WS_COMMAND_SCOPE_LINE = 4,
    UMI_WS_COMMAND_SCOPE_SETTING = 5,
    UMI_WS_COMMAND_SCOPE_PANEL = 6,
    UMI_WS_COMMAND_SCOPE_AI = 7
} UmiWsCommandScope;

typedef struct UmiWsCommandBarQuery {
    UmiWsCommandScope scope;
    char text[UMI_UI_TEXT_CAPACITY];
} UmiWsCommandBarQuery;

UmiStatus umi_ws_command_bar_parse(const char *input, UmiWsCommandBarQuery *out_query);
char umi_ws_command_bar_scope_prefix(UmiWsCommandScope scope);

#ifdef __cplusplus
}
#endif

#endif
