/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_reservations/native_cases.inc
 * PURPOSE: Exercise capture, explicit release review, invalidation, clipboard and retained native controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ui/gtk4/selection_list.h"
#include "fixture.h"
static int VerifyNativeReservations(const char *name)
{
    GtkWidget *window;BankUi *ui=Create(&window);Fixture f={0};f.bank=ui->operations;f.serial=100;ReservationActors(&f);ReservationPopulate(&f);
    ReservationsCreate(ui);GtkWidget *capture=ActivityFind(window,"bank.reservations.capture");CHECK(capture!=NULL);
    gtk_editable_set_text(GTK_EDITABLE(ui->reservationAccount),"account");g_signal_emit_by_name(capture,"clicked");
    CHECK(ui->reservations!=NULL && Contains(ui->reservationText,"Available = booked minus reserved: GBP 940.00"));
/* Check the absence of a business choice, including the explicit GTK prompt. Retain the earlier raw-index assertion for review. */
#if 0
    CHECK(gtk_drop_down_get_selected(ui->reservationRows)==GTK_INVALID_LIST_POSITION && !gtk_widget_get_sensitive(ui->reservationReview));
#endif
    CHECK(UmiGtk4SelectionListSelected(ui->reservationRows)==GTK_INVALID_LIST_POSITION && !gtk_widget_get_sensitive(ui->reservationReview));
    if(strcmp(name,"reservations-account")==0){
        gtk_editable_set_text(GTK_EDITABLE(ui->reservationAccount),"missing");CHECK(ui->reservations==NULL && !gtk_widget_get_sensitive(ui->reservationCopy));
        g_signal_emit_by_name(capture,"clicked");CHECK(ui->reservations==NULL && Contains(ui->reservationText,"Account not found"));
    }else if(strcmp(name,"reservations-copy")==0 || strcmp(name,"reservations-cached")==0){
        if(strcmp(name,"reservations-cached")==0){UmiBankCommand c=Make(&f,UMI_BANK_HOLD_RELEASE,"shared-id");Send(&f,&f.operator,&c);Refresh(ui);}
        GdkClipboard *clipboard=gtk_widget_get_clipboard(ui->reservationCopy);g_signal_emit_by_name(ui->reservationCopy,"clicked");char *text=ReportClipboard(clipboard);
        CHECK(strstr(text,"\"reservation-summary\"")!=NULL && strstr(text,"\"94000\"")!=NULL);g_free(text);
        CHECK(Contains(ui->reservationText,"Reserved: GBP 60.00"));
    }else if(strcmp(name,"reservations-close")==0){
        GtkWidget *copy=g_object_ref(ui->reservationCopy),*entry=g_object_ref(GTK_WIDGET(ui->reservationAccount)),*review=g_object_ref(ui->reservationReview);g_object_ref(capture);
        gtk_window_destroy(GTK_WINDOW(window));CHECK(ui->closed && ui->operations==NULL && ui->reservations==NULL);
        g_signal_emit_by_name(capture,"clicked");g_signal_emit_by_name(review,"clicked");g_signal_emit_by_name(copy,"clicked");gtk_editable_set_text(GTK_EDITABLE(entry),"retained");
        g_object_unref(window);window=NULL;g_signal_emit_by_name(capture,"clicked");g_signal_emit_by_name(review,"clicked");
        g_object_unref(copy);g_object_unref(entry);g_object_unref(review);g_object_unref(capture);
    }else{
        guint index=strcmp(name,"reservations-card")==0?1U:strcmp(name,"reservations-transfer")==0?2U:0U;
        gtk_drop_down_set_selected(ui->identity,strcmp(name,"reservations-permission")==0?1U:2U);
        gtk_drop_down_set_selected(ui->reservationRows,index);
        if(strcmp(name,"reservations-stale")==0){UmiBankCommand c=Make(&f,UMI_BANK_HOLD_RELEASE,"shared-id");Send(&f,&f.operator,&c);}
        g_signal_emit_by_name(ui->reservationReview,"clicked");
        if(strcmp(name,"reservations-permission")==0 || strcmp(name,"reservations-stale")==0)CHECK(ui->review==NULL && !gtk_widget_get_sensitive(GTK_WIDGET(ui->submit)));
        else{
            CHECK(ui->review!=NULL && gtk_widget_get_sensitive(GTK_WIDGET(ui->submit)));Balance(&f,100000,6000);
            if(strcmp(name,"reservations-invalidate")==0){gtk_editable_set_text(GTK_EDITABLE(ui->reservationAccount),"destination");CHECK(ui->review==NULL);Submit(NULL,window);Balance(&f,100000,6000);}
            else{
                CHECK(strcmp(name,"reservations-manual")==0 || strcmp(name,"reservations-card")==0 || strcmp(name,"reservations-transfer")==0);
                CHECK(Contains(ui->reviewBuffer,index==2U?"Resolved transfer":"Original reservation:"));
                Submit(NULL,window);Balance(&f,100000,6000-(index==0U?1000:index==1U?2000:3000));
                CHECK(Contains(ui->reservationText,"Reserved: GBP 60.00"));
                g_signal_emit_by_name(ui->reservationReview,"clicked");CHECK(ui->review==NULL);
            }
        }
    }
    if(window!=NULL){gtk_window_destroy(GTK_WINDOW(window));g_object_unref(window);}return 0;
}
