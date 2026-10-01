/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/bank_work_queue.inc
 * PURPOSE: Compose a copied request queue with the existing banking form and reviewed submission.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Included after the form helpers. Window-bound signals and SignalUi make
 * retained controls inert after close; the queue owns no service reference. */
static void QueueClear(BankUi *ui,const char *message)
{
    UmiBankWorkQueueDestroy(ui->workQueue);ui->workQueue=NULL;
    InvalidateReview(ui);
    gtk_drop_down_set_model(ui->queueRows,NULL);
    gtk_text_buffer_set_text(ui->queueDetails,"No captured request selected.",-1);
    gtk_label_set_text(ui->queueSummary,message);
    gtk_widget_set_sensitive(ui->queueReviewButton,FALSE);gtk_widget_set_sensitive(ui->queueCopyButton,FALSE);
}
static void QueueSelectionChanged(GObject *object,GParamSpec *spec,gpointer window)
{
    (void)object;(void)spec;BankUi *ui=SignalUi(window);if(ui==NULL)return;
    InvalidateReview(ui);UmiBankWorkQueueRow row;UmiBankAction action;
    guint index=gtk_drop_down_get_selected(ui->queueRows),decision=gtk_drop_down_get_selected(ui->queueDecision);
    UmiStatus status=UmiBankWorkQueueRowAt(ui->workQueue,index,&row);
    gtk_widget_set_sensitive(ui->queueReviewButton,status==UMI_STATUS_OK&&decision<4U&&
        UmiBankWorkQueueResolveAction(ui->workQueue,index,(UmiBankWorkDecision)(decision+1U),&action)==UMI_STATUS_OK);
    if(status!=UMI_STATUS_OK){gtk_text_buffer_set_text(ui->queueDetails,"No captured request selected.",-1);return;}
    char money[UMI_MONEY_TEXT_CAPACITY];
    status=UmiMoneyTextFormat(&row.amount,money,sizeof(money),NULL);
    if(status!=UMI_STATUS_OK){gtk_text_buffer_set_text(ui->queueDetails,"Amount could not be displayed.",-1);return;}
    GString *text=g_string_new("LOCAL PRACTICE - captured request, no action committed.\n");
    g_string_append_printf(text,"Type: %s\nID: %s\nState: %s\nFrom/account: %s\nTo: %s\nReference: %s\nAmount: %s\nMaker: %s\nChecker: %s\nSubmitted revision: %" PRIu64 "\n",
        UmiBankWorkKindName(row.kind),row.id.value,TransferState(row.state),row.sourceAccountId.value,row.destinationAccountId.value,
        row.referenceId.value,money,row.makerId.value,row.checkerId.value,row.submittedRevision);
    if(row.kind==UMI_BANK_WORK_CHARGE)g_string_append_printf(text,"Reason: %s\n",row.reason);
    if(row.kind==UMI_BANK_WORK_INTEREST){
        char principal[UMI_MONEY_TEXT_CAPACITY];
        if(UmiMoneyTextFormat(&row.principal,principal,sizeof(principal),NULL)==UMI_STATUS_OK)
            g_string_append_printf(text,"Fixed principal: %s\nAnnual rate: %" PRId32 " bps; days: %" PRIu32 "; basis: %" PRIu32 "\n",
                principal,row.interest.annualRateBps,row.interest.days,row.interest.dayCountBasis);
    }
    char *valid=g_utf8_make_valid(text->str,-1);gtk_text_buffer_set_text(ui->queueDetails,valid,-1);g_free(valid);g_string_free(text,TRUE);
}
static void QueueRefresh(BankUi *ui)
{
    if(ui->queueRows==NULL||ui->closed)return;
    UmiBankWorkQueueRow prior={0};
    (void)UmiBankWorkQueueRowAt(ui->workQueue,gtk_drop_down_get_selected(ui->queueRows),&prior);
    UmiBankWorkQueueFilter filter=UmiBankWorkQueueFilterAll();
    guint kind=gtk_drop_down_get_selected(ui->queueKinds),state=gtk_drop_down_get_selected(ui->queueStates);
    if(kind>3U||state>2U||!CopyId(ui->queueAccount,&filter.accountId)){
        QueueClear(ui,"Invalid queue filter. Enter an exact account ID or leave it blank.");return;
    }
    if(kind!=0)filter.kinds=UINT32_C(1)<<(kind-1U);
    if(state!=0)filter.states=UINT32_C(1)<<(state-1U);
    UmiBankWorkQueue *next=NULL;UmiStatus status=UmiBankWorkQueueCapture(ui->operations,&filter,&next);
    QueueClear(ui,status==UMI_STATUS_OK?"Captured queue.":StatusText(status));
    if(status!=UMI_STATUS_OK)return;
    ui->workQueue=next;UmiBankWorkQueueSummary summary; (void)UmiBankWorkQueueSummaryRead(next,&summary);
    GtkStringList *rows=gtk_string_list_new(NULL);guint selected=GTK_INVALID_LIST_POSITION;
    for(size_t i=0;i<summary.count;++i){
        UmiBankWorkQueueRow row;(void)UmiBankWorkQueueRowAt(next,i,&row);
        char money[UMI_MONEY_TEXT_CAPACITY];status=UmiMoneyTextFormat(&row.amount,money,sizeof(money),NULL);
        if(status!=UMI_STATUS_OK){g_object_unref(rows);QueueClear(ui,StatusText(status));return;}
        char *label=g_strdup_printf("%s | %s | %s | %s",UmiBankWorkKindName(row.kind),row.id.value,TransferState(row.state),money);
        char *valid=g_utf8_make_valid(label,-1);gtk_string_list_append(rows,valid);g_free(valid);g_free(label);
        if(row.kind==prior.kind&&strcmp(row.id.value,prior.id.value)==0)selected=(guint)i;
    }
    gtk_drop_down_set_model(ui->queueRows,G_LIST_MODEL(rows));g_object_unref(rows);
    /* Never choose another record just because a selected request disappeared. */
    gtk_drop_down_set_selected(ui->queueRows,selected);
    char label[220];(void)snprintf(label,sizeof(label),"Captured revision %" PRIu64 " | %zu shown of %zu open | %zu pending, %zu approved | %s",
        summary.revision,summary.count,summary.totalOpen,summary.pending,summary.approved,summary.durable?"local disk":"memory only");
    gtk_label_set_text(ui->queueSummary,label);gtk_widget_set_sensitive(ui->queueCopyButton,TRUE);
    QueueSelectionChanged(NULL,NULL,ui->window);
}
static void QueueFilterChanged(GtkEditable *editable,gpointer window)
{
    (void)editable;BankUi *ui=SignalUi(window);if(ui!=NULL)QueueClear(ui,"Filter changed. Choose Refresh queue.");
}
static void QueueFilterSelected(GObject *object,GParamSpec *spec,gpointer window)
{ (void)object;(void)spec;QueueFilterChanged(NULL,window); }
static void QueueRefreshClicked(GtkButton *button,gpointer window)
{
    (void)button;BankUi *ui=SignalUi(window);if(ui==NULL)return;
    InvalidateReview(ui);Refresh(ui);
}
static void QueueReviewClicked(GtkButton *button,gpointer window)
{
    (void)button;BankUi *ui=SignalUi(window);if(ui==NULL)return;
    InvalidateReview(ui);guint index=gtk_drop_down_get_selected(ui->queueRows);
    guint decision=gtk_drop_down_get_selected(ui->queueDecision);UmiBankWorkQueueRow row;UmiBankAction action;
    UmiStatus status=UmiBankWorkQueueRowAt(ui->workQueue,index,&row);
    if(status==UMI_STATUS_OK)status=UmiBankWorkQueueResolveAction(ui->workQueue,index,(UmiBankWorkDecision)(decision+1U),&action);
    if(status!=UMI_STATUS_OK){Message(ui,status);return;}
    /* Fill only the chosen action/entity. The user-selected actor is retained.
     * New request supplies a distinct id and date, never permission to execute. */
    gtk_drop_down_set_selected(ui->action,(guint)action-1U);ActionChanged(NULL,NULL,window);
    gtk_editable_set_text(GTK_EDITABLE(ui->id),row.id.value);NewRequest(NULL,window);
    UmiBankActor actor;UmiBankCommand command;
    if(!ReadCommand(ui,&actor,&command)){Message(ui,UMI_STATUS_INVALID_ARGUMENT);return;}
    status=UmiBankWorkQueueReview(ui->operations,ui->workQueue,index,&actor,&command,&ui->review);
    if(status==UMI_STATUS_OK)DisplayReview(ui);else Message(ui,status);
}
static void QueueCopyClicked(GtkButton *button,gpointer window)
{
    BankUi *ui=SignalUi(window);if(ui==NULL)return;
    UmiCsvDocument *document=NULL;UmiStatus status=UmiBankWorkQueueExportCsv(ui->workQueue,&document);
    if(status==UMI_STATUS_OK){
        gdk_clipboard_set_text(gtk_widget_get_clipboard(GTK_WIDGET(button)),UmiCsvDocumentData(document));
        gtk_label_set_text(ui->message,"Copied the captured queue and its filters. No data was reloaded or committed.");
    }else Message(ui,status);
    UmiCsvDocumentDestroy(document);
}
static GtkWidget *QueueTag(GtkWidget *widget,const char *id)
{
    gtk_widget_set_name(widget,id);
    g_object_set_data_full(G_OBJECT(widget),"umicom-automation-id",g_strdup(id),g_free);return widget;
}
static void QueueCreate(BankUi *ui)
{
    GtkWidget *page=gtk_box_new(GTK_ORIENTATION_VERTICAL,6),*filters=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,6);
    gtk_notebook_append_page(ui->pages,page,gtk_label_new("Review queue"));gtk_box_append(GTK_BOX(page),filters);
    const char *kinds[]={"All types","Transfers","Interest","Charges",NULL};
    const char *states[]={"Pending and approved","Pending","Approved",NULL};
    const char *decisions[]={"Approve","Reject","Cancel","Post / Execute",NULL};
    ui->queueKinds=GTK_DROP_DOWN(QueueTag(gtk_drop_down_new_from_strings(kinds),"bank.queue.kind"));
    ui->queueStates=GTK_DROP_DOWN(QueueTag(gtk_drop_down_new_from_strings(states),"bank.queue.state"));
    ui->queueAccount=GTK_ENTRY(QueueTag(gtk_entry_new(),"bank.queue.account"));
    gtk_entry_set_placeholder_text(ui->queueAccount,"Exact account ID, blank for all");
    gtk_box_append(GTK_BOX(filters),GTK_WIDGET(ui->queueKinds));gtk_box_append(GTK_BOX(filters),GTK_WIDGET(ui->queueStates));
    gtk_box_append(GTK_BOX(filters),GTK_WIDGET(ui->queueAccount));
    GtkWidget *refresh=QueueTag(gtk_button_new_with_label("Refresh queue"),"bank.queue.refresh");gtk_box_append(GTK_BOX(page),refresh);
    ui->queueSummary=GTK_LABEL(gtk_label_new("Choose Refresh queue."));gtk_label_set_wrap(ui->queueSummary,TRUE);gtk_box_append(GTK_BOX(page),GTK_WIDGET(ui->queueSummary));
    ui->queueRows=GTK_DROP_DOWN(QueueTag(gtk_drop_down_new(NULL,NULL),"bank.queue.rows"));gtk_box_append(GTK_BOX(page),GTK_WIDGET(ui->queueRows));
    ui->queueDecision=GTK_DROP_DOWN(QueueTag(gtk_drop_down_new_from_strings(decisions),"bank.queue.decision"));gtk_box_append(GTK_BOX(page),GTK_WIDGET(ui->queueDecision));
    GtkWidget *scroll=gtk_scrolled_window_new(),*view=gtk_text_view_new();ui->queueDetails=gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view),FALSE);gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view),GTK_WRAP_WORD_CHAR);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),view);gtk_widget_set_vexpand(scroll,TRUE);gtk_box_append(GTK_BOX(page),scroll);
    ui->queueReviewButton=QueueTag(gtk_button_new_with_label("Review selected request"),"bank.queue.review");
    ui->queueCopyButton=QueueTag(gtk_button_new_with_label("Copy captured queue CSV"),"bank.queue.copy-csv");
    gtk_box_append(GTK_BOX(page),ui->queueReviewButton);gtk_box_append(GTK_BOX(page),ui->queueCopyButton);
    g_signal_connect_object(refresh,"clicked",G_CALLBACK(QueueRefreshClicked),G_OBJECT(ui->window),0);
    g_signal_connect_object(ui->queueReviewButton,"clicked",G_CALLBACK(QueueReviewClicked),G_OBJECT(ui->window),0);
    g_signal_connect_object(ui->queueCopyButton,"clicked",G_CALLBACK(QueueCopyClicked),G_OBJECT(ui->window),0);
    g_signal_connect_object(ui->queueRows,"notify::selected",G_CALLBACK(QueueSelectionChanged),G_OBJECT(ui->window),0);
    g_signal_connect_object(ui->queueDecision,"notify::selected",G_CALLBACK(QueueSelectionChanged),G_OBJECT(ui->window),0);
    g_signal_connect_object(ui->queueKinds,"notify::selected",G_CALLBACK(QueueFilterSelected),G_OBJECT(ui->window),0);
    g_signal_connect_object(ui->queueStates,"notify::selected",G_CALLBACK(QueueFilterSelected),G_OBJECT(ui->window),0);
    g_signal_connect_object(ui->queueAccount,"changed",G_CALLBACK(QueueFilterChanged),G_OBJECT(ui->window),0);
    QueueClear(ui,"Choose Refresh queue. Reload committed data first to read another window's changes.");
}
