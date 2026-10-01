/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_timeframe/test_native.c
 * PURPOSE: Exercise real native timeframe selection, saved review and detached-control lifetime.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
#include "umicom/trading_ui/gtk4/interactive_chart.h"
static GtkWidget *Find(GtkWidget *root,const char *id)
{
    const char *tag=g_object_get_data(G_OBJECT(root),"umicom-automation-id");
    if(tag!=NULL && strcmp(tag,id)==0)return root;
    for(GtkWidget *child=gtk_widget_get_first_child(root);child!=NULL;child=gtk_widget_get_next_sibling(child)){
        GtkWidget *found=Find(child,id);if(found!=NULL)return found;
    }return NULL;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);if(!gtk_init_check())return 77;const char *name=argv[1];
    ReviewFixture f;ReviewFixtureInit(&f);TimeframeBars(&f,40U);
    UmiTradingWorkspaceSnapshot before;OK(umi_trading_workspace_snapshot(f.workspace,&before));const char *id=before.selected_instrument_id;
    UmiGtk4TradingPanelContext context={f.workspace,&f.controller,0};
    GtkWidget *root=UmiGtk4TradingInteractiveChartCreate(&context);CHECK(root!=NULL);g_object_ref_sink(root);
    GtkWidget *window=gtk_window_new();g_object_ref_sink(window);gtk_window_set_child(GTK_WINDOW(window),root);
    GtkWidget *selector=Find(root,"trading.chart.timeframe");CHECK(GTK_IS_DROP_DOWN(selector));
    CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(selector)))==7U);
    UmiDataServer *server=NULL;OK(umi_data_server_create_memory(&server));UmiTradingChartPersistence *service=NULL;
    OK(UmiTradingChartPersistenceCreate(f.workspace,&service));OK(UmiTradingChartPersistenceBind(service,server,"native.timeframe"));
    UmiGtk4TradingInteractiveChartBindPersistence(root,service);
    if(strcmp(name,"stale")==0){
        UmiInstrument other=test_instrument();OK(umi_trading_workspace_select_instrument(f.workspace,other.instrument_id.value));
        gtk_drop_down_set_selected(GTK_DROP_DOWN(selector),2U);
        UmiChartNavigation otherNavigation;OK(UmiTradingWorkspaceGetChartNavigation(f.workspace,other.instrument_id.value,&otherNavigation));CHECK(otherNavigation.interval_ms==0U);
        CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(selector))==0U);
        OK(umi_trading_workspace_select_instrument(f.workspace,id));
        UmiGtk4TradingInteractiveChartRefresh(root);OK(umi_trading_workspace_snapshot(f.workspace,&before));
    }else{
        gtk_drop_down_set_selected(GTK_DROP_DOWN(selector),2U);
        UmiChartNavigation navigation;OK(UmiTradingWorkspaceGetChartNavigation(f.workspace,id,&navigation));CHECK(navigation.interval_ms==300000U);
        if(strcmp(name,"zoom-fit")==0){
            g_signal_emit_by_name(Find(root,"trading.chart.zoom-in"),"clicked");
            OK(UmiTradingWorkspaceGetChartNavigation(f.workspace,id,&navigation));CHECK(navigation.visible_bars==6U);
            g_signal_emit_by_name(Find(root,"trading.chart.fit"),"clicked");
            OK(UmiTradingWorkspaceGetChartNavigation(f.workspace,id,&navigation));CHECK(navigation.visible_bars==0U && !navigation.pinned && navigation.interval_ms==300000U);
        }else if(strcmp(name,"restore")==0 || strcmp(name,"stale-preview")==0){
            g_signal_emit_by_name(Find(root,"trading.chart.save"),"clicked");
            gtk_drop_down_set_selected(GTK_DROP_DOWN(selector),0U);
            g_signal_emit_by_name(Find(root,"trading.chart.preview"),"clicked");
            GtkWidget *restore=Find(root,"trading.chart.restore");CHECK(gtk_widget_get_sensitive(restore));
            GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(root,"trading.chart.saved-details")));
            GtkTextIter start,end;gtk_text_buffer_get_bounds(buffer,&start,&end);char *text=gtk_text_buffer_get_text(buffer,&start,&end,FALSE);
            CHECK(strstr(text,"Saved timeframe: 5 minutes; current timeframe: Source")!=NULL);g_free(text);
            if(strcmp(name,"stale-preview")==0){gtk_drop_down_set_selected(GTK_DROP_DOWN(selector),1U);CHECK(!gtk_widget_get_sensitive(restore));}
            g_signal_emit_by_name(restore,"clicked");
            OK(UmiTradingWorkspaceGetChartNavigation(f.workspace,id,&navigation));
            CHECK(navigation.interval_ms==(strcmp(name,"restore")==0?300000U:60000U));
        }else CHECK(strcmp(name,"select")==0 || strcmp(name,"retained-control")==0 || strcmp(name,"detached-root")==0);
    }
    SameTrading(f.workspace,&before);
    if(strcmp(name,"retained-control")==0)g_object_ref(selector);
    UmiGtk4TradingInteractiveChartDetach(root);UmiTradingChartPersistenceDestroy(service);umi_data_server_destroy(server);
    umi_trading_workspace_destroy(f.workspace);f.workspace=NULL;
    if(strcmp(name,"detached-root")==0){gtk_drop_down_set_selected(GTK_DROP_DOWN(selector),4U);UmiGtk4TradingInteractiveChartRefresh(root);}
    gtk_window_destroy(GTK_WINDOW(window));g_object_unref(window);g_object_unref(root);
    if(strcmp(name,"retained-control")==0){gtk_drop_down_set_selected(GTK_DROP_DOWN(selector),4U);g_object_unref(selector);}
    return 0;
}
