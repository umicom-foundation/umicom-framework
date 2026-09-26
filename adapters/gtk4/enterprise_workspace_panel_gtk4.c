/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/enterprise_workspace_panel_gtk4.c
 * PURPOSE: Present five enterprise tasks through one Framework-owned reference-data workspace.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/enterprise_workspace.h"
#include "umicom/enterprise_workspace/practice.h"
#include <glib/gstdio.h>
#include <inttypes.h>
#include <string.h>

/* Presentation Slave Controller: only forms and copied projections live here.
 * Canonical policy, review rules, CSV mapping and transactions stay in C core. */
struct UmiEnterpriseWorkspaceGtkPanel {
    GtkWidget *root, *open;
    GtkLabel *message, *summary, *storage;
    GtkDropDown *actor, *recipe;
    GtkEntry *dataset, *name, *job, *reason, *capability, *resource;
    GtkTextBuffer *csv, *dataReport, *previewReport, *jobsReport, *auditReport, *recipeReport;
    UmiEnterpriseWorkspace *workspace;
    UmiDataServer *ownedData;
    UmiEnterprisePracticeAccess *ownedAccess;
    char loadedJob[UMI_ENTERPRISE_ID_CAPACITY];
    uint64_t loadedRevision;
    bool ownsWorkspace;
};
static const char *StatusText(UmiStatus status)
{
    switch(status) {
    case UMI_STATUS_OK: return "Completed. Review the updated results below.";
    case UMI_STATUS_PERMISSION_DENIED: return "Not permitted. Check the practice role, recipe setting and execution pause. An author cannot approve their own job.";
    case UMI_STATUS_BUSY: return "Saved data changed. Reload, inspect the job and prepare a new job ID when its target dataset changed.";
    case UMI_STATUS_ALREADY_EXISTS: return "This ID already belongs to different saved data. Use a new ID; existing work was not replaced.";
    case UMI_STATUS_INVALID_ARGUMENT: return "Check the fields. IDs use letters, digits, dots, underscores or hyphens; names and review reasons must not be empty.";
    case UMI_STATUS_INVALID_STATE: return "This job cannot perform that action in its current state. Review the saved job, not just the editable form.";
    case UMI_STATUS_NOT_FOUND: return "The requested dataset, recipe or job does not exist. Check its exact ID.";
    case UMI_STATUS_CAPACITY_EXCEEDED: return "A workspace, input or storage limit was reached. No partial import was committed.";
    case UMI_STATUS_PARSE_ERROR: return "Input or saved data failed validation. Check the CSV details; do not overwrite a damaged database.";
    case UMI_STATUS_UNAVAILABLE: return "SQLite storage is not available in this build. No memory-only substitute was opened.";
    default: return "Storage or processing failed. No success is being reported. Keep the database and inspect the error before retrying.";
    }
}
static void Tag(GtkWidget *widget,const char *id)
{
    g_object_set_data_full(G_OBJECT(widget),"umicom-automation-id",g_strdup(id),g_free);
}
static void Say(UmiEnterpriseWorkspaceGtkPanel *panel,UmiStatus status)
{
    gtk_label_set_text(panel->message,StatusText(status));
}
static bool Ready(UmiEnterpriseWorkspaceGtkPanel *panel)
{
    if(panel->workspace!=NULL)return true;
    gtk_label_set_text(panel->message,"Open practice storage first. No data has been opened or imported yet.");return false;
}
static UmiEnterpriseActor Actor(UmiEnterpriseWorkspaceGtkPanel *panel)
{
    return UmiEnterprisePracticeActor((size_t)gtk_drop_down_get_selected(panel->actor));
}
static const char *Recipe(UmiEnterpriseWorkspaceGtkPanel *panel)
{
    return gtk_drop_down_get_selected(panel->recipe)==1U ? "stock.csv.upper" : "stock.csv";
}
static void SetReport(GtkTextBuffer *buffer,GString *text)
{
    /* User data is plain text, never interpreted as GTK markup or commands. */
    gtk_text_buffer_set_text(buffer,text->str,-1);g_string_free(text,TRUE);
}
static void ShowPreview(GtkTextBuffer *buffer,const UmiEnterprisePreview *preview)
{
    GString *text=g_string_new(NULL);
    g_string_append_printf(text,"Dataset: %s | generation %" PRIu64 " | recipe: %s\n"
        "%zu rows: %zu insert, %zu update, %zu unchanged\n\n",
        preview->datasetId,preview->datasetGeneration,preview->recipeId,
        preview->rowCount,preview->insertCount,preview->updateCount,preview->unchangedCount);
    for(size_t i=0U;i<preview->rowCount;++i){
        const UmiEnterpriseChange *change=&preview->changes[i];
        g_string_append_printf(text,"%s [%s]\n",change->after.id,change->kind==UMI_ENTERPRISE_INSERT?"INSERT":change->kind==UMI_ENTERPRISE_UPDATE?"UPDATE":"UNCHANGED");
        if(change->kind!=UMI_ENTERPRISE_INSERT)g_string_append_printf(text,"Before: %s | quantity %" PRIu64 " | source job %s\n",change->before.label,change->before.quantity,change->before.sourceJob);
        g_string_append_printf(text,"After: %s | quantity %" PRIu64 "\n\n",change->after.label,change->after.quantity);
    }
    SetReport(buffer,text);
}
static void Refresh(UmiEnterpriseWorkspaceGtkPanel *panel)
{
    UmiEnterpriseSnapshot snapshot;UmiStatus status;
    if(!Ready(panel))return;
    status=UmiEnterpriseWorkspaceSnapshot(panel->workspace,&snapshot);
    if(status!=UMI_STATUS_OK){Say(panel,status);return;}
    char *summary=g_strdup_printf("Cached revision %" PRIu64 " | %zu datasets | %zu rows | %zu jobs | execution %s\n"
        "Practice roles are not logins. Reload after another application changes this workspace.",snapshot.revision,snapshot.datasetCount,snapshot.rowCount,snapshot.jobCount,snapshot.paused?"PAUSED":"enabled");
    gtk_label_set_text(panel->summary,summary);g_free(summary);
    GString *text=g_string_new(NULL);
    for(size_t i=0U;i<snapshot.datasetCount;++i){
        UmiEnterpriseDataset dataset;
        status=UmiEnterpriseWorkspaceDatasetAt(panel->workspace,i,&dataset);if(status!=UMI_STATUS_OK)break;
        g_string_append_printf(text,"%s — %s | generation %" PRIu64 " | %zu rows\n",dataset.id,dataset.name,dataset.generation,dataset.rowCount);
        for(size_t j=0U;j<dataset.rowCount;++j){UmiEnterpriseRow row;status=UmiEnterpriseWorkspaceRowAt(panel->workspace,dataset.id,j,&row);if(status!=UMI_STATUS_OK)break;
            g_string_append_printf(text,"  %s | %s | quantity %" PRIu64 " | source job %s\n",row.id,row.label,row.quantity,row.sourceJob);}
        g_string_append(text,"\n");if(status!=UMI_STATUS_OK)break;
    }
    if(status!=UMI_STATUS_OK)g_string_append(text,"A dataset could not be read completely.\n");
    SetReport(panel->dataReport,text);text=g_string_new(NULL);
    for(size_t i=0U;i<snapshot.jobCount;++i){UmiEnterpriseJob job;status=UmiEnterpriseWorkspaceJobAt(panel->workspace,i,&job);if(status!=UMI_STATUS_OK)break;
        g_string_append_printf(text,"%s | %s | dataset %s\nAuthor: %s | latest decision by: %s | executor: %s\nReason: %s\nRows: %zu | prepared revision %" PRIu64 " | applied revision %" PRIu64 "\n\n",
            job.id,UmiEnterpriseJobStateText(job.state),job.datasetId,job.author,job.reviewer,job.executor,job.reason,job.rowCount,job.preparedRevision,job.appliedRevision);}
    SetReport(panel->jobsReport,text);text=g_string_new(NULL);
    for(size_t i=0U;i<snapshot.auditCount;++i){UmiEnterpriseAuditEntry entry;status=UmiEnterpriseWorkspaceAuditAt(panel->workspace,i,&entry);if(status!=UMI_STATUS_OK)break;
        g_string_append_printf(text,"%" PRIu64 " | %s | %s | %s\n%s\n\n",entry.revision,entry.principal,entry.action,entry.target,entry.detail);}
    SetReport(panel->auditReport,text);text=g_string_new(NULL);
    for(size_t i=0U;i<UMI_ENTERPRISE_RECIPE_COUNT;++i){UmiEnterpriseRecipe recipe;status=UmiEnterpriseWorkspaceRecipeAt(panel->workspace,i,&recipe);if(status!=UMI_STATUS_OK)break;
        g_string_append_printf(text,"%s — %s [%s]\n%s\nContract %u | built into Framework; no downloadable code\n\n",recipe.id,recipe.name,recipe.enabled?"enabled":"disabled",recipe.description,recipe.contractVersion);}
    SetReport(panel->recipeReport,text);
    if(status!=UMI_STATUS_OK)Say(panel,status);
}
static void ForgetReview(UmiEnterpriseWorkspaceGtkPanel *panel)
{
    panel->loadedJob[0]='\0';panel->loadedRevision=0U;
}
static void Changed(GObject *object,GParamSpec *property,gpointer data)
{
    (void)object;(void)property;ForgetReview(data);
}
static void Reload(GtkButton *button,gpointer data)
{
    UmiEnterpriseWorkspaceGtkPanel *panel=data;(void)button;if(!Ready(panel))return;
    UmiStatus status=UmiEnterpriseWorkspaceReload(panel->workspace);ForgetReview(panel);Say(panel,status);
    if(status==UMI_STATUS_OK)Refresh(panel);
}
static void CreateDataset(GtkButton *button,gpointer data)
{
    UmiEnterpriseWorkspaceGtkPanel *panel=data;(void)button;if(!Ready(panel))return;
    UmiStatus status=UmiEnterpriseWorkspaceCreateDataset(panel->workspace,Actor(panel),gtk_editable_get_text(GTK_EDITABLE(panel->dataset)),gtk_editable_get_text(GTK_EDITABLE(panel->name)));
    ForgetReview(panel);Say(panel,status);Refresh(panel);
}
static void PreviewOrPrepare(GtkButton *button,gpointer data)
{
    UmiEnterpriseWorkspaceGtkPanel *panel=data;GtkTextIter start,end;
    UmiEnterprisePreview *preview;UmiEnterpriseIssue issue={0};UmiStatus status;
    if(!Ready(panel))return;
    bool prepare=GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button),"prepare"))!=0;
    gtk_text_buffer_get_bounds(panel->csv,&start,&end);char *csv=gtk_text_buffer_get_text(panel->csv,&start,&end,TRUE);
    const char *dataset=gtk_editable_get_text(GTK_EDITABLE(panel->dataset));
    preview=g_new0(UmiEnterprisePreview,1U);
    if(prepare){const char *id=gtk_editable_get_text(GTK_EDITABLE(panel->job));
        status=UmiEnterpriseWorkspacePrepare(panel->workspace,Actor(panel),id,dataset,Recipe(panel),csv,strlen(csv),&issue);
        if(status==UMI_STATUS_OK)status=UmiEnterpriseWorkspaceJobPreview(panel->workspace,id,preview);
    }else status=UmiEnterpriseWorkspacePreview(panel->workspace,dataset,Recipe(panel),csv,strlen(csv),preview,&issue);
    ForgetReview(panel);Say(panel,status);
    if(status==UMI_STATUS_OK)ShowPreview(panel->previewReport,preview);
    else if(issue.message[0]!='\0'){char *message=g_strdup_printf("CSV record %zu, field %zu: %s",issue.record,issue.column,issue.message);gtk_label_set_text(panel->message,message);g_free(message);}
    g_free(preview);g_free(csv);if(prepare)Refresh(panel);
}
static void LoadJob(GtkButton *button,gpointer data)
{
    UmiEnterpriseWorkspaceGtkPanel *panel=data;(void)button;if(!Ready(panel))return;
    const char *id=gtk_editable_get_text(GTK_EDITABLE(panel->job));UmiEnterpriseSnapshot snapshot;
    UmiEnterprisePreview *preview=g_new0(UmiEnterprisePreview,1U);
    UmiStatus status=UmiEnterpriseWorkspaceJobPreview(panel->workspace,id,preview);
    if(status==UMI_STATUS_OK)status=UmiEnterpriseWorkspaceSnapshot(panel->workspace,&snapshot);
    ForgetReview(panel);Say(panel,status);
    if(status==UMI_STATUS_OK){g_strlcpy(panel->loadedJob,id,sizeof(panel->loadedJob));panel->loadedRevision=snapshot.revision;ShowPreview(panel->previewReport,preview);
        char *message=g_strdup_printf("Loaded frozen job %s at revision %" PRIu64 ". Inspect its before/after rows on the Import tab before approving or applying it.",id,snapshot.revision);gtk_label_set_text(panel->message,message);g_free(message);}
    g_free(preview);
}
static void JobAction(GtkButton *button,gpointer data)
{
    UmiEnterpriseWorkspaceGtkPanel *panel=data;if(!Ready(panel))return;
    const char *id=gtk_editable_get_text(GTK_EDITABLE(panel->job));const char *reason=gtk_editable_get_text(GTK_EDITABLE(panel->reason));
    int action=GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button),"action"));UmiEnterpriseSnapshot snapshot;
    UmiStatus status=UmiEnterpriseWorkspaceSnapshot(panel->workspace,&snapshot);
    if(status!=UMI_STATUS_OK){Say(panel,status);return;}
    if(strcmp(id,panel->loadedJob)!=0 || snapshot.revision!=panel->loadedRevision){gtk_label_set_text(panel->message,"Choose Load frozen job and inspect its before/after rows first. Changing the practice role or reloading requires a fresh review.");return;}
    if(action==1||action==2)status=UmiEnterpriseWorkspaceReview(panel->workspace,Actor(panel),id,action==1,reason);
    else if(action==3)status=UmiEnterpriseWorkspaceCancel(panel->workspace,Actor(panel),id,reason);
    else status=UmiEnterpriseWorkspaceExecute(panel->workspace,Actor(panel),id);
    ForgetReview(panel);Say(panel,status);Refresh(panel);
}
static void RecipeAction(GtkButton *button,gpointer data)
{
    UmiEnterpriseWorkspaceGtkPanel *panel=data;if(!Ready(panel))return;
    bool enabled=GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button),"enabled"))!=0;
    UmiStatus status=UmiEnterpriseWorkspaceSetRecipe(panel->workspace,Actor(panel),Recipe(panel),enabled);
    ForgetReview(panel);Say(panel,status);Refresh(panel);
}
static void PauseAction(GtkButton *button,gpointer data)
{
    UmiEnterpriseWorkspaceGtkPanel *panel=data;if(!Ready(panel))return;
    bool paused=GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button),"paused"))!=0;
    UmiStatus status=UmiEnterpriseWorkspaceSetPaused(panel->workspace,Actor(panel),paused);
    ForgetReview(panel);Say(panel,status);Refresh(panel);
}
static void CheckAccess(GtkButton *button,gpointer data)
{
    UmiEnterpriseWorkspaceGtkPanel *panel=data;(void)button;if(!Ready(panel))return;
    UmiAuthorisationDecision decision;
    UmiStatus status=UmiEnterpriseWorkspaceCheckAccess(panel->workspace,Actor(panel),gtk_editable_get_text(GTK_EDITABLE(panel->capability)),gtk_editable_get_text(GTK_EDITABLE(panel->resource)),&decision);
    if(status!=UMI_STATUS_OK)Say(panel,status);
    else {char *message=g_strdup_printf("%s: %s. This check does not execute the action.",decision.allowed?"Allowed":"Denied",decision.reason);gtk_label_set_text(panel->message,message);g_free(message);}
}
static void OpenStorage(GtkButton *button,gpointer data)
{
    UmiEnterpriseWorkspaceGtkPanel *panel=data;(void)button;
    if(panel->workspace!=NULL){gtk_label_set_text(panel->message,"Storage is already open. Use Reload shared workspace to refresh it.");return;}
    const char *override=g_getenv("UMICOM_ENTERPRISE_WORKSPACE_DB");
    char *path=override!=NULL&&override[0]!='\0'?g_strdup(override):g_build_filename(g_get_user_data_dir(),"umicom","enterprise-workspace","workstation.sqlite",NULL);
    UmiStatus status=g_path_is_absolute(path)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
    if(status==UMI_STATUS_OK){char *directory=g_path_get_dirname(path);if(g_mkdir_with_parents(directory,0700)!=0)status=UMI_STATUS_IO_ERROR;g_free(directory);}
    if(status==UMI_STATUS_OK)status=UmiEnterprisePracticeAccessCreate(&panel->ownedAccess);
    if(status==UMI_STATUS_OK)status=umi_data_server_create_sqlite(path,&panel->ownedData);
    if(status==UMI_STATUS_OK)status=UmiEnterpriseWorkspaceOpen(panel->ownedData,UmiEnterprisePracticeAuthorisation(panel->ownedAccess),&panel->workspace);
    if(status==UMI_STATUS_OK){panel->ownsWorkspace=true;gtk_widget_set_sensitive(panel->open,FALSE);Refresh(panel);}
    else {umi_data_server_destroy(panel->ownedData);panel->ownedData=NULL;UmiEnterprisePracticeAccessDestroy(panel->ownedAccess);panel->ownedAccess=NULL;}
    gtk_label_set_text(panel->storage,path);g_free(path);Say(panel,status);
}
static GtkWidget *Button(GtkBox *box,const char *label,const char *id,GCallback callback,UmiEnterpriseWorkspaceGtkPanel *panel)
{
    GtkWidget *button=gtk_button_new_with_label(label);Tag(button,id);gtk_box_append(box,button);g_signal_connect(button,"clicked",callback,panel);return button;
}
static GtkEntry *Entry(GtkBox *box,const char *label,const char *id,const char *initial,int max)
{
    GtkWidget *line=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,8);GtkWidget *caption=gtk_label_new(label);GtkWidget *entry=gtk_entry_new();
    gtk_label_set_xalign(GTK_LABEL(caption),0.0F);gtk_widget_set_size_request(caption,120,-1);gtk_widget_set_hexpand(entry,TRUE);
    gtk_entry_set_max_length(GTK_ENTRY(entry),max);gtk_editable_set_text(GTK_EDITABLE(entry),initial);Tag(entry,id);
    gtk_box_append(GTK_BOX(line),caption);gtk_box_append(GTK_BOX(line),entry);gtk_box_append(box,line);return GTK_ENTRY(entry);
}
static GtkTextBuffer *Text(GtkBox *box,const char *id,bool editable,const char *initial)
{
    GtkWidget *scroll=gtk_scrolled_window_new();GtkWidget *view=gtk_text_view_new();Tag(view,id);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view),editable);gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view),GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(view),TRUE);gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),view);
    gtk_widget_set_vexpand(scroll,TRUE);gtk_widget_set_size_request(scroll,-1,180);gtk_box_append(box,scroll);
    GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));gtk_text_buffer_set_text(buffer,initial,-1);return buffer;
}
static GtkBox *Page(GtkNotebook *pages,const char *title)
{
    GtkWidget *box=gtk_box_new(GTK_ORIENTATION_VERTICAL,8);
    gtk_widget_set_margin_start(box,12);gtk_widget_set_margin_end(box,12);gtk_widget_set_margin_top(box,12);gtk_widget_set_margin_bottom(box,12);
    gtk_notebook_append_page(pages,box,gtk_label_new(title));return GTK_BOX(box);
}
UmiStatus UmiEnterpriseWorkspaceGtkPanelCreate(UmiEnterpriseWorkspace *workspace,
    const char *description,const char *applicationId,UmiEnterpriseWorkspaceGtkPanel **outPanel)
{
    if(applicationId==NULL||outPanel==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    *outPanel=NULL;UmiEnterpriseWorkspaceGtkPanel *panel=g_new0(UmiEnterpriseWorkspaceGtkPanel,1U);panel->workspace=workspace;
    panel->root=gtk_box_new(GTK_ORIENTATION_VERTICAL,8);g_object_ref_sink(panel->root);Tag(panel->root,"enterprise.workspace");
    GtkBox *root=GTK_BOX(panel->root),*toolbar=GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL,8));gtk_box_append(root,GTK_WIDGET(toolbar));
    panel->open=Button(toolbar,"Open practice storage","enterprise.open",G_CALLBACK(OpenStorage),panel);
    (void)Button(toolbar,"Reload shared workspace","enterprise.reload",G_CALLBACK(Reload),panel);
    const char *labels[UMI_ENTERPRISE_PRACTICE_ACTORS+1U]={0};for(size_t i=0U;i<UMI_ENTERPRISE_PRACTICE_ACTORS;++i)labels[i]=UmiEnterprisePracticeActorLabel(i);
    panel->actor=GTK_DROP_DOWN(gtk_drop_down_new_from_strings(labels));Tag(GTK_WIDGET(panel->actor),"enterprise.actor");gtk_box_append(toolbar,GTK_WIDGET(panel->actor));
    g_signal_connect(panel->actor,"notify::selected",G_CALLBACK(Changed),panel);
    panel->storage=GTK_LABEL(gtk_label_new(description!=NULL?description:"Storage is closed. Use fictional reference data, not customer details or secrets."));
    gtk_label_set_wrap(panel->storage,TRUE);gtk_label_set_selectable(panel->storage,TRUE);gtk_box_append(root,GTK_WIDGET(panel->storage));
    panel->summary=GTK_LABEL(gtk_label_new("No data is open. Opening this window does not run an import."));gtk_label_set_wrap(panel->summary,TRUE);gtk_box_append(root,GTK_WIDGET(panel->summary));
    panel->message=GTK_LABEL(gtk_label_new("Begin by opening practice storage."));Tag(GTK_WIDGET(panel->message),"enterprise.message");gtk_label_set_wrap(panel->message,TRUE);gtk_label_set_selectable(panel->message,TRUE);gtk_box_append(root,GTK_WIDGET(panel->message));
    panel->dataset=Entry(root,"Dataset ID","enterprise.dataset","supplies",63);
    const char *recipes[]={"stock.csv — preserve labels","stock.csv.upper — uppercase ASCII labels",NULL};
    panel->recipe=GTK_DROP_DOWN(gtk_drop_down_new_from_strings(recipes));Tag(GTK_WIDGET(panel->recipe),"enterprise.recipe");gtk_box_append(root,GTK_WIDGET(panel->recipe));
    panel->job=Entry(root,"Job ID","enterprise.job","stock-opening",63);
    GtkNotebook *pages=GTK_NOTEBOOK(gtk_notebook_new());Tag(GTK_WIDGET(pages),"enterprise.pages");gtk_widget_set_vexpand(GTK_WIDGET(pages),TRUE);gtk_box_append(root,GTK_WIDGET(pages));
    GtkBox *page=Page(pages,"Data");panel->name=Entry(page,"Dataset name","enterprise.name","Workshop supplies",191);
    (void)Button(page,"Create dataset","enterprise.create",G_CALLBACK(CreateDataset),panel);panel->dataReport=Text(page,"enterprise.data",false,"");
    page=Page(pages,"Import");panel->csv=Text(page,"enterprise.csv",true,"item_id,label,quantity\nnotebooks,Notebooks,12\npencils,Pencils,8\n");
    GtkBox *buttons=GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL,8));gtk_box_append(page,GTK_WIDGET(buttons));
    (void)Button(buttons,"Preview editable CSV","enterprise.preview",G_CALLBACK(PreviewOrPrepare),panel);
    GtkWidget *button=Button(buttons,"Prepare frozen job","enterprise.prepare",G_CALLBACK(PreviewOrPrepare),panel);g_object_set_data(G_OBJECT(button),"prepare",GINT_TO_POINTER(1));
    panel->previewReport=Text(page,"enterprise.preview-report",false,"Preview the CSV before saving it as a job.");
    page=Page(pages,"Jobs");panel->reason=Entry(page,"Review reason","enterprise.reason","Checked against the stock sheet",191);
    (void)Button(page,"Load frozen job","enterprise.load-job",G_CALLBACK(LoadJob),panel);
    buttons=GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL,8));gtk_box_append(page,GTK_WIDGET(buttons));
    const char *actions[]={"Approve","Reject","Cancel","Apply approved job"};const char *ids[]={"enterprise.approve","enterprise.reject","enterprise.cancel","enterprise.apply"};
    for(int i=0;i<4;++i){button=Button(buttons,actions[i],ids[i],G_CALLBACK(JobAction),panel);g_object_set_data(G_OBJECT(button),"action",GINT_TO_POINTER(i+1));}
    panel->jobsReport=Text(page,"enterprise.jobs",false,"");panel->auditReport=Text(page,"enterprise.audit",false,"");
    page=Page(pages,"Access");panel->capability=Entry(page,"Capability","enterprise.capability","enterprise.job.execute",127);panel->resource=Entry(page,"Resource ID","enterprise.resource","stock-opening",63);
    (void)Button(page,"Check access without executing","enterprise.check",G_CALLBACK(CheckAccess),panel);
    buttons=GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL,8));gtk_box_append(page,GTK_WIDGET(buttons));
    button=Button(buttons,"Pause imports","enterprise.pause",G_CALLBACK(PauseAction),panel);g_object_set_data(G_OBJECT(button),"paused",GINT_TO_POINTER(1));
    (void)Button(buttons,"Resume imports","enterprise.resume",G_CALLBACK(PauseAction),panel);
    GtkWidget *note=gtk_label_new("These identities are for practice only. They are not passwords or authenticated sessions.\nPause affects only this reference-data workspace, not other Umicom applications or operating-system services.");gtk_label_set_wrap(GTK_LABEL(note),TRUE);gtk_box_append(page,note);
    page=Page(pages,"Recipes");buttons=GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL,8));gtk_box_append(page,GTK_WIDGET(buttons));
    button=Button(buttons,"Enable selected recipe","enterprise.enable",G_CALLBACK(RecipeAction),panel);g_object_set_data(G_OBJECT(button),"enabled",GINT_TO_POINTER(1));
    (void)Button(buttons,"Disable selected recipe","enterprise.disable",G_CALLBACK(RecipeAction),panel);panel->recipeReport=Text(page,"enterprise.recipes",false,"");
    int initial=strstr(applicationId,"integration")!=NULL?1:strstr(applicationId,"operations")!=NULL?2:strstr(applicationId,"security")!=NULL?3:strstr(applicationId,"marketplace")!=NULL?4:0;
    gtk_notebook_set_current_page(pages,initial);
    if(workspace!=NULL){gtk_widget_set_sensitive(panel->open,FALSE);Refresh(panel);gtk_label_set_text(panel->message,"Injected practice workspace is ready. Select a practice role before changing data.");}
    /* A scrollable outer surface keeps the full form reachable on smaller
     * monitors; no fixed window size or application-specific CSS is required. */
    GtkWidget *outer=gtk_scrolled_window_new();g_object_ref_sink(outer);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(outer),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(outer),panel->root);
    g_object_unref(panel->root);panel->root=outer;Tag(outer,"enterprise.workspace");
    *outPanel=panel;return UMI_STATUS_OK;
}
GtkWidget *UmiEnterpriseWorkspaceGtkPanelWidget(UmiEnterpriseWorkspaceGtkPanel *panel)
{
    return panel!=NULL?panel->root:NULL;
}
static void Disconnect(GtkWidget *widget,void *panel)
{
    g_signal_handlers_disconnect_by_data(widget,panel);
    for(GtkWidget *child=gtk_widget_get_first_child(widget);child!=NULL;child=gtk_widget_get_next_sibling(child))Disconnect(child,panel);
}
void UmiEnterpriseWorkspaceGtkPanelDestroy(UmiEnterpriseWorkspaceGtkPanel *panel)
{
    if(panel==NULL)return;
    Disconnect(panel->root,panel);
    if(panel->ownsWorkspace)UmiEnterpriseWorkspaceDestroy(panel->workspace);
    panel->workspace=NULL;umi_data_server_destroy(panel->ownedData);UmiEnterprisePracticeAccessDestroy(panel->ownedAccess);
    g_object_unref(panel->root);g_free(panel);
}
