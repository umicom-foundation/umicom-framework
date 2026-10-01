/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_tools/test_native.c
 * PURPOSE: Exercise chart tools and protected drawing edits through real GTK controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../trading_execution/order_review_fixture.h"
#include "umicom/trading_ui/gtk4/interactive_chart.h"
#include "umicom/chart/drawing_tools.h"
#define CHECK REVIEW_CHECK
#define OK(x) CHECK((x)==UMI_STATUS_OK)
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, id); if (found != NULL) return found;
    }
    return NULL;
}
static void ClickPrice(GtkWidget *root, double fraction_x, double fraction_y)
{
    GtkWidget *area = Find(root, "trading.chart.canvas"); CHECK(area != NULL);
    gtk_widget_allocate(root, 1200, 1100, -1, NULL);
    int width = gtk_widget_get_width(area), height = gtk_widget_get_height(area);
    CHECK(width > 0 && height > 0);
    GListModel *controllers = gtk_widget_observe_controllers(area);
    GtkGestureClick *gesture = NULL;
    for (guint i = 0; i < g_list_model_get_n_items(controllers); ++i) {
        GObject *item = g_list_model_get_item(controllers, i);
        if (GTK_IS_GESTURE_CLICK(item)) { gesture = GTK_GESTURE_CLICK(item); break; }
        g_object_unref(item);
    }
    CHECK(gesture != NULL);
    g_signal_emit_by_name(gesture, "pressed", 1, width * fraction_x, height * fraction_y);
    g_object_unref(gesture); g_object_unref(controllers);
}

