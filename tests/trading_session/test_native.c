/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_session/test_native.c
 * PURPOSE: Exercise explicit capture, filter drafts, body refresh and retained native controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
#include "umicom/trading_ui/gtk4/trading_panels.h"
#include "umicom/trading_ui/gtk4/session_report.h"
static GtkWidget *Find(GtkWidget *root,const char *id)
{
    const char *tag=g_object_get_data(G_OBJECT(root),"umicom-automation-id");if(tag&&strcmp(tag,id)==0)return root;
    for(GtkWidget *child=gtk_widget_get_first_child(root);child;child=gtk_widget_get_next_sibling(child)){GtkWidget *found=Find(child,id);if(found)return found;}return NULL;
}
static char *Clipboard(GdkClipboard *clipboard)
{
    GValue value=G_VALUE_INIT;g_value_init(&value,G_TYPE_STRING);GdkContentProvider *provider=gdk_clipboard_get_content(clipboard);
    CHECK(provider&&gdk_content_provider_get_value(provider,&value,NULL));char *text=g_value_dup_string(&value);g_value_unset(&value);CHECK(text);return text;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);if(!gtk_init_check())return 77;ReviewFixture f;ReviewFixtureInit(&f);
    UmiGtk4TradingPanelContext context={f.workspace,&f.controller,0};UmiUiWorkspaceWindow spec={0};strcpy(spec.tool_id,"executions");strcpy(spec.window_id,"session-native");
    GtkWidget *panel=umi_gtk4_trading_panel_create(&spec,&context);CHECK(panel);g_object_ref_sink(panel);
    GtkWidget *session=Find(panel,"trading.session.report"),*filter=Find(panel,"trading.session.filter"),*refresh=Find(panel,"trading.session.refresh"),*copy=Find(panel,"trading.session.copy");
    CHECK(session&&filter&&refresh&&copy);g_object_ref(session);g_object_ref(refresh);g_object_ref(copy);
    GdkClipboard *clipboard=g_object_ref(gtk_widget_get_clipboard(copy));
    if(strcmp(argv[1],"filter")==0){
        gtk_editable_set_text(GTK_EDITABLE(filter),"nq");g_signal_emit_by_name(copy,"clicked");char *text=Clipboard(clipboard);
        CHECK(strstr(text,f.first)&&strstr(text,f.second));g_free(text);
        g_signal_emit_by_name(refresh,"clicked");g_signal_emit_by_name(copy,"clicked");text=Clipboard(clipboard);CHECK(strstr(text,f.first)&&!strstr(text,f.second));g_free(text);
    }else if(strcmp(argv[1],"stale")==0){
        Fill(&f,f.first,"new-fill",1,100,1200);UmiGtk4TradingSessionReportMarkStale(session);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(panel,"trading.session.message"))),"changed after capture"));
        g_signal_emit_by_name(copy,"clicked");char *text=Clipboard(clipboard);CHECK(!strstr(text,"new-fill"));g_free(text);
        g_signal_emit_by_name(refresh,"clicked");g_signal_emit_by_name(copy,"clicked");text=Clipboard(clipboard);CHECK(strstr(text,"new-fill"));g_free(text);
    }else if(strcmp(argv[1],"body-refresh")==0){
        gtk_editable_set_text(GTK_EDITABLE(filter),"typed-filter");OK(UmiGtk4TradingPanelRefresh(panel,1));
        CHECK(Find(panel,"trading.session.report")==session&&Find(panel,"trading.session.filter")==filter);
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(filter)),"typed-filter")==0);
    }else if(strcmp(argv[1],"failed-refresh")==0){
        /* GTK limits characters, while the portable API bounds UTF-8 bytes. */
        char oversized[97];for(size_t i=0;i<48;++i){oversized[2*i]=(char)0xc3;oversized[2*i+1]=(char)0xa9;}oversized[96]='\0';
        gtk_editable_set_text(GTK_EDITABLE(filter),oversized);g_signal_emit_by_name(refresh,"clicked");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(panel,"trading.session.message"))),"not replaced"));
        g_signal_emit_by_name(copy,"clicked");char *text=Clipboard(clipboard);CHECK(strstr(text,f.first)&&strstr(text,f.second));g_free(text);
    }else CHECK(strcmp(argv[1],"detach")==0||strcmp(argv[1],"mount-destroy")==0);
    if(strcmp(argv[1],"mount-destroy")==0){g_object_unref(panel);panel=NULL;}
    else UmiGtk4TradingPanelDetachChart(panel);
    umi_trading_workspace_destroy(f.workspace);context.workspace=NULL;context.controller=NULL;
    CHECK(!gtk_widget_get_sensitive(copy)&&!gtk_widget_get_sensitive(refresh));
    gdk_clipboard_set_text(clipboard,"closed-sentinel");g_signal_emit_by_name(refresh,"clicked");g_signal_emit_by_name(copy,"clicked");
    char *text=Clipboard(clipboard);CHECK(strcmp(text,"closed-sentinel")==0);g_free(text);
    if(panel)g_object_unref(panel);
    g_object_unref(copy);g_object_unref(refresh);g_object_unref(session);g_object_unref(clipboard);return 0;
}
