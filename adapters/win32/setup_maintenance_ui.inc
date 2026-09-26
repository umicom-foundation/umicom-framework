/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: adapters/win32/setup_maintenance_ui.inc
 * Maintenance uses Setup Centre's existing worker, cancellation and UI lifetime.
 * The displayed installed list is bound to its receipt hash. A reordered list
 * cannot reinterpret a removal selection after another installation changes it.
 *---------------------------------------------------------------------------*/
static uint64_t MaintenanceSelected(Ui *ui)
{
    uint64_t mask=0U;
    for(size_t i=0U;i<UmiSetupApplicationCount(ui->maintenanceBundle);++i)
        if(ListView_GetCheckState(Find(ui,ID_MAINT_LIST),(int)i))mask|=UINT64_C(1)<<i;
    return mask;
}
static void MaintenanceForget(Ui *ui,int forgetList)
{
    UmiSetupMaintenancePlanDestroy(ui->maintenancePlan);ui->maintenancePlan=NULL;
    if(forgetList){UmiSetupBundleDestroy(ui->maintenanceBundle);ui->maintenanceBundle=NULL;ListView_DeleteAllItems(Find(ui,ID_MAINT_LIST));}
}
static void MaintenanceButtons(Ui *ui)
{
    int idle=ui->job==NULL;
    int remove=SendMessageW(Find(ui,ID_MAINT_ACTION),CB_GETCURSEL,0,0)==2;
    EnableWindow(Find(ui,ID_MAINT_REVIEW),idle&&ui->maintenanceBundle!=NULL);
    EnableWindow(Find(ui,ID_MAINT_APPLY),idle&&ui->maintenancePlan!=NULL);
    EnableWindow(Find(ui,ID_MAINT_LIST),idle&&remove&&ui->maintenanceBundle!=NULL);
    EnableWindow(Find(ui,ID_MAINT_ALL),idle&&remove&&ui->maintenanceBundle!=NULL);
    EnableWindow(Find(ui,ID_MAINT_NONE),idle&&remove&&ui->maintenanceBundle!=NULL);
    int id=GetWindowTextLengthW(Find(ui,ID_MAINT_ID))==64;
    EnableWindow(Find(ui,ID_MAINT_RECOVER),idle&&id);
    EnableWindow(Find(ui,ID_MAINT_UNDO),idle&&id);
}
static UmiStatus MaintenanceInspect(Job *job)
{
    if(job->maintenanceAction==UMI_SETUP_MAINTENANCE_REMOVE)return UMI_STATUS_OK;
    UmiSetupBundle *source=NULL,*installed=NULL;
    UmiStatus status=UmiSetupBundleOpen(job->source,&source,&job->report);
    if(status==UMI_STATUS_OK)status=UmiSetupInstalledBundleOpen(job->destination,&installed,&job->report);
    uint64_t mask=0U;
    if(status==UMI_STATUS_OK) {
        const char *identity=UmiSetupMaintenanceReceiptIdentity(installed);
        if(!identity||strcmp(identity,job->maintenanceReceipt))status=UMI_STATUS_INVALID_STATE;
    }
    for(size_t i=0U;status==UMI_STATUS_OK&&i<UmiSetupApplicationCount(installed);++i) {
        const UmiSetupApplication *old=UmiSetupApplicationAt(installed,i);size_t j=0U;
        for(;j<UmiSetupApplicationCount(source);++j)
            if(!strcmp(old->id,UmiSetupApplicationAt(source,j)->id))break;
        if(j==UmiSetupApplicationCount(source))status=UMI_STATUS_INVALID_ARGUMENT;
        else mask|=UINT64_C(1)<<j;
    }
    if(status==UMI_STATUS_OK)status=UmiReleaseInspectBundle(source,mask,InspectionProgress,job,&job->inspection);
    UmiSetupBundleDestroy(installed);UmiSetupBundleDestroy(source);
    if(status!=UMI_STATUS_OK) {
        job->maintenanceReport.io.status=status;
        (void)snprintf(job->maintenanceReport.io.detail,sizeof job->maintenanceReport.io.detail,
            "Windows release inspection or installed-list binding failed. %.180s",job->inspection.firstIssue);
    }
    return status;
}
static void MaintenanceWorker(Job *job)
{
    Ui *ui=job->ui;
    if(job->kind==JOB_MAINT_LOAD) {
        job->status=UmiSetupMaintenancePending(job->destination,&job->maintenanceReport);
        if(job->status==UMI_STATUS_NOT_FOUND)
            job->status=UmiSetupInstalledBundleOpen(job->destination,&job->loaded,&job->maintenanceReport.io);
    } else if(job->kind==JOB_MAINT_REVIEW) {
        UmiSetupMaintenanceConfig config={job->destination,job->source,job->maintenanceAction,
            job->maintenanceAction==UMI_SETUP_MAINTENANCE_REMOVE?job->selected:0U,
            job->maintenanceReceipt[0]?job->maintenanceReceipt:NULL};
        job->status=MaintenanceInspect(job);
        if(job->status==UMI_STATUS_OK)job->status=UmiSetupMaintenancePlanCreate(&config,&job->maintenancePlan,Progress,job,&job->maintenanceReport);
    } else if(job->kind==JOB_MAINT_APPLY) {
        job->status=MaintenanceInspect(job);
        if(job->status==UMI_STATUS_OK)job->status=UmiSetupMaintenanceApply(ui->maintenancePlan,job->fingerprint,Progress,job,&job->maintenanceReport);
    } else {
        job->status=UmiSetupMaintenanceRestore(job->destination,job->fingerprint,
            job->kind==JOB_MAINT_RECOVER,Progress,job,&job->maintenanceReport);
    }
    job->report=job->maintenanceReport.io;
}
static void MaintenanceDone(Ui *ui,Job *job)
{
    if(job->kind==JOB_MAINT_LOAD) {
        MaintenanceForget(ui,1);
        if(job->status==UMI_STATUS_OK&&job->loaded) {
            ui->maintenanceBundle=job->loaded;job->loaded=NULL;
            for(size_t i=0U;i<UmiSetupApplicationCount(ui->maintenanceBundle);++i) {
                wchar_t *label=Display(UmiSetupApplicationAt(ui->maintenanceBundle,i)->title);
                if(label) {
                    LVITEMW item={0};item.mask=LVIF_TEXT;item.iItem=(int)i;item.pszText=label;
                    SendMessageW(Find(ui,ID_MAINT_LIST),LVM_INSERTITEMW,0,(LPARAM)&item);free(label);
                }
            }
        }
    }
    if(job->kind==JOB_MAINT_REVIEW) {
        MaintenanceForget(ui,0);
        if(job->status==UMI_STATUS_OK){ui->maintenancePlan=job->maintenancePlan;job->maintenancePlan=NULL;}
    }
    if(job->kind==JOB_MAINT_APPLY||job->kind==JOB_MAINT_RECOVER||job->kind==JOB_MAINT_UNDO)MaintenanceForget(ui,1);
    if(job->maintenanceReport.transaction[0]) {
        wchar_t *id=Display(job->maintenanceReport.transaction);
        if(id){SetWindowTextW(Find(ui,ID_MAINT_ID),id);free(id);}
    }
}
static char *MaintenanceText(Ui *ui,const Job *job)
{
    const size_t capacity=60000U;char *text=calloc(1U,capacity);if(!text)return NULL;
    int count=snprintf(text,capacity,"Installation: %s\r\n",job->destination);
    if(count<0||(size_t)count>=capacity){free(text);return NULL;}
    size_t used=(size_t)count;
    if(job->kind==JOB_MAINT_REVIEW&&ui->maintenancePlan) {
        const UmiSetupMaintenanceSummary *summary=UmiSetupMaintenancePlanSummary(ui->maintenancePlan);
        count=snprintf(text+used,capacity-used,
            "Plan: %s\r\nReceipt before: %s\r\nReceipt after: %s\r\nApplications: %zu to %zu\r\n"
            "Replace: %zu; archive: %zu; preserve edits: %zu; staged bytes: %llu\r\n",
            summary->fingerprint,summary->beforeReceipt,summary->afterReceipt[0]?summary->afterReceipt:"none",
            summary->applicationsBefore,summary->applicationsAfter,summary->filesToWrite,summary->filesToRetire,
            summary->modifiedFilesPreserved,(unsigned long long)summary->stagingBytes);
        if(count<0||(size_t)count>=capacity-used){free(text);return NULL;}used+=(size_t)count;
        for(size_t i=0U;i<summary->checkedFiles;++i) {
            const UmiSetupMaintenanceRow *row=UmiSetupMaintenancePlanRow(ui->maintenancePlan,i);
            if(row->change==UMI_SETUP_MAINTENANCE_KEEP)continue;
            if(capacity-used<1200U){(void)snprintf(text+used,capacity-used,"Additional rows omitted from this view. Use the native review command for the full list.\r\n");break;}
            count=snprintf(text+used,capacity-used,"%s: %s\r\n",UmiSetupMaintenanceChangeText(row->change),row->relative);
            if(count<0||(size_t)count>=capacity-used){free(text);return NULL;}used+=(size_t)count;
        }
    } else {
        (void)snprintf(text+used,capacity-used,
            "Transaction: %s\r\nArchive: %s\r\nRecovery required: %s\r\n"
            "Files only: no database migration, registry edit, shortcut removal or application launch.",
            job->maintenanceReport.transaction[0]?job->maintenanceReport.transaction:"none",
            job->maintenanceReport.archive[0]?job->maintenanceReport.archive:"none",
            job->maintenanceReport.recoveryRequired?"yes":
                job->status==UMI_STATUS_OK?"no pending change reported":"not established; inspect the pending state");
    }
    return text;
}
static void MaintenanceBuild(Ui *ui)
{
    Edit(ui,ID_MAINT_ROOT,ID_MAINT_ROOT_BROWSE,3,L"Installed folder",125);
    Edit(ui,ID_MAINT_SOURCE,ID_MAINT_SOURCE_BROWSE,3,L"Offline release folder",166);
    Add(ui,ID_MAINT_LOAD,3,L"BUTTON",L"Load installed / pending",BS_PUSHBUTTON|WS_TABSTOP,30,207,220,28);
    Label(ui,3,L"Maintenance action",280,211,155,24);
    HWND action=Add(ui,ID_MAINT_ACTION,3,L"COMBOBOX",L"",CBS_DROPDOWNLIST|WS_TABSTOP,450,207,370,100);
    SendMessageW(action,CB_ADDSTRING,0,(LPARAM)L"Update installed applications");
    SendMessageW(action,CB_ADDSTRING,0,(LPARAM)L"Repair from the original release");
    SendMessageW(action,CB_ADDSTRING,0,(LPARAM)L"Remove checked applications");
    SendMessageW(action,CB_SETCURSEL,0,0);
    HWND list=Add(ui,ID_MAINT_LIST,3,WC_LISTVIEWW,L"",LVS_REPORT|LVS_NOCOLUMNHEADER|WS_BORDER|WS_TABSTOP,30,248,-60,120);
    ListView_SetExtendedListViewStyle(list,LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT|LVS_EX_DOUBLEBUFFER);
    LVCOLUMNW column={0};column.mask=LVCF_TEXT|LVCF_WIDTH;column.pszText=L"Installed applications";column.cx=800;
    SendMessageW(list,LVM_INSERTCOLUMNW,0,(LPARAM)&column);
    Add(ui,ID_MAINT_ALL,3,L"BUTTON",L"Check all",BS_PUSHBUTTON|WS_TABSTOP,30,378,100,26);
    Add(ui,ID_MAINT_NONE,3,L"BUTTON",L"Check none",BS_PUSHBUTTON|WS_TABSTOP,145,378,110,26);
    Label(ui,3,L"Check boxes apply to removal only. Original files stay in an archive.",275,382,640,24);
    Add(ui,ID_MAINT_REVIEW,3,L"BUTTON",L"Review maintenance",BS_PUSHBUTTON|WS_TABSTOP,30,416,205,30);
    Add(ui,ID_MAINT_APPLY,3,L"BUTTON",L"Apply reviewed maintenance",BS_PUSHBUTTON|WS_TABSTOP,255,416,260,30);
    Edit(ui,ID_MAINT_ID,0,3,L"Transaction ID",457);
    Add(ui,ID_MAINT_RECOVER,3,L"BUTTON",L"Recover unfinished change",BS_PUSHBUTTON|WS_TABSTOP,30,497,245,30);
    Add(ui,ID_MAINT_UNDO,3,L"BUTTON",L"Undo completed change",BS_PUSHBUTTON|WS_TABSTOP,295,497,240,30);
}
