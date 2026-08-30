/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/trading_ui_interaction/test_main.c
 *
 * PURPOSE:
 *   Execute focused guarded trading workstation interaction tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

#include <stdio.h>

int main(void)
{
    test_actions();
    test_controller_refresh();
    test_controller_environment();
    test_controller_filter();
    test_controller_selection();
    test_controller_draft();
    test_controller_preview();
    test_controller_kill_switch();
    test_simulation_seed();
    test_simulation_step();
    test_controller_dispatch();
    test_controller_order_filter();
    test_controller_submit_gate();
    test_controller_live_opt_in();
    test_controller_unknown_action();
    test_simulation_idempotence();
    test_simulation_environment_gate();
    test_simulation_filter_preservation();
    (void)printf("Trading UI interaction tests passed.\n");
    return 0;
}
