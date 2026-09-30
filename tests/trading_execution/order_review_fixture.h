/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_execution/order_review_fixture.h
 * PURPOSE: Build deterministic simulation orders for domain and native review tests.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_ORDER_REVIEW_TEST_FIXTURE_H
#define UMICOM_ORDER_REVIEW_TEST_FIXTURE_H
#include "../trading/test_trading_common.h"
#include "umicom/trading_ui/action_controller.h"
#include <stdlib.h>
#define REVIEW_CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
typedef struct ReviewFixture {
    UmiTradingWorkspace *workspace;
    UmiTradingUiController controller;
    char first[UMI_FINANCE_ID_CAPACITY];
    char second[UMI_FINANCE_ID_CAPACITY];
} ReviewFixture;
static inline void ReviewFixtureInit(ReviewFixture *f)
{
    memset(f, 0, sizeof(*f));
    REVIEW_CHECK(umi_trading_workspace_create(NULL, &f->workspace) == UMI_STATUS_OK);
    UmiInstrument instrument = test_instrument();
    instrument.multiplier = 1.0;
    REVIEW_CHECK(umi_trading_workspace_add_instrument(f->workspace, &instrument) == UMI_STATUS_OK);
    REVIEW_CHECK(umi_trading_workspace_set_health(f->workspace, 1, 0, 1) == UMI_STATUS_OK);
    REVIEW_CHECK(umi_trading_workspace_set_draft_quantity(f->workspace, 2.0) == UMI_STATUS_OK);
    REVIEW_CHECK(umi_trading_workspace_set_draft_prices(f->workspace, 100.0, 0.0) == UMI_STATUS_OK);
    UmiRiskDecision decision;
    UmiTradingWorkspaceSnapshot snapshot;
    REVIEW_CHECK(umi_trading_workspace_submit_order(f->workspace, 1000, &decision) == UMI_STATUS_OK && decision.allowed);
    REVIEW_CHECK(umi_trading_workspace_snapshot(f->workspace, &snapshot) == UMI_STATUS_OK);
    strcpy(f->first, snapshot.selected_order_id);
    strcpy(instrument.instrument_id.value, "ICE.ES.202609");
    strcpy(instrument.symbol, "ES");
    strcpy(instrument.venue, "ICE");
    REVIEW_CHECK(umi_trading_workspace_add_instrument(f->workspace, &instrument) == UMI_STATUS_OK);
    REVIEW_CHECK(umi_trading_workspace_select_instrument(f->workspace, instrument.instrument_id.value) == UMI_STATUS_OK);
    REVIEW_CHECK(umi_trading_workspace_set_draft_quantity(f->workspace, 2.0) == UMI_STATUS_OK);
    REVIEW_CHECK(umi_trading_workspace_set_draft_prices(f->workspace, 100.0, 0.0) == UMI_STATUS_OK);
    REVIEW_CHECK(umi_trading_workspace_submit_order(f->workspace, 1001, &decision) == UMI_STATUS_OK && decision.allowed);
    REVIEW_CHECK(umi_trading_workspace_snapshot(f->workspace, &snapshot) == UMI_STATUS_OK);
    strcpy(f->second, snapshot.selected_order_id);
    REVIEW_CHECK(umi_trading_ui_controller_init(&f->controller, f->workspace, NULL) == UMI_STATUS_OK);
}
static inline UmiExecutionReport ReviewFixtureFill(const char *order_id)
{
    UmiExecutionReport fill = {0};
    strcpy(fill.execution_id.value, "review-fill");
    strcpy(fill.client_order_id.value, order_id);
    fill.fill_quantity = 1.0;
    fill.fill_price = 100.0;
    fill.event_time_ms = 1100;
    return fill;
}
#endif
