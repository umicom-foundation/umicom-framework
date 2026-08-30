/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/trading_ui_interaction/test_actions.c
 *
 * PURPOSE:
 *   Verify stable action identifiers and payload classification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

#include <assert.h>
#include <string.h>

void test_actions(void)
{
    assert(umi_trading_ui_action_kind(UMI_TRADING_UI_ACTION_REFRESH) ==
           UMI_TRADING_UI_ACTION_KIND_REFRESH);
    assert(umi_trading_ui_action_kind(UMI_TRADING_UI_ACTION_SUBMIT_ORDER) ==
           UMI_TRADING_UI_ACTION_KIND_SUBMIT_ORDER);
    assert(umi_trading_ui_action_kind("unknown") ==
           UMI_TRADING_UI_ACTION_KIND_UNKNOWN);
    assert(strcmp(umi_trading_ui_action_id(
                      UMI_TRADING_UI_ACTION_KIND_SELECT_INSTRUMENT),
                  UMI_TRADING_UI_ACTION_SELECT_INSTRUMENT) == 0);
    assert(umi_trading_ui_action_requires_payload(
               UMI_TRADING_UI_ACTION_KIND_SET_QUANTITY));
    assert(!umi_trading_ui_action_requires_payload(
               UMI_TRADING_UI_ACTION_KIND_REFRESH));
}
