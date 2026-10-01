/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/trading_ui/session_report_gtk4.c
 * PURPOSE: Keep an explicit session snapshot stable while incoming market evidence refreshes other panels.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/trading_ui/gtk4/session_report.h"
#include "umicom/ui/gtk4/automation.h"
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

typedef struct SessionUi {
    UmiTradingWorkspace *workspace;
    UmiTradingSessionReport *report;
    GtkWidget *filter, *refresh, *copy, *message;
    GtkTextBuffer *buffer;
    bool detached, error;
} SessionUi;
static SessionUi *State(gpointer root)
{ return g_object_get_data(G_OBJECT(root), "umicom-trading-session-review"); }
static void Free(gpointer data)
{ SessionUi *state=data;UmiTradingSessionReportDestroy(state->report);g_free(state); }
static void Failure(SessionUi *state,UmiStatus status)
{
    char text[256];g_snprintf(text,sizeof(text),"Report not replaced: %s. The previous captured report remains available.",umi_status_text(status));
    gtk_label_set_text(GTK_LABEL(state->message),text);state->error=true;
}
static void Refresh(GtkButton *button,gpointer root)
{
    (void)button;SessionUi *state=State(root);
    if(state==NULL||state->detached||state->workspace==NULL)return;
    UmiTradingSessionReport *report=NULL;
    UmiStatus status=UmiTradingSessionReportCapture(state->workspace,gtk_editable_get_text(GTK_EDITABLE(state->filter)),&report);
    size_t required=0;char *text=NULL;
    if(status==UMI_STATUS_OK)status=UmiTradingSessionReportDescribe(report,NULL,0,&required);
    if(status==UMI_STATUS_OK){text=g_try_malloc(required);if(text==NULL)status=UMI_STATUS_OUT_OF_MEMORY;}
    if(status==UMI_STATUS_OK)status=UmiTradingSessionReportDescribe(report,text,required,NULL);
    if(status==UMI_STATUS_OK && !g_utf8_validate(text,-1,NULL))status=UMI_STATUS_INVALID_ARGUMENT;
    if(status!=UMI_STATUS_OK){g_free(text);UmiTradingSessionReportDestroy(report);Failure(state,status);return;}
    UmiTradingSessionReportDestroy(state->report);state->report=report;state->error=false;
    gtk_text_buffer_set_text(state->buffer,text,-1);g_free(text);
    gtk_widget_set_sensitive(state->copy,TRUE);
    gtk_label_set_text(GTK_LABEL(state->message),"Captured local records. Copy CSV exports this report; editing the filter does not change it until Refresh report.");
}
static void Copy(GtkButton *button,gpointer root)
{
    SessionUi *state=State(root);
    if(state==NULL||state->detached||state->report==NULL)return;
    UmiCsvDocument *csv=NULL;UmiStatus status=UmiTradingSessionReportExportCsv(state->report,&csv);
    if(status!=UMI_STATUS_OK){Failure(state,status);return;}
    gdk_clipboard_set_text(gtk_widget_get_clipboard(GTK_WIDGET(button)),UmiCsvDocumentData(csv));
    UmiCsvDocumentDestroy(csv);state->error=false;
    gtk_label_set_text(GTK_LABEL(state->message),"Copied the displayed captured report, including its filter, revision and whole-book consistency issues.");
}
static void Edited(GtkEditable *editable,gpointer root)
{
    (void)editable;SessionUi *state=State(root);if(state==NULL||state->detached)return;
    state->error=false;
    gtk_label_set_text(GTK_LABEL(state->message),"Filter edited. Refresh report applies it; Copy captured CSV still uses the displayed report.");
}
GtkWidget *UmiGtk4TradingSessionReportCreate(UmiTradingWorkspace *workspace)
{
    if(workspace==NULL)return NULL;
    GtkWidget *root=gtk_box_new(GTK_ORIENTATION_VERTICAL,6);
    SessionUi *state=g_new0(SessionUi,1);state->workspace=workspace;
    g_object_set_data_full(G_OBJECT(root),"umicom-trading-session-review",state,Free);
    GtkWidget *label=gtk_label_new("Review all retained orders, fills and positions. Consistency checks use the whole local book; no broker reconciliation or trading action is performed.");
    gtk_label_set_wrap(GTK_LABEL(label),TRUE);gtk_label_set_xalign(GTK_LABEL(label),0);
    gtk_box_append(GTK_BOX(root),label);
    GtkWidget *row=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,6);
    state->filter=gtk_entry_new();gtk_entry_set_placeholder_text(GTK_ENTRY(state->filter),"Instrument, symbol, venue or currency");
    gtk_entry_set_max_length(GTK_ENTRY(state->filter),UMI_TRADING_WORKSPACE_FILTER_CAPACITY-1);
    gtk_widget_set_hexpand(state->filter,TRUE);
    gtk_accessible_update_property(GTK_ACCESSIBLE(state->filter),GTK_ACCESSIBLE_PROPERTY_LABEL,"Session instrument filter",-1);
    state->refresh=gtk_button_new_with_label("Refresh report");state->copy=gtk_button_new_with_label("Copy captured CSV");
    gtk_widget_set_sensitive(state->copy,FALSE);
    gtk_box_append(GTK_BOX(row),state->filter);gtk_box_append(GTK_BOX(row),state->refresh);gtk_box_append(GTK_BOX(row),state->copy);
    gtk_box_append(GTK_BOX(root),row);
    state->message=gtk_label_new(NULL);gtk_label_set_wrap(GTK_LABEL(state->message),TRUE);gtk_label_set_xalign(GTK_LABEL(state->message),0);
    gtk_box_append(GTK_BOX(root),state->message);
    GtkWidget *view=gtk_text_view_new();state->buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view),FALSE);gtk_text_view_set_monospace(GTK_TEXT_VIEW(view),TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view),GTK_WRAP_WORD_CHAR);
    GtkWidget *scroll=gtk_scrolled_window_new();gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),view);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll),260);gtk_widget_set_vexpand(scroll,TRUE);
    gtk_box_append(GTK_BOX(root),scroll);
    (void)umi_gtk4_automation_tag_widget(root,"trading.session.report");
    (void)umi_gtk4_automation_tag_widget(state->filter,"trading.session.filter");
    (void)umi_gtk4_automation_tag_widget(state->refresh,"trading.session.refresh");
    (void)umi_gtk4_automation_tag_widget(state->copy,"trading.session.copy");
    (void)umi_gtk4_automation_tag_widget(state->message,"trading.session.message");
    (void)umi_gtk4_automation_tag_widget(view,"trading.session.text");
    g_signal_connect_object(state->refresh,"clicked",G_CALLBACK(Refresh),G_OBJECT(root),0);
    g_signal_connect_object(state->copy,"clicked",G_CALLBACK(Copy),G_OBJECT(root),0);
    g_signal_connect_object(state->filter,"changed",G_CALLBACK(Edited),G_OBJECT(root),0);
    Refresh(NULL,root);return root;
}
void UmiGtk4TradingSessionReportMarkStale(GtkWidget *panel)
{
    if(panel==NULL)return;
    SessionUi *state=State(panel);
    if(state==NULL||state->detached||state->report==NULL||state->error)return;
    bool current=false;
    if(UmiTradingSessionReportIsCurrent(state->report,state->workspace,&current)==UMI_STATUS_OK&&!current)
        gtk_label_set_text(GTK_LABEL(state->message),"The workspace changed after capture. This report and its CSV remain fixed; choose Refresh report for current records and the entered filter.");
}
void UmiGtk4TradingSessionReportDetach(GtkWidget *panel)
{
    if(panel==NULL)return;
    SessionUi *state=State(panel);
    if(state==NULL||state->detached)return;
    state->workspace=NULL;state->detached=true;
    gtk_widget_set_sensitive(state->filter,FALSE);gtk_widget_set_sensitive(state->refresh,FALSE);gtk_widget_set_sensitive(state->copy,FALSE);
    gtk_label_set_text(GTK_LABEL(state->message),"Workspace closed. The captured report remains readable; its actions are disabled.");
}
