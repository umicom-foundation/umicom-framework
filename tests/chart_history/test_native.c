/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_history/test_native.c
 * PURPOSE: Drive real history buttons, canvas shortcuts, delayed actions and retained controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../trading_execution/order_review_fixture.h"
#include "umicom/trading_ui/gtk4/interactive_chart.h"
#include "umicom/trading/chart_history.h"
#define CHECK REVIEW_CHECK
#define OK(x) CHECK((x)==UMI_STATUS_OK)
static GtkWidget *Find(GtkWidget *root,const char *id)
{
    const char *tag=g_object_get_data(G_OBJECT(root),"umicom-automation-id");
    if (tag!=NULL && strcmp(tag,id)==0) return root;
    for (GtkWidget *child=gtk_widget_get_first_child(root);child!=NULL;child=gtk_widget_get_next_sibling(child)) {
        GtkWidget *found=Find(child,id); if (found!=NULL) return found;
    }
    return NULL;
}
static gboolean Key(GtkWidget *area,guint key,GdkModifierType mods)
{
    GListModel *controllers=gtk_widget_observe_controllers(area); gboolean handled=FALSE;
    for (guint i=0;i<g_list_model_get_n_items(controllers);++i) {
        GObject *item=g_list_model_get_item(controllers,i);
        if (GTK_IS_EVENT_CONTROLLER_KEY(item)) g_signal_emit_by_name(item,"key-pressed",key,0,mods,&handled);
        g_object_unref(item);
    }
    g_object_unref(controllers); return handled;
}
int main(int argc,char **argv)
{
    CHECK(argc==2); if (!gtk_init_check()) return 77;
    const char *name=argv[1]; ReviewFixture f; ReviewFixtureInit(&f);
    UmiTradingWorkspaceSnapshot before,after; OK(umi_trading_workspace_snapshot(f.workspace,&before));
    const char *pane=before.selected_instrument_id;
    OK(UmiTradingWorkspaceAddChartDrawing(f.workspace,pane,"range",(UmiChartPoint){100,10},(UmiChartPoint){200,20}));
    UmiChartDrawingRegistry *registry=umi_chart_workspace_drawings(umi_trading_workspace_charts(f.workspace));
    UmiChartDrawingSnapshot drawing; OK(umi_chart_drawing_registry_at(registry,0,&drawing));
    UmiGtk4TradingPanelContext context={f.workspace,&f.controller,0};
    GtkWidget *root=UmiGtk4TradingInteractiveChartCreate(&context); CHECK(root!=NULL); g_object_ref_sink(root);
    GtkWidget *undo=Find(root,"trading.chart.undo-drawing"), *redo=Find(root,"trading.chart.redo-drawing");
    GtkWidget *area=Find(root,"trading.chart.canvas"), *label=Find(root,"trading.chart.drawing-history");
    CHECK(GTK_IS_BUTTON(undo) && GTK_IS_BUTTON(redo) && GTK_IS_LABEL(label));
    CHECK(gtk_widget_is_sensitive(undo) && !gtk_widget_is_sensitive(redo));
    if (strcmp(name,"buttons")==0 || strcmp(name,"key-y")==0 || strcmp(name,"key-shift-z")==0) {
        if (strcmp(name,"buttons")==0) g_signal_emit_by_name(undo,"clicked");
        else CHECK(Key(area,GDK_KEY_z,GDK_CONTROL_MASK));
        CHECK(umi_chart_drawing_registry_count(registry)==0 && gtk_widget_is_sensitive(redo));
        if (strcmp(name,"buttons")==0) g_signal_emit_by_name(redo,"clicked");
        else if (strcmp(name,"key-y")==0) CHECK(Key(area,GDK_KEY_y,GDK_CONTROL_MASK));
        else CHECK(Key(area,GDK_KEY_Z,GDK_CONTROL_MASK|GDK_SHIFT_MASK));
        CHECK(umi_chart_drawing_registry_count(registry)==1);
    } else if (strcmp(name,"key-modifiers")==0) {
        CHECK(!Key(area,GDK_KEY_z,0)); CHECK(!Key(area,GDK_KEY_z,GDK_CONTROL_MASK|GDK_ALT_MASK));
        CHECK(umi_chart_drawing_registry_count(registry)==1);
    } else if (strcmp(name,"stale")==0) {
        OK(UmiTradingWorkspaceSetChartDrawingLocked(f.workspace,pane,drawing.id,drawing.revision,1));
        g_signal_emit_by_name(undo,"clicked"); OK(umi_chart_drawing_registry_at(registry,0,&drawing)); CHECK(drawing.locked);
        g_signal_emit_by_name(undo,"clicked"); OK(umi_chart_drawing_registry_at(registry,0,&drawing)); CHECK(!drawing.locked);
    } else if (strcmp(name,"external")==0) {
        drawing.locked=1; OK(umi_chart_drawing_registry_upsert(registry,&drawing)); UmiGtk4TradingInteractiveChartRefresh(root);
        CHECK(!gtk_widget_is_sensitive(undo)); g_signal_emit_by_name(undo,"clicked");
        OK(umi_chart_drawing_registry_at(registry,0,&drawing)); CHECK(drawing.locked);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(label)),"out of date")!=NULL);
    } else if (strcmp(name,"instrument")==0) {
        UmiInstrument other=test_instrument(); OK(umi_trading_workspace_select_instrument(f.workspace,other.instrument_id.value));
        g_signal_emit_by_name(undo,"clicked"); CHECK(umi_chart_drawing_registry_count(registry)==1);
        CHECK(!gtk_widget_is_sensitive(undo)); CHECK(strstr(gtk_label_get_text(GTK_LABEL(label)),pane)!=NULL);
        OK(umi_trading_workspace_snapshot(f.workspace,&before));
    } else if (strcmp(name,"appearance-draft")==0) {
        GtkWidget *load=Find(root,"trading.chart.appearance-load"), *apply=Find(root,"trading.chart.appearance-apply"), *color=Find(root,"trading.chart.appearance-color");
        g_signal_emit_by_name(load,"clicked"); gtk_editable_set_text(GTK_EDITABLE(color),"#123456");
        g_signal_emit_by_name(undo,"clicked"); g_signal_emit_by_name(redo,"clicked"); g_signal_emit_by_name(apply,"clicked");
        OK(umi_chart_drawing_registry_at(registry,0,&drawing)); CHECK(drawing.style[0]=='\0');
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(color)),"#123456")==0);
    } else if (strcmp(name,"retained")==0) {
        g_object_ref(undo); g_object_ref(redo); UmiGtk4TradingInteractiveChartDetach(root);
        g_signal_emit_by_name(undo,"clicked"); g_signal_emit_by_name(redo,"clicked");
        g_object_unref(root); root=NULL;
        g_signal_emit_by_name(undo,"clicked"); g_signal_emit_by_name(redo,"clicked");
        CHECK(umi_chart_drawing_registry_count(registry)==1); g_object_unref(undo); g_object_unref(redo);
    } else return 2;
    OK(umi_trading_workspace_snapshot(f.workspace,&after));
    CHECK(after.order_count==before.order_count && after.environment==before.environment && after.live_armed==before.live_armed);
    CHECK(memcmp(&after.draft_order,&before.draft_order,sizeof before.draft_order)==0);
    if (root!=NULL) { UmiGtk4TradingInteractiveChartDetach(root); g_object_unref(root); }
    umi_trading_workspace_destroy(f.workspace); return 0;
}
