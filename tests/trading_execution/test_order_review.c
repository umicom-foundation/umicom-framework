/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_execution/test_order_review.c
 * PURPOSE: Verify atomic browsing and cancellation against the reviewed order version.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "order_review_fixture.h"
#define CHECK REVIEW_CHECK
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    ReviewFixture f;
    ReviewFixtureInit(&f);
    UmiTradingOrderQuery query = {UMI_TRADING_WORKSPACE_ORDERS_ALL, ""};
    UmiTradingWorkspaceSnapshot before, after;
    UmiTradingOrderReview *review = calloc(1U, sizeof(*review));
    CHECK(review != NULL);
    CHECK(umi_trading_workspace_snapshot(f.workspace, &before) == UMI_STATUS_OK);
    if (strcmp(argv[1], "query") == 0) {
        strcpy(query.text, "nq");
        CHECK(UmiTradingUiControllerSetOrderQuery(&f.controller, &query) == UMI_STATUS_OK);
        CHECK(umi_trading_workspace_snapshot(f.workspace, &after) == UMI_STATUS_OK);
        CHECK(after.visible_order_count == 1U && strcmp(after.selected_order_id, f.first) == 0);
        CHECK(strcmp(before.selected_instrument_id, after.selected_instrument_id) == 0);
        CHECK(memcmp(&before.draft_order, &after.draft_order, sizeof(after.draft_order)) == 0);
        uint64_t revision = after.revision;
        CHECK(UmiTradingWorkspaceSetOrderQuery(f.workspace, &query) == UMI_STATUS_OK);
        CHECK(umi_trading_workspace_snapshot(f.workspace, &after) == UMI_STATUS_OK && revision == after.revision);
        strcpy(query.text, "ice");
        CHECK(UmiTradingWorkspaceSetOrderQuery(f.workspace, &query) == UMI_STATUS_OK);
        CHECK(umi_trading_workspace_snapshot(f.workspace, &after) == UMI_STATUS_OK && strcmp(after.selected_order_id, f.second) == 0);
        strcpy(query.text, f.first);
        CHECK(UmiTradingWorkspaceSetOrderQuery(f.workspace, &query) == UMI_STATUS_OK);
        CHECK(umi_trading_workspace_snapshot(f.workspace, &after) == UMI_STATUS_OK && after.visible_order_count == 1U);
    } else if (strcmp(argv[1], "selection") == 0) {
        CHECK(UmiTradingWorkspaceReviewOrder(f.workspace, f.second, review) == UMI_STATUS_OK && review->can_cancel);
        CHECK(umi_trading_workspace_select_order(f.workspace, f.first) == UMI_STATUS_OK);
        CHECK(UmiTradingUiControllerCancelReviewedOrder(&f.controller, f.second, review->order.version) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiTradingWorkspaceReviewOrder(f.workspace, f.first, review) == UMI_STATUS_OK && review->can_cancel);
        CHECK(UmiTradingUiControllerCancelReviewedOrder(&f.controller, f.first, review->order.version) == UMI_STATUS_OK);
        CHECK(UmiTradingWorkspaceReviewOrder(f.workspace, f.first, review) == UMI_STATUS_OK && review->order.status == UMI_ORDER_CANCELLED);
        CHECK(UmiTradingWorkspaceReviewOrder(f.workspace, f.second, review) == UMI_STATUS_OK && review->order.status != UMI_ORDER_CANCELLED);
    } else if (strcmp(argv[1], "stale-fill") == 0) {
        CHECK(UmiTradingWorkspaceReviewOrder(f.workspace, f.second, review) == UMI_STATUS_OK);
        uint64_t version = review->order.version;
        UmiExecutionReport fill = ReviewFixtureFill(f.second);
        CHECK(umi_trading_workspace_record_execution(f.workspace, &fill) == UMI_STATUS_OK);
        CHECK(review->execution_count == 0U && review->order.filled_quantity == 0.0);
        CHECK(UmiTradingWorkspaceCancelReviewedOrder(f.workspace, f.second, version) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiTradingWorkspaceReviewOrder(f.workspace, f.second, review) == UMI_STATUS_OK && review->execution_count == 1U);
        CHECK(review->order.filled_quantity == 1.0 && strcmp(review->executions[0].execution_id.value, "review-fill") == 0);
        CHECK(UmiTradingWorkspaceCancelReviewedOrder(f.workspace, f.second, review->order.version) == UMI_STATUS_OK);
    } else if (strcmp(argv[1], "hidden") == 0) {
        CHECK(UmiTradingWorkspaceReviewOrder(f.workspace, f.second, review) == UMI_STATUS_OK);
        uint64_t version = review->order.version;
        strcpy(query.text, "no matching order");
        CHECK(UmiTradingWorkspaceSetOrderQuery(f.workspace, &query) == UMI_STATUS_OK);
        CHECK(umi_trading_workspace_snapshot(f.workspace, &after) == UMI_STATUS_OK && !after.has_selected_order && after.visible_order_count == 0U);
        CHECK(UmiTradingWorkspaceCancelReviewedOrder(f.workspace, f.second, version) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiTradingWorkspaceReviewOrder(f.workspace, f.second, review) == UMI_STATUS_OK && !review->can_cancel);
    } else if (strcmp(argv[1], "identity") == 0) {
        UmiExecutionReport fill = ReviewFixtureFill(f.first);
        CHECK(umi_trading_workspace_record_execution(f.workspace, &fill) == UMI_STATUS_OK);
        CHECK(UmiTradingWorkspaceReviewOrder(f.workspace, f.second, review) == UMI_STATUS_OK && review->execution_count == 0U);
        query.status = UMI_TRADING_WORKSPACE_ORDERS_FILLED;
        CHECK(UmiTradingWorkspaceSetOrderQuery(f.workspace, &query) == UMI_STATUS_OK);
        CHECK(umi_trading_workspace_snapshot(f.workspace, &after) == UMI_STATUS_OK && after.visible_order_count == 0U);
        query.status = UMI_TRADING_WORKSPACE_ORDERS_OPEN;
        CHECK(UmiTradingWorkspaceSetOrderQuery(f.workspace, &query) == UMI_STATUS_OK);
        CHECK(umi_trading_workspace_snapshot(f.workspace, &after) == UMI_STATUS_OK && after.visible_order_count == 2U);
    } else {
        CHECK(strcmp(argv[1], "output") == 0);
        memset(query.text, 'x', sizeof(query.text));
        CHECK(UmiTradingWorkspaceSetOrderQuery(f.workspace, &query) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_trading_workspace_snapshot(f.workspace, &after) == UMI_STATUS_OK && before.revision == after.revision);
        memset(review, 0x5a, sizeof(*review));
        UmiTradingOrderReview *copy = malloc(sizeof(*copy));
        CHECK(copy != NULL);
        memcpy(copy, review, sizeof(*copy));
        CHECK(UmiTradingWorkspaceReviewOrder(f.workspace, "unknown", review) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(review, copy, sizeof(*copy)) == 0);
        query.text[0] = '\0';
        query.status = (UmiTradingWorkspaceOrderFilter)99;
        CHECK(UmiTradingWorkspaceSetOrderQuery(f.workspace, &query) == UMI_STATUS_INVALID_ARGUMENT);
        free(copy);
    }
    free(review);
    umi_trading_workspace_destroy(f.workspace);
    return 0;
}
