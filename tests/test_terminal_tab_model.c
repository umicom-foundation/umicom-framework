/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_terminal_tab_model.c
 *
 * PURPOSE:
 *   Implement the test terminal tab model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework terminal tab-model tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/terminal/tab_model.h"

int main(void)
{
    UmiTerminalTabModel *model = NULL;
    UmiTerminalTabSnapshot tab;
    assert(umi_terminal_tab_model_create(&model) == UMI_STATUS_OK);
    assert(umi_terminal_tab_model_add(model, "one", "One", "bash") == UMI_STATUS_OK);
    assert(umi_terminal_tab_model_add(model, "two", "Two", "bash") == UMI_STATUS_OK);
    assert(strcmp(umi_terminal_tab_model_active_id(model), "one") == 0);
    assert(umi_terminal_tab_model_activate_relative(model, 1) == UMI_STATUS_OK);
    assert(strcmp(umi_terminal_tab_model_active_id(model), "two") == 0);
    assert(umi_terminal_tab_model_set_pinned(model, "two", 1) == UMI_STATUS_OK);
    assert(umi_terminal_tab_model_move(model, "two", 0U) == UMI_STATUS_OK);
    assert(umi_terminal_tab_model_at(model, 0U, &tab) == UMI_STATUS_OK);
    assert(strcmp(tab.session_id, "two") == 0 && tab.pinned);
    assert(umi_terminal_tab_model_remove(model, "two") == UMI_STATUS_OK);
    assert(strcmp(umi_terminal_tab_model_active_id(model), "one") == 0);
    umi_terminal_tab_model_destroy(model);
    return 0;
}
