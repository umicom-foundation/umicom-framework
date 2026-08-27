/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/trading_ui_interaction/test_support.h
 *
 * PURPOSE:
 *   Share deterministic setup for guarded trading UI interaction tests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_UI_INTERACTION_TEST_SUPPORT_H
#define UMICOM_TRADING_UI_INTERACTION_TEST_SUPPORT_H

#include "umicom/trading_ui/trading_ui.h"

typedef struct UmiTradingUiTestFixture {
    UmiTradingWorkspace *workspace;
    UmiTradingUiController controller;
    UmiTradingSimulationMarket simulation;
    unsigned int changed_count;
    uint64_t last_revision;
} UmiTradingUiTestFixture;

void umi_trading_ui_test_fixture_init(UmiTradingUiTestFixture *fixture,
                                      int seed_market);
void umi_trading_ui_test_fixture_destroy(UmiTradingUiTestFixture *fixture);

void test_actions(void);
void test_controller_refresh(void);
void test_controller_environment(void);
void test_controller_filter(void);
void test_controller_selection(void);
void test_controller_draft(void);
void test_controller_preview(void);
void test_controller_kill_switch(void);
void test_simulation_seed(void);
void test_simulation_step(void);
void test_controller_dispatch(void);
void test_controller_order_filter(void);
void test_controller_submit_gate(void);
void test_controller_live_opt_in(void);
void test_controller_unknown_action(void);
void test_simulation_idempotence(void);
void test_simulation_environment_gate(void);
void test_simulation_filter_preservation(void);

#endif
