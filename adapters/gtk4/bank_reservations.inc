/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/bank_reservations.inc
 * PURPOSE: Explain account reservations and prepare explicit canonical release reviews.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Included after the existing form, review and tagging helpers. A report owns
 * its evidence, while window-bound callbacks reject controls retained after close. */
#include "umicom/ui/gtk4/selection_list.h"

static void ReservationsClear(BankUi *ui,const char *message)
{
    UmiBankReservationsDestroy(ui->reservations);ui->reservations=NULL;
    InvalidateReview(ui);
    gtk_drop_down_set_model(ui->reservationRows,NULL);
    gtk_text_buffer_set_text(ui->reservationText,message,-1);
    gtk_widget_set_sensitive(ui->reservationCopy,FALSE);gtk_widget_set_sensitive(ui->reservationReview,FALSE);
}
static void ReservationsSelection(GObject *object,GParamSpec *spec,gpointer window)
{
    (void)object;(void)spec;BankUi *ui=SignalUi(window);if(ui==NULL)return;
    InvalidateReview(ui);UmiBankAction action;
/* Read only business rows; the displayed prompt is not an instruction. Retain the raw GTK selection reader for review. */
#if 0
    UmiStatus status=UmiBankReservationsReleaseAction(ui->reservations,gtk_drop_down_get_selected(ui->reservationRows),&action);
#endif
    UmiStatus status=UmiBankReservationsReleaseAction(ui->reservations,UmiGtk4SelectionListSelected(ui->reservationRows),&action);
    gtk_widget_set_sensitive(ui->reservationReview,status==UMI_STATUS_OK);
    if(status==UMI_STATUS_OK){
        const char *label=action==UMI_BANK_HOLD_RELEASE?"Review manual hold release":
            action==UMI_BANK_CARD_VOID?"Review card authorisation void":"Review transfer cancellation";
        gtk_button_set_label(GTK_BUTTON(ui->reservationReview),label);
    }else gtk_button_set_label(GTK_BUTTON(ui->reservationReview),"Review release / cancel");
}
static void ReservationsAccountChanged(GtkEditable *editable,gpointer window)
{
    (void)editable;BankUi *ui=SignalUi(window);if(ui!=NULL)ReservationsClear(ui,"Account changed. Capture reserved funds again.");
}
static void ReservationsCaptureClicked(GtkButton *button,gpointer window)
{
    (void)button;BankUi *ui=SignalUi(window);if(ui==NULL)return;
    /* Refresh the cached model presentation so the shared command form uses
     * the same revision as this explicit capture. This does not reload storage. */
    Refresh(ui);
    ReservationsClear(ui,"No reserved-funds report captured.");
    UmiBankReservations *next=NULL;UmiStatus status=UmiBankReservationsCapture(ui->operations,EntryText(ui->reservationAccount),&next);
    size_t required=0U;if(status==UMI_STATUS_OK)status=UmiBankReservationsDescribe(next,NULL,0U,&required);
    char *text=status==UMI_STATUS_OK?g_try_malloc(required):NULL;
    if(status==UMI_STATUS_OK && text==NULL)status=UMI_STATUS_OUT_OF_MEMORY;
    if(status==UMI_STATUS_OK)status=UmiBankReservationsDescribe(next,text,required,NULL);
    if(status!=UMI_STATUS_OK){
        g_free(text);UmiBankReservationsDestroy(next);
        ReservationsClear(ui,status==UMI_STATUS_NOT_FOUND?"Account not found. Enter its exact ID.":
            "Reserved funds could not be captured. Check the account ID and storage status; no data changed.");return;
    }
    ui->reservations=next;char *valid=g_utf8_make_valid(text,-1);
    gtk_text_buffer_set_text(ui->reservationText,valid,-1);g_free(valid);g_free(text);
    UmiBankReservationsSummary summary;(void)UmiBankReservationsReadSummary(next,&summary);
    GtkStringList *rows=gtk_string_list_new(NULL);
    for(size_t i=0;i<summary.count;++i){
        UmiBankReservationRow row;(void)UmiBankReservationsRowAt(next,i,&row);char money[UMI_MONEY_TEXT_CAPACITY];
        status=UmiMoneyTextFormat(&row.amount,money,sizeof money,NULL);
        if(status!=UMI_STATUS_OK){g_object_unref(rows);ReservationsClear(ui,"Reserved amount could not be displayed.");return;}
        char *label=g_strdup_printf("%s | %s | %s",UmiBankReservationKindName(row.kind),row.id.value,money);
        valid=g_utf8_make_valid(label,-1);gtk_string_list_append(rows,valid);g_free(valid);g_free(label);
    }
/* Publish model and explicit choice together. The shared prompt replaces invalid-index clearing, which GTK may ignore; retain the old publication for review. */
#if 0
    gtk_drop_down_set_model(ui->reservationRows,G_LIST_MODEL(rows));g_object_unref(rows);
    /* A new capture never silently selects a different financial record. */
    gtk_drop_down_set_selected(ui->reservationRows,GTK_INVALID_LIST_POSITION);
#endif
    /* A new capture never silently selects a different financial record. */
    (void)UmiGtk4SelectionListPublish(ui->reservationRows, rows, GTK_INVALID_LIST_POSITION);
    g_object_unref(rows);
    gtk_widget_set_sensitive(ui->reservationCopy,TRUE);ReservationsSelection(NULL,NULL,window);
}
static void ReservationsReviewClicked(GtkButton *button,gpointer window)
{
    (void)button;BankUi *ui=SignalUi(window);if(ui==NULL)return;
/* Read only business rows; the displayed prompt is not an instruction. Retain the raw GTK selection reader for review. */
#if 0
    InvalidateReview(ui);guint index=gtk_drop_down_get_selected(ui->reservationRows);
#endif
    InvalidateReview(ui);guint index=UmiGtk4SelectionListSelected(ui->reservationRows);
    UmiBankReservationRow row;UmiBankAction action;
    UmiStatus status=UmiBankReservationsRowAt(ui->reservations,index,&row);
    if(status==UMI_STATUS_OK)status=UmiBankReservationsReleaseAction(ui->reservations,index,&action);
    if(status!=UMI_STATUS_OK){Message(ui,status);return;}
    gtk_drop_down_set_selected(ui->action,(guint)action-1U);ActionChanged(NULL,NULL,window);
    gtk_editable_set_text(GTK_EDITABLE(ui->id),row.id.value);NewRequest(NULL,window);
    UmiBankActor actor;UmiBankCommand command;
    if(!ReadCommand(ui,&actor,&command)){Message(ui,UMI_STATUS_INVALID_ARGUMENT);return;}
    status=UmiBankReservationsReviewRelease(ui->operations,ui->reservations,index,&actor,&command,&ui->review);
    if(status==UMI_STATUS_OK)DisplayReview(ui);
    else if(status==UMI_STATUS_BUSY)gtk_label_set_text(ui->message,"The captured reservations changed. Reload committed data if needed, capture reserved funds again, then review.");
    else Message(ui,status);
}
static void ReservationsCopyClicked(GtkButton *button,gpointer window)
{
    BankUi *ui=SignalUi(window);if(ui==NULL || ui->reservations==NULL)return;
    UmiCsvDocument *document=NULL;UmiStatus status=UmiBankReservationsExportCsv(ui->reservations,&document);
    if(status==UMI_STATUS_OK){
        gdk_clipboard_set_text(gtk_widget_get_clipboard(GTK_WIDGET(button)),UmiCsvDocumentData(document));
        gtk_label_set_text(ui->message,"Copied the captured reserved-funds report and revision. No data was reloaded or changed.");
    }else Message(ui,status);
    UmiCsvDocumentDestroy(document);
}
static void ReservationsCreate(BankUi *ui)
{
    GtkWidget *page=gtk_box_new(GTK_ORIENTATION_VERTICAL,6),*fields=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,6);
    ui->reservationAccount=GTK_ENTRY(QueueTag(gtk_entry_new(),"bank.reservations.account"));
    gtk_entry_set_placeholder_text(ui->reservationAccount,"Exact account ID");
    gtk_accessible_update_property(GTK_ACCESSIBLE(ui->reservationAccount),GTK_ACCESSIBLE_PROPERTY_LABEL,"Reserved funds account ID",-1);
    gtk_box_append(GTK_BOX(fields),gtk_label_new("Account ID"));gtk_box_append(GTK_BOX(fields),GTK_WIDGET(ui->reservationAccount));
    GtkWidget *capture=QueueTag(gtk_button_new_with_label("Capture reserved funds"),"bank.reservations.capture");
    gtk_box_append(GTK_BOX(fields),capture);gtk_box_append(GTK_BOX(page),fields);
    GtkWidget *hint=gtk_label_new("Read the captured balance explanation, then explicitly select one reservation to review. Reload committed data first for another window's changes. Release/void/cancel is a local practice action.");
    gtk_label_set_wrap(GTK_LABEL(hint),TRUE);gtk_label_set_xalign(GTK_LABEL(hint),0);gtk_box_append(GTK_BOX(page),hint);
    ui->reservationRows=GTK_DROP_DOWN(QueueTag(gtk_drop_down_new(NULL,NULL),"bank.reservations.rows"));
    gtk_accessible_update_property(GTK_ACCESSIBLE(ui->reservationRows),GTK_ACCESSIBLE_PROPERTY_LABEL,"Captured reservation",-1);
    gtk_box_append(GTK_BOX(page),GTK_WIDGET(ui->reservationRows));
    GtkWidget *scroll=gtk_scrolled_window_new(),*view=QueueTag(gtk_text_view_new(),"bank.reservations.report");
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view),FALSE);gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view),GTK_WRAP_WORD_CHAR);
    ui->reservationText=gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),view);
    gtk_widget_set_vexpand(scroll,TRUE);gtk_box_append(GTK_BOX(page),scroll);
    ui->reservationReview=QueueTag(gtk_button_new_with_label("Review release / cancel"),"bank.reservations.review");
    ui->reservationCopy=QueueTag(gtk_button_new_with_label("Copy captured reservations CSV"),"bank.reservations.copy-csv");
    gtk_box_append(GTK_BOX(page),ui->reservationReview);gtk_box_append(GTK_BOX(page),ui->reservationCopy);
    gtk_notebook_append_page(ui->pages,page,gtk_label_new("Reserved funds"));
    g_signal_connect_object(ui->reservationAccount,"changed",G_CALLBACK(ReservationsAccountChanged),G_OBJECT(ui->window),0);
    g_signal_connect_object(ui->reservationRows,"notify::selected",G_CALLBACK(ReservationsSelection),G_OBJECT(ui->window),0);
    g_signal_connect_object(capture,"clicked",G_CALLBACK(ReservationsCaptureClicked),G_OBJECT(ui->window),0);
    g_signal_connect_object(ui->reservationReview,"clicked",G_CALLBACK(ReservationsReviewClicked),G_OBJECT(ui->window),0);
    g_signal_connect_object(ui->reservationCopy,"clicked",G_CALLBACK(ReservationsCopyClicked),G_OBJECT(ui->window),0);
    ReservationsClear(ui,"Enter an account ID and capture its reserved funds. This report does not update automatically.");
}
