/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_command_bar.c
 *
 * PURPOSE:
 *   Implement the test command bar behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include "umicom/ui/workstation/command_bar.h"
int main(void) {
    UmiWsCommandBarQuery q;
    if (umi_ws_command_bar_parse("+ Memory", &q) != UMI_STATUS_OK) return 1;
    if (q.scope != UMI_WS_COMMAND_SCOPE_PANEL || q.text[0] != 'M') return 2;
    if (umi_ws_command_bar_scope_prefix(UMI_WS_COMMAND_SCOPE_AI) != '?') return 3;
    puts("command bar: ok");
    return 0;
}
