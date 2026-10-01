/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/bank_audit.inc
 * PURPOSE: Expose explicit audit capture, selected command details and copied journal evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static void AuditFiltersChanged(GtkWidget *widget,gpointer window)
{
    (void)widget;BankUi *ui=SignalUi(window);if(ui==NULL)return;
    gtk_label_set_text(ui->auditHint,"Filters changed. Choose Capture audit. Any displayed report and copy buttons still refer to the previous capture.");
}
static void AuditFilterSelection(GObject *object,GParamSpec *spec,gpointer window)
{(void)spec;AuditFiltersChanged(GTK_WIDGET(object),window);}
static void AuditSelect(GObject *object,GParamSpec *spec,gpointer window)
{
    (void)object;(void)spec;BankUi *ui=SignalUi(window);if(ui==NULL || ui->auditPublishing)return;
    guint at=gtk_drop_down_get_selected(ui->auditRows);size_t required=0;
    UmiStatus status=UmiBankAuditDescribeEvent(ui->auditReport,at,NULL,0,&required);
    char *text=status==UMI_STATUS_OK?g_try_malloc(required):NULL;
    if(status==UMI_STATUS_OK && text==NULL)status=UMI_STATUS_OUT_OF_MEMORY;
    if(status==UMI_STATUS_OK)status=UmiBankAuditDescribeEvent(ui->auditReport,at,text,required,NULL);
    if(status==UMI_STATUS_OK){char *valid=g_utf8_make_valid(text,-1);gtk_text_buffer_set_text(ui->auditDetail,valid,-1);g_free(valid);}
    else gtk_text_buffer_set_text(ui->auditDetail,"Select a captured command to inspect its original payload and exact posting journals.",-1);
    g_free(text);
}
static bool AuditReadQuery(BankUi *ui,UmiBankAuditQuery *query)
{
    memset(query,0,sizeof *query);
    if(!CopyId(ui->auditActor,&query->actorId) || !CopyId(ui->auditEntity,&query->entityId) || !CopyId(ui->auditRequest,&query->requestId))return false;
    query->family=(UmiBankAuditFamily)gtk_drop_down_get_selected(ui->auditFamily);
    query->action=(UmiBankAction)gtk_drop_down_get_selected(ui->auditAction);
    const char *first=EntryText(ui->auditFirst),*last=EntryText(ui->auditLast);
    if((first[0] && !UnsignedText(first,UINT64_MAX,&query->firstRevision)) || (last[0] && !UnsignedText(last,UINT64_MAX,&query->lastRevision)))return false;
    return UmiBankActivityDateParse(EntryText(ui->auditFrom),&query->fromDate)==UMI_STATUS_OK &&
        UmiBankActivityDateParse(EntryText(ui->auditTo),&query->toDate)==UMI_STATUS_OK && UmiBankAuditQueryValidate(query)==UMI_STATUS_OK;
}
static void AuditCaptureClicked(GtkButton *button,gpointer window)
{
    (void)button;BankUi *ui=SignalUi(window);if(ui==NULL)return;
    UmiBankAuditQuery query;UmiBankAuditReport *next=NULL;
    UmiStatus status=AuditReadQuery(ui,&query)?UmiBankAuditCapture(ui->operations,&query,&next):UMI_STATUS_INVALID_ARGUMENT;
    size_t required=0;if(status==UMI_STATUS_OK)status=UmiBankAuditDescribe(next,NULL,0,&required);
    char *text=status==UMI_STATUS_OK?g_try_malloc(required):NULL;
    if(status==UMI_STATUS_OK && text==NULL)status=UMI_STATUS_OUT_OF_MEMORY;
    if(status==UMI_STATUS_OK)status=UmiBankAuditDescribe(next,text,required,NULL);
    if(status!=UMI_STATUS_OK){
        UmiBankAuditDestroy(next);g_free(text);
        gtk_label_set_text(ui->auditHint,"Capture failed. Check exact IDs, dates and revision bounds. The previously displayed capture is retained.");return;
    }
    UmiBankAuditSummary summary;(void)UmiBankAuditReadSummary(next,&summary);
    GtkStringList *rows=gtk_string_list_new(NULL);
    for(size_t i=0;i<summary.count;++i){
        UmiBankAuditRow row;(void)UmiBankAuditRowAt(next,i,&row);
        char *label=g_strdup_printf("%" G_GUINT64_FORMAT " | %s | %s | %s",(guint64)row.event.revision,UmiBankActionName(row.event.command.action),row.event.command.id.value,row.event.command.requestId.value);
        char *valid=g_utf8_make_valid(label,-1);gtk_string_list_append(rows,valid);g_free(valid);g_free(label);
    }
    /* Complete the next owned report before replacing a useful older capture.
     * Native selection signals are blocked from choosing a new row implicitly. */
    UmiBankAuditReport *previous=ui->auditReport;ui->auditReport=next;ui->auditPublishing=true;
    gtk_drop_down_set_model(ui->auditRows,G_LIST_MODEL(rows));gtk_drop_down_set_selected(ui->auditRows,GTK_INVALID_LIST_POSITION);
    g_object_unref(rows);char *valid=g_utf8_make_valid(text,-1);gtk_text_buffer_set_text(ui->auditSummary,valid,-1);g_free(valid);g_free(text);
    gtk_text_buffer_set_text(ui->auditDetail,"Select a captured command to inspect its original payload and exact posting journals.",-1);
    ui->auditPublishing=false;UmiBankAuditDestroy(previous);
    gtk_widget_set_sensitive(ui->auditCopy,TRUE);gtk_widget_set_sensitive(ui->auditJournalCopy,TRUE);
    gtk_label_set_text(ui->auditHint,"Captured accepted local commands. This view stays at its captured revision. Reload committed data first if another window has written changes.");
}
static void AuditCopyClicked(GtkButton *button,gpointer window)
{
    BankUi *ui=SignalUi(window);if(ui==NULL || ui->auditReport==NULL)return;
    UmiCsvDocument *csv=NULL;int journals=GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button),"bank-audit-journals"));
    UmiStatus status=journals?UmiBankAuditExportJournalsCsv(ui->auditReport,&csv):UmiBankAuditExportCsv(ui->auditReport,&csv);
    if(status==UMI_STATUS_OK){gdk_clipboard_set_text(gtk_widget_get_clipboard(GTK_WIDGET(button)),UmiCsvDocumentData(csv));
        gtk_label_set_text(ui->auditHint,journals?"Copied journal lines from the displayed capture, including both debit and credit sides.":"Copied accepted commands from the displayed capture, including its filters and revision.");}
    else gtk_label_set_text(ui->auditHint,"Export failed. The previous clipboard and captured report are unchanged.");
    UmiCsvDocumentDestroy(csv);
}
static GtkEntry *AuditEntry(BankUi *ui,GtkGrid *grid,int row,int column,const char *label,const char *tag)
{
    GtkWidget *box=gtk_box_new(GTK_ORIENTATION_VERTICAL,2),*caption=gtk_label_new(label),*entry=QueueTag(gtk_entry_new(),tag);
    gtk_label_set_xalign(GTK_LABEL(caption),0);gtk_box_append(GTK_BOX(box),caption);gtk_box_append(GTK_BOX(box),entry);
    gtk_widget_set_hexpand(box,TRUE);gtk_grid_attach(grid,box,column,row,1,1);
    gtk_accessible_update_property(GTK_ACCESSIBLE(entry),GTK_ACCESSIBLE_PROPERTY_LABEL,label,-1);
    g_signal_connect_object(entry,"changed",G_CALLBACK(AuditFiltersChanged),G_OBJECT(ui->window),0);return GTK_ENTRY(entry);
}
static GtkTextBuffer *AuditView(GtkWidget *box,const char *tag,int height)
{
    GtkWidget *scroll=gtk_scrolled_window_new(),*view=QueueTag(gtk_text_view_new(),tag);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view),FALSE);gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view),GTK_WRAP_WORD_CHAR);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),view);gtk_widget_set_size_request(scroll,-1,height);
    gtk_widget_set_vexpand(scroll,TRUE);gtk_box_append(GTK_BOX(box),scroll);return gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
}
static void AuditCreate(BankUi *ui)
{
    GtkWidget *page=gtk_box_new(GTK_ORIENTATION_VERTICAL,6),*grid=gtk_grid_new();gtk_grid_set_column_spacing(GTK_GRID(grid),6);gtk_grid_set_row_spacing(GTK_GRID(grid),4);
    ui->auditActor=AuditEntry(ui,GTK_GRID(grid),0,0,"Exact actor ID (optional)","bank.audit.actor");
    ui->auditEntity=AuditEntry(ui,GTK_GRID(grid),0,1,"Exact entity ID (optional)","bank.audit.entity");
    ui->auditRequest=AuditEntry(ui,GTK_GRID(grid),0,2,"Exact request ID (optional)","bank.audit.request");
    ui->auditFirst=AuditEntry(ui,GTK_GRID(grid),1,0,"First revision (blank = any)","bank.audit.first");
    ui->auditLast=AuditEntry(ui,GTK_GRID(grid),1,1,"Last revision (blank = any)","bank.audit.last");
    ui->auditFrom=AuditEntry(ui,GTK_GRID(grid),2,0,"From YYYY-MM-DD (optional)","bank.audit.from");
    ui->auditTo=AuditEntry(ui,GTK_GRID(grid),2,1,"To YYYY-MM-DD (optional)","bank.audit.to");
    gtk_box_append(GTK_BOX(page),grid);
    const char *families[UMI_BANK_AUDIT_RECONCILIATION+2];
    for(unsigned i=0;i<=UMI_BANK_AUDIT_RECONCILIATION;++i)families[i]=UmiBankAuditFamilyName((UmiBankAuditFamily)i);
    families[UMI_BANK_AUDIT_RECONCILIATION+1]=NULL;
    const char *actions[UMI_BANK_ACTION_LAST+2];actions[0]="All actions";
    /* GTK reads this as a terminated string list. Fill each action first,
     * then write the terminator once, outside the loop. New action names
     * remain supplied by the shared action catalogue.
     * The earlier compact spelling is retained for engineering review. */
#if 0
    for(unsigned i=1;i<=UMI_BANK_ACTION_LAST;++i)actions[i]=UmiBankActionName((UmiBankAction)i);actions[UMI_BANK_ACTION_LAST+1]=NULL;
#endif
    for (unsigned i = 1; i <= UMI_BANK_ACTION_LAST; ++i) {
        actions[i] = UmiBankActionName((UmiBankAction)i);
    }
    actions[UMI_BANK_ACTION_LAST + 1] = NULL;
    GtkWidget *filters=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,6);
    ui->auditFamily=GTK_DROP_DOWN(QueueTag(gtk_drop_down_new_from_strings(families),"bank.audit.family"));
    ui->auditAction=GTK_DROP_DOWN(QueueTag(gtk_drop_down_new_from_strings(actions),"bank.audit.action"));
    gtk_accessible_update_property(GTK_ACCESSIBLE(ui->auditFamily),GTK_ACCESSIBLE_PROPERTY_LABEL,"Audit workflow filter",-1);
    gtk_accessible_update_property(GTK_ACCESSIBLE(ui->auditAction),GTK_ACCESSIBLE_PROPERTY_LABEL,"Audit action filter",-1);
    gtk_box_append(GTK_BOX(filters),gtk_label_new("Workflow"));gtk_box_append(GTK_BOX(filters),GTK_WIDGET(ui->auditFamily));
    gtk_box_append(GTK_BOX(filters),gtk_label_new("Action"));gtk_box_append(GTK_BOX(filters),GTK_WIDGET(ui->auditAction));
    GtkWidget *capture=QueueTag(gtk_button_new_with_label("Capture audit"),"bank.audit.capture");gtk_box_append(GTK_BOX(filters),capture);gtk_box_append(GTK_BOX(page),filters);
    ui->auditHint=GTK_LABEL(QueueTag(gtk_label_new("Choose filters and Capture audit. IDs match exactly; entity IDs can occur in different workflows."),"bank.audit.hint"));
    gtk_label_set_wrap(ui->auditHint,TRUE);gtk_label_set_xalign(ui->auditHint,0);gtk_box_append(GTK_BOX(page),GTK_WIDGET(ui->auditHint));
    ui->auditSummary=AuditView(page,"bank.audit.summary",110);
    ui->auditRows=GTK_DROP_DOWN(QueueTag(gtk_drop_down_new(NULL,NULL),"bank.audit.rows"));gtk_box_append(GTK_BOX(page),GTK_WIDGET(ui->auditRows));
    gtk_accessible_update_property(GTK_ACCESSIBLE(ui->auditRows),GTK_ACCESSIBLE_PROPERTY_LABEL,"Captured audit command",-1);
    ui->auditDetail=AuditView(page,"bank.audit.detail",180);
    GtkWidget *copies=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,6);
    ui->auditCopy=QueueTag(gtk_button_new_with_label("Copy captured commands CSV"),"bank.audit.copy");
    ui->auditJournalCopy=QueueTag(gtk_button_new_with_label("Copy captured journal lines CSV"),"bank.audit.copy-journals");
    g_object_set_data(G_OBJECT(ui->auditJournalCopy),"bank-audit-journals",GINT_TO_POINTER(1));
    gtk_box_append(GTK_BOX(copies),ui->auditCopy);gtk_box_append(GTK_BOX(copies),ui->auditJournalCopy);gtk_box_append(GTK_BOX(page),copies);
    gtk_widget_set_sensitive(ui->auditCopy,FALSE);gtk_widget_set_sensitive(ui->auditJournalCopy,FALSE);
    gtk_notebook_append_page(ui->pages,page,gtk_label_new("Audit investigation"));
    g_signal_connect_object(capture,"clicked",G_CALLBACK(AuditCaptureClicked),G_OBJECT(ui->window),0);
    g_signal_connect_object(ui->auditRows,"notify::selected",G_CALLBACK(AuditSelect),G_OBJECT(ui->window),0);
    g_signal_connect_object(ui->auditFamily,"notify::selected",G_CALLBACK(AuditFilterSelection),G_OBJECT(ui->window),0);
    g_signal_connect_object(ui->auditAction,"notify::selected",G_CALLBACK(AuditFilterSelection),G_OBJECT(ui->window),0);
    g_signal_connect_object(ui->auditCopy,"clicked",G_CALLBACK(AuditCopyClicked),G_OBJECT(ui->window),0);
    g_signal_connect_object(ui->auditJournalCopy,"clicked",G_CALLBACK(AuditCopyClicked),G_OBJECT(ui->window),0);
}
