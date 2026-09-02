/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_terminal_tabs.c
 *
 * PURPOSE:
 *   Verify terminal tab activation/removal state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/developer_productivity/terminal_tabs.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDeveloperTerminalTabModel model;

    umi_developer_terminal_tabs_init(&model);
    assert(umi_developer_terminal_tabs_add(
        &model, "one", "One") == UMI_STATUS_OK);
    assert(umi_developer_terminal_tabs_add(
        &model, "two", "Two") == UMI_STATUS_OK);
    assert(model.count == 2U);
    assert(model.tabs[1].active == 1);
    assert(umi_developer_terminal_tabs_activate(
        &model, "one") == UMI_STATUS_OK);
    assert(model.tabs[0].active == 1);
    return 0;
}
