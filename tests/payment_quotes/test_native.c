/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/payment_quotes/test_native.c
 * PURPOSE: Exercise real pricing controls and Bank closure without opening a user database.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../../adapters/gtk4/bank_operations_gtk4.c"
#include "fixture.h"
static GtkWidget *Find(GtkWidget *root,const char *tag)
{
    const char *id=g_object_get_data(G_OBJECT(root),"umicom-automation-id");if(id!=NULL&&strcmp(id,tag)==0)return root;
    for(GtkWidget *child=gtk_widget_get_first_child(root);child!=NULL;child=gtk_widget_get_next_sibling(child)){
        GtkWidget *found=Find(child,tag);if(found!=NULL)return found;
    }
    return NULL;
}
static void Edit(GtkWidget *root,const char *tag,const char *text)
{ GtkWidget *entry=Find(root,tag);CHECK(entry!=NULL);gtk_editable_set_text(GTK_EDITABLE(entry),text); }
static char *Text(GtkWidget *root)
{
    GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(root,"payment.quote.result")));GtkTextIter start,end;
    gtk_text_buffer_get_bounds(buffer,&start,&end);return gtk_text_buffer_get_text(buffer,&start,&end,FALSE);
}
int main(int argc,char **argv)
{
    CHECK(argc==2);if(!gtk_init_check())return 77;
    GtkWidget *root=UmiGtk4PaymentQuoteCreate();g_object_ref_sink(root);
    GtkWidget *calculate=Find(root,"payment.quote.calculate"),*copy=Find(root,"payment.quote.copy");CHECK(calculate&&copy&&!gtk_widget_get_sensitive(copy));
    g_signal_emit_by_name(calculate,"clicked");char *before=Text(root);CHECK(strstr(before,"Principal plus charges: 100.35")&&gtk_widget_get_sensitive(copy));
    const char *test=argv[1];
    if(strcmp(test,"calculate")==0){
        Edit(root,"payment.quote.tax","2000");g_signal_emit_by_name(calculate,"clicked");char *text=Text(root);CHECK(strstr(text,"Principal plus charges: 100.42"));g_free(text);
    }else if(strcmp(test,"edit")==0){
        Edit(root,"payment.quote.principal","200.00");CHECK(!gtk_widget_get_sensitive(copy));
        char *text=Text(root);CHECK(strcmp(text,before)==0);g_free(text);
        g_signal_emit_by_name(copy,"clicked");g_signal_emit_by_name(calculate,"clicked");CHECK(gtk_widget_get_sensitive(copy));
        text=Text(root);CHECK(strstr(text,"Principal plus charges: 200.60"));g_free(text);
    }else if(strcmp(test,"rounding")==0){
        Edit(root,"payment.quote.principal","0.05");Edit(root,"payment.quote.fixed","0.00");Edit(root,"payment.quote.rate","5000");
        gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root,"payment.quote.fee-rounding")),UMI_MONEY_HALF_AWAY);
        CHECK(!gtk_widget_get_sensitive(copy));g_signal_emit_by_name(calculate,"clicked");char *text=Text(root);CHECK(strstr(text,"Fee after cap: 0.03"));g_free(text);
    }else if(strcmp(test,"invalid")==0){
        const char *bad[]={"1e2","0.001","-1.00","","1,00"};
        for(size_t i=0;i<sizeof(bad)/sizeof(bad[0]);++i){Edit(root,"payment.quote.principal",bad[i]);g_signal_emit_by_name(calculate,"clicked");
            char *text=Text(root);CHECK(strcmp(text,before)==0&&!gtk_widget_get_sensitive(copy));g_free(text);}
    }else if(strcmp(test,"overflow")==0){
        Edit(root,"payment.quote.principal","92233720368547758.07");g_signal_emit_by_name(calculate,"clicked");
        char *text=Text(root);CHECK(strcmp(text,before)==0&&!gtk_widget_get_sensitive(copy));g_free(text);
    }else if(strcmp(test,"detach")==0){
        UmiGtk4PaymentQuoteDetach(root);UmiGtk4PaymentQuoteDetach(root);CHECK(!gtk_widget_get_sensitive(calculate)&&!gtk_widget_get_sensitive(copy));
        Edit(root,"payment.quote.principal","200.00");g_signal_emit_by_name(calculate,"clicked");g_signal_emit_by_name(copy,"clicked");
        char *text=Text(root);CHECK(strcmp(text,before)==0);g_free(text);
    }else if(strcmp(test,"owner-close")==0){
        BankUi *ui=g_new0(BankUi,1);OK(UmiBankOperationsOpenMemory(&ui->operations));
        GtkWidget *window=gtk_window_new();g_object_ref_sink(window);ui->window=GTK_WINDOW(window);ui->feeCalculator=g_object_ref(root);
        g_object_set_data_full(G_OBJECT(window),"umicom-bank-operations",ui,UiFree);
        UmiGtk4ObserveWindowRemoval(GTK_WINDOW(window),G_OBJECT(window),UiClosed,ui);
        gtk_window_set_child(GTK_WINDOW(window),root);gtk_window_destroy(GTK_WINDOW(window));
        while(g_main_context_iteration(NULL,FALSE)){}
        CHECK(ui->closed&&ui->operations==NULL&&!gtk_widget_get_sensitive(calculate)&&!gtk_widget_get_sensitive(copy));
        g_signal_emit_by_name(calculate,"clicked");g_signal_emit_by_name(copy,"clicked");g_object_unref(window);
    }else if(strcmp(test,"retained-button")==0){
        g_object_ref(calculate);g_object_ref(copy);g_object_unref(root);root=NULL;
        g_signal_emit_by_name(calculate,"clicked");g_signal_emit_by_name(copy,"clicked");g_object_unref(calculate);g_object_unref(copy);
    }else return 2;
    g_free(before);if(root!=NULL)g_object_unref(root);return 0;
}
