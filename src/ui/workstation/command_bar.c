/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/command_bar.c
 *
 * PURPOSE:
 *   Implement a unified workstation command/search bar model for commands, panels, layouts, settings, symbols and AI actions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/command_bar.h"

UmiStatus umi_ws_command_bar_parse(const char *input, UmiWsCommandBarQuery *out_query) {
    const char *text;
    UmiWsCommandScope scope = UMI_WS_COMMAND_SCOPE_ALL;
    if (input == NULL || out_query == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    text = input;
    switch (input[0]) {
        case '>': scope = UMI_WS_COMMAND_SCOPE_COMMAND; ++text; break;
        case '@': scope = UMI_WS_COMMAND_SCOPE_SYMBOL; ++text; break;
        case '#': scope = UMI_WS_COMMAND_SCOPE_TEXT; ++text; break;
        case ':': scope = UMI_WS_COMMAND_SCOPE_LINE; ++text; break;
        case '/': scope = UMI_WS_COMMAND_SCOPE_SETTING; ++text; break;
        case '+': scope = UMI_WS_COMMAND_SCOPE_PANEL; ++text; break;
        case '?': scope = UMI_WS_COMMAND_SCOPE_AI; ++text; break;
        default: break;
    }
    while (*text == ' ') ++text;
    *out_query = (UmiWsCommandBarQuery){0};
    out_query->scope = scope;
    return umi_ws_copy_text(out_query->text, sizeof(out_query->text), text);
}

char umi_ws_command_bar_scope_prefix(UmiWsCommandScope scope) {
    switch (scope) {
        case UMI_WS_COMMAND_SCOPE_COMMAND: return '>';
        case UMI_WS_COMMAND_SCOPE_SYMBOL: return '@';
        case UMI_WS_COMMAND_SCOPE_TEXT: return '#';
        case UMI_WS_COMMAND_SCOPE_LINE: return ':';
        case UMI_WS_COMMAND_SCOPE_SETTING: return '/';
        case UMI_WS_COMMAND_SCOPE_PANEL: return '+';
        case UMI_WS_COMMAND_SCOPE_AI: return '?';
        default: return '\0';
    }
}
