/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/trading_ui_interaction/test_support.h
 *
 * PURPOSE:
 *   Share deterministic setup for guarded trading UI interaction tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_UI_INTERACTION_TEST_SUPPORT_H
#define UMICOM_TRADING_UI_INTERACTION_TEST_SUPPORT_H

#include "umicom/trading_ui/trading_ui.h"

/**
 * Represent the trading ui test fixture data shared with callers of this public contract.
 */
typedef struct UmiTradingUiTestFixture {
    UmiTradingWorkspace *workspace;
    UmiTradingUiController controller;
    UmiTradingSimulationMarket simulation;
    unsigned int changed_count;
    uint64_t last_revision;
} UmiTradingUiTestFixture;

/**
 * Initialise trading ui test fixture from caller-provided values so later operations
 * receive a known state.
 */
void umi_trading_ui_test_fixture_init(UmiTradingUiTestFixture *fixture,
                                      int seed_market);
/**
 * Release or reset state held by trading ui test fixture so the same storage can be reused
 * safely.
 */
void umi_trading_ui_test_fixture_destroy(UmiTradingUiTestFixture *fixture);

/**
 * Exercise test actions and return a clear result when the behaviour no longer matches its
 * contract.
 */
void test_actions(void);
/**
 * Exercise test controller refresh and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_controller_refresh(void);
/**
 * Exercise test controller environment and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_controller_environment(void);
/**
 * Exercise test controller filter and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_controller_filter(void);
/**
 * Exercise test controller selection and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_controller_selection(void);
/**
 * Exercise test controller draft and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_controller_draft(void);
/**
 * Exercise test controller preview and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_controller_preview(void);
/**
 * Exercise test controller kill switch and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_controller_kill_switch(void);
/**
 * Exercise test simulation seed and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_simulation_seed(void);
/**
 * Exercise test simulation step and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_simulation_step(void);
/**
 * Perform test controller through the module contract so client applications do not
 * duplicate its policy.
 */
void test_controller_dispatch(void);
/**
 * Exercise test controller order filter and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_controller_order_filter(void);
/**
 * Exercise test controller submit gate and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_controller_submit_gate(void);
/**
 * Exercise test controller live opt in and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_controller_live_opt_in(void);
/**
 * Exercise test controller unknown action and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_controller_unknown_action(void);
/**
 * Exercise test simulation idempotence and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_simulation_idempotence(void);
/**
 * Exercise test simulation environment gate and return a clear result when the behaviour
 * no longer matches its contract.
 */
void test_simulation_environment_gate(void);
/**
 * Exercise test simulation filter preservation and return a clear result when the
 * behaviour no longer matches its contract.
 */
void test_simulation_filter_preservation(void);

#endif
