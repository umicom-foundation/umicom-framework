/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_queue/native_cases.inc
 * PURPOSE: Exercise the actual Bank review queue with a memory-only service and retained controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/selection_list.h"
static int VerifyNativeQueue(const char *name)
{
    GtkWidget *window;BankUi *ui=Create(&window);Fixture f={0};f.bank=ui->operations;f.serial=100;
    Id(&f.maker.id,"test-maker");f.maker.capabilities=UMI_BANK_CAP_PAYMENTS;
    Id(&f.checker.id,"test-checker");f.checker.capabilities=UMI_BANK_CAP_APPROVE;
    UmiBankCommand c=Make(&f,UMI_BANK_CHARGE_SUBMIT,"queue-charge");Id(&c.ownerId,"queue-reference");Id(&c.sourceAccountId,"account");
    strcpy(c.name,"Queue charge");c.amount=Cash(250);Send(&f,&f.maker,&c);
    QueueCreate(ui);Refresh(ui);CHECK(ui->workQueue!=NULL);
    UmiBankWorkQueueSummary summary;OK(UmiBankWorkQueueSummaryRead(ui->workQueue,&summary));CHECK(summary.count==1&&summary.revision==4);
/* Check the absence of a business choice, including the explicit GTK prompt. Retain the earlier raw-index assertion for review. */
#if 0
    CHECK(gtk_drop_down_get_selected(ui->queueRows)==GTK_INVALID_LIST_POSITION);
#endif
    CHECK(UmiGtk4SelectionListSelected(ui->queueRows)==GTK_INVALID_LIST_POSITION);
    gtk_drop_down_set_selected(ui->queueRows,0);gtk_drop_down_set_selected(ui->identity,1);
    if(strcmp(name,"queue-capture")==0){
        CHECK(Contains(ui->queueDetails,"queue-charge")&&Contains(ui->queueDetails,"GBP 2.50"));Balance(&f,100000,0);
        CHECK(gtk_widget_get_sensitive(ui->queueReviewButton));
    }else if(strcmp(name,"queue-review")==0||strcmp(name,"queue-posted")==0){
        g_signal_emit_by_name(ui->queueReviewButton,"clicked");CHECK(ui->review!=NULL&&Contains(ui->reviewBuffer,"queue-charge"));
        CHECK(strcmp(EntryText(ui->id),"queue-charge")==0&&gtk_drop_down_get_selected(ui->action)==UMI_BANK_CHARGE_APPROVE-1U);
        Balance(&f,100000,0);
        if(strcmp(name,"queue-posted")==0){
            Submit(ui->submit,window);CHECK(ui->review==NULL);OK(UmiBankWorkQueueSummaryRead(ui->workQueue,&summary));CHECK(summary.approved==1);
            gtk_drop_down_set_selected(ui->identity,2);gtk_drop_down_set_selected(ui->queueDecision,3);
            g_signal_emit_by_name(ui->queueReviewButton,"clicked");CHECK(ui->review!=NULL);Submit(ui->submit,window);Balance(&f,99750,0);
            OK(UmiBankWorkQueueSummaryRead(ui->workQueue,&summary));CHECK(summary.count==0);
/* Check the absence of a business choice, including the explicit GTK prompt. Retain the earlier raw-index assertion for review. */
#if 0
            CHECK(gtk_drop_down_get_selected(ui->queueRows)==GTK_INVALID_LIST_POSITION&&!gtk_widget_get_sensitive(ui->queueReviewButton));
#endif
            CHECK(UmiGtk4SelectionListSelected(ui->queueRows)==GTK_INVALID_LIST_POSITION&&!gtk_widget_get_sensitive(ui->queueReviewButton));
        }
    }else if(strcmp(name,"queue-filter")==0){
        g_signal_emit_by_name(ui->queueReviewButton,"clicked");CHECK(ui->review!=NULL);
        gtk_editable_set_text(GTK_EDITABLE(ui->queueAccount),"missing");CHECK(ui->workQueue==NULL&&ui->review==NULL);
        CHECK(!gtk_widget_get_sensitive(ui->queueReviewButton)&&!gtk_widget_get_sensitive(ui->queueCopyButton));
        QueueRefreshClicked(NULL,window);CHECK(ui->workQueue==NULL);
        gtk_editable_set_text(GTK_EDITABLE(ui->queueAccount),"");QueueRefreshClicked(NULL,window);CHECK(ui->workQueue!=NULL);
    }else if(strcmp(name,"queue-stale")==0){
        c=Make(&f,UMI_BANK_CHARGE_APPROVE,"queue-charge");Send(&f,&f.checker,&c);
        g_signal_emit_by_name(ui->queueReviewButton,"clicked");CHECK(ui->review==NULL);
        UmiBankCounts counts;OK(UmiBankOperationsCounts(ui->operations,&counts));CHECK(counts.revision==5);
        CHECK(strstr(gtk_label_get_text(ui->message),"State changed")!=NULL);
        QueueRefreshClicked(NULL,window);OK(UmiBankWorkQueueSummaryRead(ui->workQueue,&summary));CHECK(summary.approved==1);
    }else if(strcmp(name,"queue-copy")==0){
        GdkClipboard *clipboard=gtk_widget_get_clipboard(ui->queueCopyButton);gdk_clipboard_set_text(clipboard,"sentinel");
        g_signal_emit_by_name(ui->queueCopyButton,"clicked");char *text=ReportClipboard(clipboard);
        CHECK(strstr(text,"queue-charge")!=NULL&&strstr(text,"LOCAL PRACTICE")!=NULL);g_free(text);
    }else if(strcmp(name,"queue-close")==0){
        GtkWidget *review=g_object_ref(ui->queueReviewButton),*copy=g_object_ref(ui->queueCopyButton),*filter=g_object_ref(GTK_WIDGET(ui->queueAccount));
        gtk_window_destroy(GTK_WINDOW(window));CHECK(ui->closed&&ui->workQueue==NULL&&ui->operations==NULL);
        g_signal_emit_by_name(review,"clicked");g_signal_emit_by_name(copy,"clicked");gtk_editable_set_text(GTK_EDITABLE(filter),"account");
        g_object_unref(window);window=NULL;
        g_signal_emit_by_name(review,"clicked");g_signal_emit_by_name(copy,"clicked");
        g_object_unref(review);g_object_unref(copy);g_object_unref(filter);
    }else CHECK(0);
    if(window!=NULL){gtk_window_destroy(GTK_WINDOW(window));g_object_unref(window);}
    return 0;
}