int main(int argc,char **argv)
{
    CHECK(argc==2);if(!gtk_init_check())return 77;const char *name=argv[1];
    ReviewFixture f;ReviewFixtureInit(&f);UmiTradingMarketSnapshot market;
    OK(umi_trading_workspace_selected_market(f.workspace,&market));
    for(int i=0;i<40;++i){UmiBar bar={0};bar.instrument=market.instrument;bar.start_time_ms=60000*(i+1);bar.end_time_ms=bar.start_time_ms+59999;
        bar.open=100;bar.close=101;bar.low=90;bar.high=110;bar.volume=100;OK(umi_trading_workspace_update_bar(f.workspace,&bar,100));}
    UmiGtk4TradingPanelContext context={f.workspace,&f.controller,0};
    GtkWidget *root=UmiGtk4TradingInteractiveChartCreate(&context);CHECK(root!=NULL);g_object_ref_sink(root);
    UmiChartDrawingRegistry *registry=umi_chart_workspace_drawings(umi_trading_workspace_charts(f.workspace));
    UmiTradingWorkspaceSnapshot before,after;OK(umi_trading_workspace_snapshot(f.workspace,&before));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Find(root,"trading.chart.liquidity-zone")),TRUE);
    ClickPrice(root,0.2,0.25);ClickPrice(root,0.6,0.55);CHECK(umi_chart_drawing_registry_count(registry)==1);
    UmiChartDrawingSnapshot drawing,changed;OK(umi_chart_drawing_registry_at(registry,0,&drawing));CHECK(strcmp(drawing.tool,"liquidity-zone")==0);
    if(strcmp(name,"new-tools")==0){
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Find(root,"trading.chart.range")),TRUE);ClickPrice(root,0.7,0.55);ClickPrice(root,0.3,0.25);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Find(root,"trading.chart.ray")),TRUE);ClickPrice(root,0.7,0.55);ClickPrice(root,0.3,0.25);
        CHECK(umi_chart_drawing_registry_count(registry)==3);OK(umi_chart_drawing_registry_at(registry,2,&changed));CHECK(strcmp(changed.tool,"ray")==0&&changed.time1>changed.time2);
        CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(Find(root,"trading.chart.drawings"))))==3);
    }else if(strcmp(name,"lock")==0){
        g_signal_emit_by_name(Find(root,"trading.chart.lock-drawing"),"clicked");OK(umi_chart_drawing_registry_find(registry,drawing.id,&changed));CHECK(changed.locked);
        g_signal_emit_by_name(Find(root,"trading.chart.remove-drawing"),"clicked");CHECK(umi_chart_drawing_registry_count(registry)==1);
        g_signal_emit_by_name(Find(root,"trading.chart.lock-drawing"),"clicked");OK(umi_chart_drawing_registry_find(registry,drawing.id,&changed));CHECK(!changed.locked);
    }else if(strcmp(name,"move")==0){
        g_signal_emit_by_name(Find(root,"trading.chart.move-drawing"),"clicked");ClickPrice(root,0.3,0.35);ClickPrice(root,0.7,0.65);
        CHECK(umi_chart_drawing_registry_count(registry)==1);OK(umi_chart_drawing_registry_find(registry,drawing.id,&changed));
        CHECK(changed.time1!=drawing.time1&&changed.value1!=drawing.value1&&changed.revision>drawing.revision);
    }else if(strcmp(name,"duplicate")==0){
        g_signal_emit_by_name(Find(root,"trading.chart.lock-drawing"),"clicked");
        g_signal_emit_by_name(Find(root,"trading.chart.duplicate-drawing"),"clicked");CHECK(umi_chart_drawing_registry_count(registry)==2);
        OK(umi_chart_drawing_registry_at(registry,0,&drawing));OK(umi_chart_drawing_registry_at(registry,1,&changed));CHECK(drawing.locked&&!changed.locked&&strcmp(drawing.id,changed.id)!=0);
        CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(Find(root,"trading.chart.drawings")))==1);
    }else if(strcmp(name,"stale-move")==0){
        g_signal_emit_by_name(Find(root,"trading.chart.move-drawing"),"clicked");ClickPrice(root,0.3,0.35);
        drawing.locked=1;OK(umi_chart_drawing_registry_upsert(registry,&drawing));
        ClickPrice(root,0.7,0.65);CHECK(umi_chart_drawing_registry_count(registry)==1);
        OK(umi_chart_drawing_registry_find(registry,drawing.id,&changed));CHECK(changed.time1==drawing.time1&&changed.value1==drawing.value1&&changed.locked);
    }else if(strcmp(name,"selection-change")==0){
        g_signal_emit_by_name(Find(root,"trading.chart.move-drawing"),"clicked");ClickPrice(root,0.3,0.35);
        gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root,"trading.chart.drawings")),GTK_INVALID_LIST_POSITION);
        /* Cancelling a move must not turn its second click into a new drawing. */
        ClickPrice(root,0.7,0.65);ClickPrice(root,0.4,0.45);
        CHECK(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Find(root,"trading.chart.cursor"))));
        CHECK(umi_chart_drawing_registry_count(registry)==1);
        OK(umi_chart_drawing_registry_find(registry,drawing.id,&changed));CHECK(changed.time1==drawing.time1&&changed.value1==drawing.value1);
    }else if(strcmp(name,"escape")==0){
        g_signal_emit_by_name(Find(root,"trading.chart.move-drawing"),"clicked");ClickPrice(root,0.3,0.35);
        GListModel *controllers=gtk_widget_observe_controllers(Find(root,"trading.chart.canvas"));
        int handled=0;
        for(guint i=0;i<g_list_model_get_n_items(controllers);++i){
            GObject *item=g_list_model_get_item(controllers,i);
            if(GTK_IS_EVENT_CONTROLLER_KEY(item)){
                gboolean result=FALSE;g_signal_emit_by_name(item,"key-pressed",GDK_KEY_Escape,0,0,&result);handled=result;
            }
            g_object_unref(item);
        }
        g_object_unref(controllers);CHECK(handled);
        ClickPrice(root,0.7,0.65);ClickPrice(root,0.4,0.45);
        CHECK(umi_chart_drawing_registry_count(registry)==1);
        OK(umi_chart_drawing_registry_find(registry,drawing.id,&changed));CHECK(changed.revision==drawing.revision);
        CHECK(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Find(root,"trading.chart.cursor"))));
    }else if(strcmp(name,"instrument-change")==0){
        g_signal_emit_by_name(Find(root,"trading.chart.move-drawing"),"clicked");ClickPrice(root,0.3,0.35);
        UmiInstrument other=test_instrument();OK(umi_trading_workspace_select_instrument(f.workspace,other.instrument_id.value));
        /* Instrument selection intentionally prepares its own ticket. Capture
         * that state before checking that chart refresh and clicks preserve it. */
        OK(umi_trading_workspace_snapshot(f.workspace,&before));
        UmiGtk4TradingInteractiveChartRefresh(root);
        CHECK(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Find(root,"trading.chart.cursor"))));
        ClickPrice(root,0.7,0.65);ClickPrice(root,0.4,0.45);CHECK(umi_chart_drawing_registry_count(registry)==1);
        OK(umi_chart_drawing_registry_find(registry,drawing.id,&changed));CHECK(changed.revision==drawing.revision);
    }else if(strcmp(name,"retained")==0){
        GtkWidget *lock=Find(root,"trading.chart.lock-drawing"),*copy=Find(root,"trading.chart.duplicate-drawing"),*move=Find(root,"trading.chart.move-drawing");
        g_object_ref(lock);g_object_ref(copy);g_object_ref(move);
        UmiGtk4TradingInteractiveChartDetach(root);
        g_signal_emit_by_name(lock,"clicked");g_signal_emit_by_name(copy,"clicked");g_signal_emit_by_name(move,"clicked");
        CHECK(umi_chart_drawing_registry_count(registry)==1);g_object_unref(root);root=NULL;
        g_signal_emit_by_name(lock,"clicked");g_signal_emit_by_name(copy,"clicked");g_signal_emit_by_name(move,"clicked");
        g_object_unref(lock);g_object_unref(copy);g_object_unref(move);
    }else return 2;
    OK(umi_trading_workspace_snapshot(f.workspace,&after));CHECK(after.order_count==before.order_count&&after.live_armed==before.live_armed&&after.environment==before.environment);
    CHECK(memcmp(&after.draft_order,&before.draft_order,sizeof(before.draft_order))==0);
    if(root!=NULL)g_object_unref(root);
    umi_trading_workspace_destroy(f.workspace);return 0;
}
