/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Actual GTK panel: select the existing Accounting periods report, confirm
 * no write, then retain widgets after controller and service destruction. */
#include "umicom/ui/gtk4/finance_operations.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"GTK close check: %s\n",#x);return 1;}}while(0)
static GtkWidget *Find(GtkWidget *root,const char *name)
{
    if(!strcmp(gtk_widget_get_name(root),name))return root;
    for(GtkWidget *c=gtk_widget_get_first_child(root);c;c=gtk_widget_get_next_sibling(c)) {
        GtkWidget *found=Find(c,name);if(found)return found;
    }
    return NULL;
}
int main(void)
{
    if(!gtk_init_check()){puts("No GTK display: NOT RUN");return 77;}
    UmiDataServer *server=NULL;UmiFinanceOperations *o=NULL;UmiFinanceOperationsGtkPanel *panel=NULL;
    CHECK(umi_data_server_create_memory(&server)==UMI_STATUS_OK);
    CHECK(UmiFinanceOperationsCreate(server,&o)==UMI_STATUS_OK);
    UmiFinanceOperationCommand c;UmiFinanceOperationCommandInit(&c);c.kind=UMI_FINANCE_OPEN_PERIOD;
    CHECK(umi_financial_id_assign(&c.id,"september")==UMI_STATUS_OK);
    CHECK(umi_financial_id_assign(&c.actorId,"operator")==UMI_STATUS_OK);
    CHECK(umi_financial_id_assign(&c.requestId,"one")==UMI_STATUS_OK);
    c.date=(UmiFinancialDate){2026,9U,1U};c.endDate=(UmiFinancialDate){2026,9U,30U};
    UmiFinanceOperationReceipt receipt;CHECK(UmiFinanceOperationsApply(o,&c,&receipt)==UMI_STATUS_OK);
    CHECK(UmiFinanceOperationsGtkPanelCreate(o,"Practice only",&panel)==UMI_STATUS_OK);
    GtkWidget *root=UmiFinanceOperationsGtkPanelWidget(panel);g_object_ref(root);
    GtkWidget *choice=Find(root,"finance-report-view"),*report=Find(root,"finance-report"),*refresh=Find(root,"finance-refresh-report");
    CHECK(choice&&report&&refresh);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(choice),2U);g_signal_emit_by_name(refresh,"clicked");
    GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(report));GtkTextIter start,end;
    gtk_text_buffer_get_bounds(buffer,&start,&end);char *text=gtk_text_buffer_get_text(buffer,&start,&end,FALSE);
    CHECK(strstr(text,"CLOSE REVIEW - september")&&strstr(text,"blocking records: 0"));g_free(text);
    UmiFinanceOperationCounts counts;CHECK(UmiFinanceOperationsCounts(o,&counts)==UMI_STATUS_OK&&counts.revision==1U);
    UmiFinanceOperationsGtkPanelDestroy(panel);UmiFinanceOperationsDestroy(o);umi_data_server_destroy(server);
    g_signal_emit_by_name(refresh,"clicked");gtk_drop_down_set_selected(GTK_DROP_DOWN(choice),0U);
    g_object_unref(root);puts("Close report rendering and retained-widget lifecycle passed.");return 0;
}
