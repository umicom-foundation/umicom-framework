/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_timeframe/fixture.h
 * PURPOSE: Provide observed bars and verify chart-only changes preserve trading evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_CHART_TIMEFRAME_TEST_FIXTURE_H
#define UMICOM_CHART_TIMEFRAME_TEST_FIXTURE_H
#include "../trading_execution/order_review_fixture.h"
#include "umicom/trading/chart_timeframe.h"
#include "umicom/trading/chart_persistence.h"
#include "umicom/trading_ui/chart_scene.h"
#define CHECK REVIEW_CHECK
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static inline void TimeframeBars(ReviewFixture *f, size_t count)
{
    UmiTradingMarketSnapshot market; OK(umi_trading_workspace_selected_market(f->workspace, &market));
    for (size_t i = 0; i < count; ++i) {
        UmiBar bar = {0}; bar.instrument = market.instrument;
        bar.start_time_ms = (int64_t)i * 60000; bar.end_time_ms = bar.start_time_ms + 59999;
        bar.open = 100 + (double)i; bar.high = 102 + (double)i; bar.low = 99 + (double)i;
        bar.close = 101 + (double)i; bar.volume = 10 + (double)i;
        OK(umi_trading_workspace_update_bar(f->workspace, &bar, 100));
    }
}
static inline void SameTrading(UmiTradingWorkspace *workspace, const UmiTradingWorkspaceSnapshot *before)
{
    UmiTradingWorkspaceSnapshot after; OK(umi_trading_workspace_snapshot(workspace, &after));
    CHECK(after.order_count == before->order_count && after.environment == before->environment && after.live_armed == before->live_armed);
    CHECK(strcmp(after.selected_instrument_id, before->selected_instrument_id) == 0);
    CHECK(strcmp(after.selected_order_id, before->selected_order_id) == 0);
    CHECK(memcmp(&after.draft_order, &before->draft_order, sizeof before->draft_order) == 0);
}
#endif
