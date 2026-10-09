/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_audit/native_cases.inc
 * PURPOSE: Exercise audit capture, stable selections, clipboard exports and window teardown through real native controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/selection_list.h"
#include "../bank_audit/fixture.h"
static int VerifyNativeAudit(const char *name)
{
    GtkWidget *window;BankUi *ui=Create(&window);AuditCreate(ui);
    /* The shared action catalogue supplies every selectable action exactly
     * once. The GTK string list must stop after its final entry. */
    GListModel *actions=gtk_drop_down_get_model(ui->auditAction);
    CHECK(GTK_IS_STRING_LIST(actions));
    CHECK(g_list_model_get_n_items(actions)==UMI_BANK_ACTION_LAST+1U);
    CHECK(strcmp(gtk_string_list_get_string(GTK_STRING_LIST(actions),0),"All actions")==0);
    for(unsigned action=1;action<=UMI_BANK_ACTION_LAST;++action){
        CHECK(strcmp(gtk_string_list_get_string(GTK_STRING_LIST(actions),action),UmiBankActionName((UmiBankAction)action))==0);
    }

    Fixture f={0};f.bank=ui->operations;f.serial=3;
    Id(&f.maker.id,"maker");f.maker.capabilities=UMI_BANK_CAP_CUSTOMERS|UMI_BANK_CAP_PAYMENTS;
    Id(&f.checker.id,"checker");f.checker.capabilities=UMI_BANK_CAP_APPROVE;
    Id(&f.operator.id,"operator");f.operator.capabilities=UMI_BANK_CAP_OPERATE|UMI_BANK_CAP_TEST_FUNDING;AuditPopulate(&f);
    GtkWidget *capture=ActivityFind(window,"bank.audit.capture");CHECK(GTK_IS_BUTTON(capture) && !gtk_widget_get_sensitive(ui->auditCopy));
    g_signal_emit_by_name(capture,"clicked");CHECK(ui->auditReport!=NULL && Contains(ui->auditSummary,"Captured revision: 9"));
/* Check the absence of a business choice, including the explicit GTK prompt. Retain the earlier raw-index assertion for review. */
#if 0
    CHECK(gtk_drop_down_get_selected(ui->auditRows)==GTK_INVALID_LIST_POSITION && gtk_widget_get_sensitive(ui->auditCopy));
#endif
    CHECK(UmiGtk4SelectionListSelected(ui->auditRows)==GTK_INVALID_LIST_POSITION && gtk_widget_get_sensitive(ui->auditCopy));
    UmiBankAuditReport *original=ui->auditReport;
    if(strcmp(name,"audit-capture")==0){CHECK(Contains(ui->auditSummary,"Matching accepted commands: 9 of 9"));}
    else if(strcmp(name,"audit-filters")==0){
        gtk_editable_set_text(GTK_EDITABLE(ui->auditEntity),"shared");gtk_drop_down_set_selected(ui->auditFamily,UMI_BANK_AUDIT_CHARGE);
        g_signal_emit_by_name(capture,"clicked");CHECK(AuditSummary(ui->auditReport).count==4 && AuditSummary(ui->auditReport).journalCount==2);
    }else if(strcmp(name,"audit-draft-filters")==0){
        gtk_drop_down_set_selected(ui->auditRows,5);CHECK(Contains(ui->auditDetail,"bank-journal-6"));
        gtk_editable_set_text(GTK_EDITABLE(ui->auditActor),"checker");CHECK(ui->auditReport==original && Contains(ui->auditDetail,"bank-journal-6"));
/* Check the absence of a business choice, including the explicit GTK prompt. Retain the earlier raw-index assertion for review. */
#if 0
        g_signal_emit_by_name(capture,"clicked");CHECK(AuditSummary(ui->auditReport).count==1 && gtk_drop_down_get_selected(ui->auditRows)==GTK_INVALID_LIST_POSITION);
#endif
        g_signal_emit_by_name(capture,"clicked");CHECK(AuditSummary(ui->auditReport).count==1 && UmiGtk4SelectionListSelected(ui->auditRows)==GTK_INVALID_LIST_POSITION);
        CHECK(!Contains(ui->auditDetail,"bank-journal-6"));
    }else if(strcmp(name,"audit-invalid")==0){
        gtk_editable_set_text(GTK_EDITABLE(ui->auditFrom),"2026-02-30");g_signal_emit_by_name(capture,"clicked");
        CHECK(ui->auditReport==original && Contains(ui->auditSummary,"Captured revision: 9") && strstr(gtk_label_get_text(ui->auditHint),"Capture failed"));
        gtk_editable_set_text(GTK_EDITABLE(ui->auditFrom),"");gtk_editable_set_text(GTK_EDITABLE(ui->auditFirst),"18446744073709551616");g_signal_emit_by_name(capture,"clicked");CHECK(ui->auditReport==original);
    }else if(strcmp(name,"audit-empty")==0){
        gtk_editable_set_text(GTK_EDITABLE(ui->auditRequest),"missing");g_signal_emit_by_name(capture,"clicked");
        CHECK(AuditSummary(ui->auditReport).count==0 && Contains(ui->auditSummary,"No accepted commands match") && gtk_widget_get_sensitive(ui->auditCopy));
    }else if(strcmp(name,"audit-selection")==0){
        gtk_drop_down_set_selected(ui->auditRows,4);CHECK(Contains(ui->auditDetail,"This command created no booked journal"));
        gtk_drop_down_set_selected(ui->auditRows,5);CHECK(Contains(ui->auditDetail,"bank-journal-6") && !Contains(ui->auditDetail,"bank-journal-7"));
        gtk_drop_down_set_selected(ui->auditRows,6);CHECK(Contains(ui->auditDetail,"compensating reversal: yes") && !Contains(ui->auditDetail,"bank-journal-6"));
    }else if(strcmp(name,"audit-copy")==0 || strcmp(name,"audit-journals")==0 || strcmp(name,"audit-cached")==0){
        if(strcmp(name,"audit-cached")==0){UmiBankCommand c=Make(&f,UMI_BANK_TEST_CREDIT,"later");Id(&c.sourceAccountId,"account");c.amount=Cash(1);Send(&f,&f.operator,&c);Refresh(ui);CHECK(ui->auditReport==original);}
        GtkWidget *copy=strcmp(name,"audit-journals")==0?ui->auditJournalCopy:ui->auditCopy;
        GdkClipboard *clipboard=gtk_widget_get_clipboard(copy);gdk_clipboard_set_text(clipboard,"before");g_signal_emit_by_name(copy,"clicked");char *text=ReportClipboard(clipboard);
        CHECK(strstr(text,"LOCAL PRACTICE") && strstr(text,strcmp(name,"audit-journals")==0?"bank-journal-6":"accepted-command") && !strstr(text,"\"later\""));g_free(text);
    }else if(strcmp(name,"audit-close")==0){
        GtkWidget *copy=g_object_ref(ui->auditCopy),*journals=g_object_ref(ui->auditJournalCopy),*entry=g_object_ref(GTK_WIDGET(ui->auditEntity));g_object_ref(capture);
        GdkClipboard *clipboard=g_object_ref(gtk_widget_get_clipboard(copy));gtk_window_destroy(GTK_WINDOW(window));CHECK(ui->closed && ui->operations==NULL && ui->auditReport==NULL);
        gdk_clipboard_set_text(clipboard,"closed-audit-sentinel");g_signal_emit_by_name(capture,"clicked");g_signal_emit_by_name(copy,"clicked");gtk_editable_set_text(GTK_EDITABLE(entry),"retained");
        g_object_unref(window);window=NULL;g_signal_emit_by_name(capture,"clicked");g_signal_emit_by_name(journals,"clicked");
        char *text=ReportClipboard(clipboard);CHECK(strcmp(text,"closed-audit-sentinel")==0);g_free(text);
        g_object_unref(clipboard);g_object_unref(copy);g_object_unref(journals);g_object_unref(entry);g_object_unref(capture);
    }else return 2;
    if(window!=NULL){UmiBankCounts counts;OK(UmiBankOperationsCounts(ui->operations,&counts));CHECK(counts.revision==(strcmp(name,"audit-cached")==0?10U:9U));gtk_window_destroy(GTK_WINDOW(window));g_object_unref(window);}
    return 0;
}
