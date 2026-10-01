/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/payment_quote_gtk4.c
 * PURPOSE: Render explicit pricing assumptions using Framework-owned checked quotes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/payment_quote.h"
#include "umicom/finance/payments/payment_quote.h"
#include "umicom/finance/decimal.h"
#include "umicom/ui/gtk4/automation.h"
#include <string.h>

typedef struct QuoteUi {
    UmiPaymentQuote *quote;
    GtkWidget *quoteId, *paymentId, *ruleId, *principal, *currency, *scale;
    GtkWidget *fixed, *rate, *maximum, *tax, *feeRounding, *taxRounding;
    GtkWidget *calculate, *copy, *message;
    GtkTextBuffer *buffer;
    bool detached, edited;
} QuoteUi;
static QuoteUi *State(gpointer root) { return g_object_get_data(G_OBJECT(root),"umicom-payment-quote"); }
static void Free(gpointer data) { QuoteUi *ui=data; UmiPaymentQuoteDestroy(ui->quote); g_free(ui); }
static void Edited(GtkEditable *editable, gpointer root)
{
    (void)editable; QuoteUi *ui=State(root); if(ui==NULL||ui->detached)return;
    ui->edited=true; gtk_widget_set_sensitive(ui->copy,FALSE);
    gtk_label_set_text(GTK_LABEL(ui->message),"Inputs changed. Calculate quote to replace the captured result. Copy is disabled until then.");
}
static void Selected(GObject *object,GParamSpec *property,gpointer root)
{ (void)object; (void)property; Edited(NULL,root); }
static UmiStatus Number(GtkWidget *entry,uint8_t scale,int64_t *out)
{
    const char *text=gtk_editable_get_text(GTK_EDITABLE(entry)); UmiDecimal decimal;
    UmiStatus status=UmiDecimalParse(text,strlen(text),scale,&decimal);
    if(status==UMI_STATUS_OK){ if(decimal.coefficient<0)return UMI_STATUS_INVALID_ARGUMENT; *out=decimal.coefficient; }
    return status;
}
static UmiStatus Inputs(QuoteUi *ui,UmiPaymentQuoteRequest *out)
{
    UmiPaymentQuoteRequest request={0}; int64_t scale,rate,tax,fixed,maximum;
    UmiStatus status=Number(ui->scale,0,&scale);
    if(status!=UMI_STATUS_OK)return status;
    if(scale>9)return UMI_STATUS_INVALID_ARGUMENT;
    request.principal.scale=(uint8_t)scale;
    status=Number(ui->principal,(uint8_t)scale,&request.principal.minor_units);
    if(status==UMI_STATUS_OK)status=Number(ui->fixed,(uint8_t)scale,&fixed);
    if(status==UMI_STATUS_OK)status=Number(ui->maximum,(uint8_t)scale,&maximum);
    if(status==UMI_STATUS_OK)status=Number(ui->rate,0,&rate);
    if(status==UMI_STATUS_OK)status=Number(ui->tax,0,&tax);
    if(status!=UMI_STATUS_OK)return status;
    if(rate>10000||tax>10000)return UMI_STATUS_INVALID_ARGUMENT;
    status=umi_payments_id_assign(&request.quoteId,gtk_editable_get_text(GTK_EDITABLE(ui->quoteId)));
    if(status==UMI_STATUS_OK)status=umi_payments_id_assign(&request.paymentId,gtk_editable_get_text(GTK_EDITABLE(ui->paymentId)));
    if(status==UMI_STATUS_OK)status=umi_payments_currency_from_code(gtk_editable_get_text(GTK_EDITABLE(ui->currency)),&request.principal.currency);
    if(status==UMI_STATUS_OK)status=umi_payments_payment_fee_rule_init(&request.rule,gtk_editable_get_text(GTK_EDITABLE(ui->ruleId)),fixed,(uint32_t)rate,maximum);
    if(status!=UMI_STATUS_OK)return status;
    request.feeRounding=(UmiMoneyRounding)gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->feeRounding));
    request.taxRounding=(UmiMoneyRounding)gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->taxRounding));
    request.taxBasisPoints=(uint32_t)tax; *out=request; return UMI_STATUS_OK;
}
static void Failure(QuoteUi *ui,UmiStatus status)
{
    char message[256];g_snprintf(message,sizeof(message),"Quote not replaced: %s. Check amounts, cap, rates, currency and scale. No payment was changed.",umi_status_text(status));
    gtk_label_set_text(GTK_LABEL(ui->message),message);
}
static void Calculate(GtkButton *button,gpointer root)
{
    (void)button;QuoteUi *ui=State(root);if(ui==NULL||ui->detached)return;
    UmiPaymentQuoteRequest request; UmiPaymentQuote *quote=NULL; char text[UMI_PAYMENT_QUOTE_TEXT_CAPACITY];
    UmiStatus status=Inputs(ui,&request);
    if(status==UMI_STATUS_OK)status=UmiPaymentQuoteCreate(&request,&quote);
    if(status==UMI_STATUS_OK)status=UmiPaymentQuoteDescribe(quote,text,sizeof(text),NULL);
    if(status==UMI_STATUS_OK&&!g_utf8_validate(text,-1,NULL))status=UMI_STATUS_INVALID_ARGUMENT;
    if(status!=UMI_STATUS_OK){UmiPaymentQuoteDestroy(quote);Failure(ui,status);return;}
    UmiPaymentQuoteDestroy(ui->quote);ui->quote=quote;ui->edited=false;
    gtk_text_buffer_set_text(ui->buffer,text,-1);gtk_widget_set_sensitive(ui->copy,TRUE);
    gtk_label_set_text(GTK_LABEL(ui->message),"Captured scenario. Copy CSV exports these assumptions and amounts. This is not a payment instruction.");
}
static void Copy(GtkButton *button,gpointer root)
{
    QuoteUi *ui=State(root);if(ui==NULL||ui->detached||ui->edited||ui->quote==NULL)return;
    UmiCsvDocument *csv=NULL;UmiStatus status=UmiPaymentQuoteExportCsv(ui->quote,&csv);
    if(status!=UMI_STATUS_OK){Failure(ui,status);return;}
    gdk_clipboard_set_text(gtk_widget_get_clipboard(GTK_WIDGET(button)),UmiCsvDocumentData(csv));
    UmiCsvDocumentDestroy(csv);
    gtk_label_set_text(GTK_LABEL(ui->message),"Copied the captured scenario, exact minor units and rounding assumptions. No payment was submitted.");
}
static GtkWidget *Entry(GtkGrid *grid,int row,const char *label,const char *value,const char *tag)
{
    GtkWidget *entry=gtk_entry_new();gtk_entry_set_max_length(GTK_ENTRY(entry),95);
    gtk_editable_set_text(GTK_EDITABLE(entry),value);gtk_widget_set_hexpand(entry,TRUE);
    GtkWidget *name=gtk_label_new(label);gtk_label_set_xalign(GTK_LABEL(name),0);
    gtk_grid_attach(grid,name,0,row,1,1);gtk_grid_attach(grid,entry,1,row,1,1);
    gtk_accessible_update_property(GTK_ACCESSIBLE(entry),GTK_ACCESSIBLE_PROPERTY_LABEL,label,-1);
    (void)umi_gtk4_automation_tag_widget(entry,tag);return entry;
}
GtkWidget *UmiGtk4PaymentQuoteCreate(void)
{
    GtkWidget *root=gtk_box_new(GTK_ORIENTATION_VERTICAL,6);QuoteUi *ui=g_new0(QuoteUi,1);
    g_object_set_data_full(G_OBJECT(root),"umicom-payment-quote",ui,Free);
    (void)umi_gtk4_automation_tag_widget(root,"payment.quote");
    GtkWidget *intro=gtk_label_new("Enter a fee scenario. Tax applies only to the capped fee. Rates are assumptions you choose; no tariff or tax rule is inferred. This calculator cannot submit, reserve or post a payment.");
    gtk_label_set_wrap(GTK_LABEL(intro),TRUE);gtk_box_append(GTK_BOX(root),intro);
    GtkGrid *grid=GTK_GRID(gtk_grid_new());gtk_grid_set_row_spacing(grid,4);gtk_grid_set_column_spacing(grid,8);
    ui->quoteId=Entry(grid,0,"Quote reference","scenario-1","payment.quote.id");
    ui->paymentId=Entry(grid,1,"Payment reference","payment-1","payment.quote.payment");
    ui->ruleId=Entry(grid,2,"Rule reference","illustration","payment.quote.rule");
    ui->currency=Entry(grid,3,"Currency (three uppercase letters)","GBP","payment.quote.currency");
    ui->scale=Entry(grid,4,"Decimal places (0 to 9)","2","payment.quote.scale");
    ui->principal=Entry(grid,5,"Principal amount","100.00","payment.quote.principal");
    ui->fixed=Entry(grid,6,"Fixed fee amount","0.10","payment.quote.fixed");
    ui->rate=Entry(grid,7,"Variable fee (basis points)","25","payment.quote.rate");
    ui->maximum=Entry(grid,8,"Maximum fee amount (before tax)","10.00","payment.quote.maximum");
    ui->tax=Entry(grid,9,"Tax on fee (basis points)","0","payment.quote.tax");
    const char *roundings[]={"Toward zero","Nearest, ties away from zero","Nearest, ties to even","Away from zero",NULL};
    ui->feeRounding=gtk_drop_down_new_from_strings(roundings);ui->taxRounding=gtk_drop_down_new_from_strings(roundings);
    gtk_accessible_update_property(GTK_ACCESSIBLE(ui->feeRounding),GTK_ACCESSIBLE_PROPERTY_LABEL,"Fee rounding",-1);
    gtk_accessible_update_property(GTK_ACCESSIBLE(ui->taxRounding),GTK_ACCESSIBLE_PROPERTY_LABEL,"Tax rounding",-1);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(ui->feeRounding),UMI_MONEY_HALF_EVEN);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(ui->taxRounding),UMI_MONEY_HALF_EVEN);
    gtk_grid_attach(grid,gtk_label_new("Fee rounding"),0,10,1,1);gtk_grid_attach(grid,ui->feeRounding,1,10,1,1);
    gtk_grid_attach(grid,gtk_label_new("Tax rounding"),0,11,1,1);gtk_grid_attach(grid,ui->taxRounding,1,11,1,1);
    (void)umi_gtk4_automation_tag_widget(ui->feeRounding,"payment.quote.fee-rounding");
    (void)umi_gtk4_automation_tag_widget(ui->taxRounding,"payment.quote.tax-rounding");
    GtkWidget *inputScroll=gtk_scrolled_window_new();gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(inputScroll),GTK_WIDGET(grid));
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(inputScroll),190);gtk_box_append(GTK_BOX(root),inputScroll);
    GtkWidget *row=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,6);ui->calculate=gtk_button_new_with_label("Calculate quote");ui->copy=gtk_button_new_with_label("Copy captured CSV");
    gtk_box_append(GTK_BOX(row),ui->calculate);gtk_box_append(GTK_BOX(row),ui->copy);gtk_box_append(GTK_BOX(root),row);
    ui->message=gtk_label_new("Choose your assumptions, then calculate a quote.");gtk_label_set_wrap(GTK_LABEL(ui->message),TRUE);gtk_box_append(GTK_BOX(root),ui->message);
    GtkWidget *view=gtk_text_view_new();ui->buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view),FALSE);gtk_text_view_set_monospace(GTK_TEXT_VIEW(view),TRUE);gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view),GTK_WRAP_WORD_CHAR);
    GtkWidget *scroll=gtk_scrolled_window_new();gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),view);gtk_widget_set_vexpand(scroll,TRUE);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll),180);gtk_box_append(GTK_BOX(root),scroll);
    (void)umi_gtk4_automation_tag_widget(ui->calculate,"payment.quote.calculate");(void)umi_gtk4_automation_tag_widget(ui->copy,"payment.quote.copy");
    (void)umi_gtk4_automation_tag_widget(view,"payment.quote.result");(void)umi_gtk4_automation_tag_widget(ui->message,"payment.quote.message");
    GtkWidget *entries[]={ui->quoteId,ui->paymentId,ui->ruleId,ui->principal,ui->currency,ui->scale,ui->fixed,ui->rate,ui->maximum,ui->tax};
    for(size_t i=0;i<G_N_ELEMENTS(entries);++i)g_signal_connect_object(entries[i],"changed",G_CALLBACK(Edited),G_OBJECT(root),0);
    g_signal_connect_object(ui->feeRounding,"notify::selected",G_CALLBACK(Selected),G_OBJECT(root),0);
    g_signal_connect_object(ui->taxRounding,"notify::selected",G_CALLBACK(Selected),G_OBJECT(root),0);
    g_signal_connect_object(ui->calculate,"clicked",G_CALLBACK(Calculate),G_OBJECT(root),0);
    g_signal_connect_object(ui->copy,"clicked",G_CALLBACK(Copy),G_OBJECT(root),0);
    gtk_widget_set_sensitive(ui->copy,FALSE);return root;
}
void UmiGtk4PaymentQuoteDetach(GtkWidget *calculator)
{
    if(calculator==NULL)return;
    QuoteUi *ui=State(calculator);if(ui==NULL||ui->detached)return;
    ui->detached=true;gtk_widget_set_sensitive(ui->calculate,FALSE);gtk_widget_set_sensitive(ui->copy,FALSE);
    gtk_label_set_text(GTK_LABEL(ui->message),"Calculator closed. Captured results remain readable; actions are disabled.");
}
