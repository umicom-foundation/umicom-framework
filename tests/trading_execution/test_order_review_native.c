/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_execution/test_order_review_native.c
 * PURPOSE: Exercise actual GTK order review signals and retained-widget lifetimes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "order_review_fixture.h"
#include "umicom/trading_ui/gtk4/trading_panels.h"
#define CHECK REVIEW_CHECK
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *match = Find(child, id); if (match != NULL) return match;
    }
    return NULL;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    if (!gtk_init_check()) return 77;
    ReviewFixture f; ReviewFixtureInit(&f);
    UmiGtk4TradingPanelContext context = {f.workspace, &f.controller, 0};
    UmiUiWorkspaceWindow window = {0}; strcpy(window.tool_id, "blotter");
    GtkWidget *root = umi_gtk4_trading_panel_create(&window, &context);
    CHECK(root != NULL); g_object_ref_sink(root);
    GtkWidget *cancel = Find(root, "trading.orders.cancel");
    CHECK(cancel != NULL);
    UmiTradingWorkspaceSnapshot snapshot;
    if (strcmp(argv[1], "query") == 0) {
        GtkWidget *search = Find(root, "trading.orders.search");
        GtkWidget *apply = Find(root, "trading.orders.apply");
        CHECK(GTK_IS_EDITABLE(search) && apply != NULL);
        gtk_editable_set_text(GTK_EDITABLE(search), "nq"); g_signal_emit_by_name(apply, "clicked");
        CHECK(umi_trading_workspace_snapshot(f.workspace, &snapshot) == UMI_STATUS_OK && snapshot.visible_order_count == 1);
        CHECK(strcmp(snapshot.selected_order_id, f.first) == 0);
    } else if (strcmp(argv[1], "refresh") == 0) {
        g_object_ref(cancel);
        UmiTradingOrderQuery query = {UMI_TRADING_WORKSPACE_ORDERS_ALL, "nq"};
        CHECK(UmiTradingWorkspaceSetOrderQuery(f.workspace, &query) == UMI_STATUS_OK);
        CHECK(UmiGtk4TradingPanelRefresh(root, 1) == UMI_STATUS_OK);
        GtkWidget *selection = Find(root, "trading.orders.selection");
        CHECK(GTK_IS_DROP_DOWN(selection) && g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(selection))) == 1);
        g_signal_emit_by_name(cancel, "clicked");
        UmiTradingOrderReview *review = calloc(1, sizeof *review); CHECK(review != NULL);
        CHECK(UmiTradingWorkspaceReviewOrder(f.workspace, f.first, review) == UMI_STATUS_OK && review->order.status == UMI_ORDER_ACCEPTED);
        free(review); g_object_unref(cancel);
    } else if (strcmp(argv[1], "selection") == 0) {
        GtkWidget *selection = Find(root, "trading.orders.selection");
        CHECK(GTK_IS_DROP_DOWN(selection)); gtk_drop_down_set_selected(GTK_DROP_DOWN(selection), 1);
        g_signal_emit_by_name(cancel, "clicked");
        CHECK(umi_trading_ui_controller_snapshot(&f.controller).last_status == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(argv[1], "stale-fill") == 0) {
        UmiExecutionReport fill = ReviewFixtureFill(f.second);
        CHECK(umi_trading_workspace_record_execution(f.workspace, &fill) == UMI_STATUS_OK);
        g_signal_emit_by_name(cancel, "clicked");
        CHECK(umi_trading_ui_controller_snapshot(&f.controller).last_status == UMI_STATUS_INVALID_STATE);
    } else {
        CHECK(strcmp(argv[1], "retained-widget") == 0);
        g_object_ref(cancel); g_object_unref(root); root = NULL;
        g_signal_emit_by_name(cancel, "clicked");
        UmiTradingOrderReview *review = calloc(1, sizeof *review); CHECK(review != NULL);
        CHECK(UmiTradingWorkspaceReviewOrder(f.workspace, f.second, review) == UMI_STATUS_OK && review->order.status == UMI_ORDER_ACCEPTED);
        free(review); g_object_unref(cancel);
    }
    if (root != NULL) g_object_unref(root);
    umi_trading_workspace_destroy(f.workspace);
    return 0;
}
