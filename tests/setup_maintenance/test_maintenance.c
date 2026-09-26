/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Real filesystem tests of the production maintenance API. Fixture payloads are
 * inert text, not functioning Windows applications. No fixture is executed.
 * Each invocation owns a new directory; retained files help inspect a failure.
 *---------------------------------------------------------------------------*/
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "maintenance_internal.h"
#include "../../examples/setup_maintenance/review_client.h"
#include <time.h>
#include <inttypes.h>
#include <limits.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#endif
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); return 1; } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
typedef struct Fixture {
    char base[UMI_SETUP_PATH_CAPACITY];
    char oldRelease[UMI_SETUP_PATH_CAPACITY], newRelease[UMI_SETUP_PATH_CAPACITY];
    char root[UMI_SETUP_PATH_CAPACITY];
} Fixture;
static int Path(const char *root, const char *name, char out[UMI_SETUP_PATH_CAPACITY]) { return SmPath(root,name,out)==UMI_STATUS_OK; }
static int Put(const char *root, const char *name, const char *text)
{
    char path[UMI_SETUP_PATH_CAPACITY];
    CHECK(Path(root,name,path));
    OK(ScParents(root,name,NULL)); OK(ScWriteNew(path,text,strlen(text),NULL)); return 0;
}
static int Change(const char *root, const char *name, const char *text)
{
    char path[UMI_SETUP_PATH_CAPACITY]; CHECK(Path(root,name,path));
#ifdef _WIN32
    int count=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,path,-1,NULL,0);
    CHECK(count>0); wchar_t *wide=malloc((size_t)count*sizeof *wide);CHECK(wide!=NULL);
    CHECK(MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,path,-1,wide,count)==count);
    CHECK(DeleteFileW(wide));free(wide);
#else
    CHECK(unlink(path)==0);
#endif
    if(text!=NULL) { OK(ScWriteNew(path,text,strlen(text),NULL)); }
    return 0;
}
static int Is(const char *root, const char *name, const char *expected)
{
    char path[UMI_SETUP_PATH_CAPACITY],*bytes=NULL;size_t count=0;CHECK(Path(root,name,path));
    UmiStatus status=ScRead(path,100000U,&bytes,&count,NULL);
    if(expected==NULL){free(bytes);CHECK(status==UMI_STATUS_NOT_FOUND);return 0;}
    CHECK(status==UMI_STATUS_OK);CHECK(count==strlen(expected));CHECK(!memcmp(bytes,expected,count));free(bytes);return 0;
}
static int FileLine(ScText *text,const char *owner,const char *path,const char *data)
{
    char hash[65];OK(UmiNativeSha256Buffer(data,strlen(data),hash));
    ScPrint(text,"file\t%s\t%zu\t%s\t%s\n",owner,strlen(data),hash,path);return 0;
}
static int Release(const char *root,int newer)
{
    OK(ScMakeDirectory(root,NULL));
    const char *notes=newer?"Notes binary generation two\n":"Notes binary generation one\n";
    const char *stock=newer?"Stock binary generation two\n":"Stock binary generation one\n";
    const char *shared=newer?"Shared runtime two\n":"Shared runtime one\n";
    CHECK(!Put(root,"payload/bin/notes.exe",notes));CHECK(!Put(root,"payload/bin/stock.exe",stock));
    CHECK(!Put(root,"payload/bin/common.dll",shared));
    CHECK(!Put(root,newer?"payload/share/new-help.txt":"payload/share/old-help.txt",newer?"New help\n":"Old help\n"));
    ScText catalogue;ScTextInit(&catalogue);ScPrint(&catalogue,"UMICOM_SUITE\t1\napp\tnotes\tUmicom Notes\tbin/notes.exe\napp\tstock\tUmicom Stock\tbin/stock.exe\n");
    CHECK(!FileLine(&catalogue,"notes","bin/notes.exe",notes));CHECK(!FileLine(&catalogue,"stock","bin/stock.exe",stock));
    CHECK(!FileLine(&catalogue,"shared","bin/common.dll",shared));
    CHECK(!FileLine(&catalogue,"notes",newer?"share/new-help.txt":"share/old-help.txt",newer?"New help\n":"Old help\n"));
    CHECK(catalogue.status==UMI_STATUS_OK);CHECK(!Put(root,UMI_SETUP_CATALOGUE,catalogue.data));ScTextFree(&catalogue);return 0;
}
static int Init(Fixture *fixture,const char *base,const char *name)
{
    struct timespec now;CHECK(timespec_get(&now,TIME_UTC)==TIME_UTC);
#ifdef _WIN32
    unsigned long pid=(unsigned long)GetCurrentProcessId();
#else
    unsigned long pid=(unsigned long)getpid();
#endif
    char leaf[200];int n=snprintf(leaf,sizeof leaf,"%s-%lu-%lld-%ld",name,pid,(long long)now.tv_sec,now.tv_nsec);CHECK(n>0&&(size_t)n<sizeof leaf);
    CHECK(Path(base,leaf,fixture->base));OK(ScMakeDirectory(fixture->base,NULL));
    CHECK(Path(fixture->base,"release-one",fixture->oldRelease));CHECK(Path(fixture->base,"release-two",fixture->newRelease));
    CHECK(Path(fixture->base,"installed notes-\xc3\xa9",fixture->root));
    CHECK(!Release(fixture->oldRelease,0));CHECK(!Release(fixture->newRelease,1));
    UmiSetupBundle *bundle=NULL;UmiSetupReport report={0};char fingerprint[65];
    OK(UmiSetupBundleOpen(fixture->oldRelease,&bundle,&report));
    OK(UmiSetupReview(bundle,UmiSetupAllApplications(bundle),fixture->root,fingerprint,NULL,NULL,&report));
    OK(UmiSetupInstall(bundle,UmiSetupAllApplications(bundle),fixture->root,fingerprint,NULL,NULL,&report));
    UmiSetupBundleDestroy(bundle);CHECK(!Put(fixture->root,"my-notes.txt","My personal document\n"));return 0;
}
static UmiStatus Plan(Fixture *fixture,UmiSetupMaintenanceAction action,uint64_t mask,UmiSetupMaintenancePlan **plan,UmiSetupMaintenanceReport *report)
{
    UmiSetupMaintenanceConfig config={fixture->root,action==UMI_SETUP_MAINTENANCE_UPDATE?fixture->newRelease:fixture->oldRelease,action,mask,NULL};
    return UmiSetupMaintenancePlanCreate(&config,plan,NULL,NULL,report);
}
static UmiStatus Apply(UmiSetupMaintenancePlan *plan,UmiSetupProgress progress,void *context,UmiSetupMaintenanceReport *report)
{
    char hash[65];strcpy(hash,UmiSetupMaintenancePlanSummary(plan)->fingerprint);
    return UmiSetupMaintenanceApply(plan,hash,progress,context,report);
}
static int Previous(Fixture *fixture)
{
    CHECK(!Is(fixture->root,"bin/notes.exe","Notes binary generation one\n"));
    CHECK(!Is(fixture->root,"bin/stock.exe","Stock binary generation one\n"));
    CHECK(!Is(fixture->root,"bin/common.dll","Shared runtime one\n"));
    CHECK(!Is(fixture->root,"share/old-help.txt","Old help\n"));
    CHECK(!Is(fixture->root,"my-notes.txt","My personal document\n"));
    OK(UmiSetupVerifyInstallation(fixture->root,NULL));return 0;
}
typedef struct StopAt {const char *phase;unsigned hit;unsigned seen;int crash;} StopAt;
static int Stop(const UmiSetupReport *report,void *context)
{
    StopAt *stop=context;
    if(strcmp(report->detail,stop->phase)!=0)return 0;
    if(++stop->seen!=stop->hit)return 0;
#ifndef _WIN32
    if(stop->crash)_exit(79);
#endif
    return 1;
}
static int Normal(Fixture *fixture,const char *name)
{
    UmiSetupMaintenancePlan *plan=NULL;UmiSetupMaintenanceReport report={0};
    if(!strcmp(name,"update")||!strcmp(name,"undo-update")) {
        OK(Plan(fixture,UMI_SETUP_MAINTENANCE_UPDATE,0U,&plan,&report));
        const UmiSetupMaintenanceSummary *summary=UmiSetupMaintenancePlanSummary(plan);
        CHECK(summary->filesToWrite==4U&&summary->filesToRetire==1U&&summary->applicationsAfter==2U);
        OK(Apply(plan,NULL,NULL,&report));CHECK(report.io.completed&&!report.recoveryRequired);
        CHECK(!Is(fixture->root,"bin/notes.exe","Notes binary generation two\n"));CHECK(!Is(fixture->root,"share/old-help.txt",NULL));
        CHECK(!Is(fixture->root,"share/new-help.txt","New help\n"));CHECK(!Is(fixture->root,"my-notes.txt","My personal document\n"));
        OK(UmiSetupVerifyInstallation(fixture->root,NULL));
        if(!strcmp(name,"undo-update")) {char id[65];strcpy(id,report.transaction);OK(UmiSetupMaintenanceRestore(fixture->root,id,0,NULL,NULL,&report));CHECK(report.restored);CHECK(!Previous(fixture));CHECK(!Is(fixture->root,"share/new-help.txt",NULL));}
    } else if(!strcmp(name,"repair")||!strcmp(name,"undo-repair")) {
        CHECK(!Change(fixture->root,"bin/notes.exe",NULL));CHECK(!Change(fixture->root,"bin/common.dll","Edited runtime\n"));
        OK(Plan(fixture,UMI_SETUP_MAINTENANCE_REPAIR,0U,&plan,&report));
        CHECK(plan->summary.filesToWrite==2U&&plan->summary.modifiedFilesPreserved==1U);OK(Apply(plan,NULL,NULL,&report));CHECK(!Previous(fixture));
        if(!strcmp(name,"undo-repair")){char id[65];strcpy(id,report.transaction);OK(UmiSetupMaintenanceRestore(fixture->root,id,0,NULL,NULL,&report));CHECK(report.restored);CHECK(!Is(fixture->root,"bin/notes.exe",NULL));CHECK(!Is(fixture->root,"bin/common.dll","Edited runtime\n"));CHECK(UmiSetupVerifyInstallation(fixture->root,NULL)!=UMI_STATUS_OK);}
    } else if(!strcmp(name,"remove-one")||!strcmp(name,"remove-edited")||!strcmp(name,"remove-all")||!strcmp(name,"undo-remove")) {
        int edit=!strcmp(name,"remove-edited");if(edit)CHECK(!Change(fixture->root,"bin/notes.exe","Locally edited owned file\n"));
        uint64_t mask=!strcmp(name,"remove-one")||edit?1U:3U;
        OK(Plan(fixture,UMI_SETUP_MAINTENANCE_REMOVE,mask,&plan,&report));OK(Apply(plan,NULL,NULL,&report));
        CHECK(!Is(fixture->root,"bin/notes.exe",edit?"Locally edited owned file\n":NULL));
        CHECK(!Is(fixture->root,"my-notes.txt","My personal document\n"));
        if(mask==1U){CHECK(!Is(fixture->root,"bin/common.dll","Shared runtime one\n"));OK(UmiSetupVerifyInstallation(fixture->root,NULL));}
        else{CHECK(report.allApplicationsRemoved);CHECK(UmiSetupInstalledBundleOpen(fixture->root,&(UmiSetupBundle *){NULL},NULL)==UMI_STATUS_NOT_FOUND);}
        if(!strcmp(name,"undo-remove")){char id[65];strcpy(id,report.transaction);OK(UmiSetupMaintenanceRestore(fixture->root,id,0,NULL,NULL,&report));CHECK(!Previous(fixture));}
    } else if(!strcmp(name,"no-change")) {
        OK(Plan(fixture,UMI_SETUP_MAINTENANCE_REPAIR,0U,&plan,&report));CHECK(plan->summary.noChange);OK(Apply(plan,NULL,NULL,&report));CHECK(report.io.completed&&report.transaction[0]==0);CHECK(!Previous(fixture));
    } else return 2;
    UmiSetupMaintenancePlanDestroy(plan);return 0;
}
static int Rejection(Fixture *fixture,const char *name)
{
    const char *known[]={"edited-update","unowned-collision","repair-wrong-release","remove-retained-damage","bad-mask","damaged-release","wrong-fingerprint","source-changed","receipt-changed","installed-changed","double-apply","journal-owner","malformed-pending"};
    int recognised=0;for(size_t i=0;i<sizeof known/sizeof known[0];++i)if(!strcmp(name,known[i]))recognised=1;
    if(!recognised)return 2;
    UmiSetupMaintenancePlan *plan=NULL;UmiSetupMaintenanceReport report={0};UmiStatus status;
    if(!strcmp(name,"edited-update")){CHECK(!Change(fixture->root,"bin/notes.exe","User edit\n"));CHECK(Plan(fixture,UMI_SETUP_MAINTENANCE_UPDATE,0,&plan,&report)==UMI_STATUS_INVALID_STATE);CHECK(!Is(fixture->root,"bin/notes.exe","User edit\n"));return 0;}
    if(!strcmp(name,"unowned-collision")){CHECK(!Put(fixture->root,"share/new-help.txt","User document\n"));CHECK(Plan(fixture,UMI_SETUP_MAINTENANCE_UPDATE,0,&plan,&report)==UMI_STATUS_ALREADY_EXISTS);CHECK(!Is(fixture->root,"share/new-help.txt","User document\n"));return 0;}
    if(!strcmp(name,"repair-wrong-release")){UmiSetupMaintenanceConfig config={fixture->root,fixture->newRelease,UMI_SETUP_MAINTENANCE_REPAIR,0,NULL};CHECK(UmiSetupMaintenancePlanCreate(&config,&plan,NULL,NULL,&report)==UMI_STATUS_INVALID_STATE);return 0;}
    if(!strcmp(name,"remove-retained-damage")){CHECK(!Change(fixture->root,"bin/stock.exe","Edited stock\n"));CHECK(Plan(fixture,UMI_SETUP_MAINTENANCE_REMOVE,1,&plan,&report)==UMI_STATUS_INVALID_STATE);CHECK(!Is(fixture->root,"bin/notes.exe","Notes binary generation one\n"));return 0;}
    if(!strcmp(name,"bad-mask")){CHECK(Plan(fixture,UMI_SETUP_MAINTENANCE_REMOVE,4,&plan,&report)==UMI_STATUS_INVALID_ARGUMENT);CHECK(Plan(fixture,UMI_SETUP_MAINTENANCE_REMOVE,0,&plan,&report)==UMI_STATUS_INVALID_ARGUMENT);return 0;}
    if(!strcmp(name,"damaged-release")){CHECK(!Change(fixture->newRelease,"payload/bin/notes.exe","Unrecorded replacement\n"));CHECK(Plan(fixture,UMI_SETUP_MAINTENANCE_UPDATE,0,&plan,&report)==UMI_STATUS_INVALID_STATE);return 0;}
    OK(Plan(fixture,UMI_SETUP_MAINTENANCE_UPDATE,0,&plan,&report));
    if(!strcmp(name,"wrong-fingerprint")){char hash[65];memset(hash,'0',64);hash[64]=0;CHECK(UmiSetupMaintenanceApply(plan,hash,NULL,NULL,&report)==UMI_STATUS_INVALID_STATE);}
    else if(!strcmp(name,"source-changed")){CHECK(!Change(fixture->newRelease,"payload/bin/notes.exe","Changed after review\n"));CHECK(Apply(plan,NULL,NULL,&report)!=UMI_STATUS_OK);}
    else if(!strcmp(name,"receipt-changed")){CHECK(!Change(fixture->root,UMI_SETUP_RECEIPT,"broken receipt\n"));CHECK(Apply(plan,NULL,NULL,&report)!=UMI_STATUS_OK);UmiSetupMaintenancePlanDestroy(plan);return 0;}
    else if(!strcmp(name,"installed-changed")){CHECK(!Change(fixture->root,"bin/notes.exe","Changed after review\n"));CHECK(Apply(plan,NULL,NULL,&report)==UMI_STATUS_INVALID_STATE);CHECK(!Is(fixture->root,"bin/notes.exe","Changed after review\n"));UmiSetupMaintenancePlanDestroy(plan);return 0;}
    else if(!strcmp(name,"double-apply")){OK(Apply(plan,NULL,NULL,&report));CHECK(Apply(plan,NULL,NULL,&report)==UMI_STATUS_INVALID_ARGUMENT);UmiSetupMaintenancePlanDestroy(plan);return 0;}
    else if(!strcmp(name,"journal-owner")){char dir[UMI_SETUP_PATH_CAPACITY];CHECK(Path(fixture->root,UMI_SETUP_MAINTENANCE_DIRECTORY,dir));OK(ScMakeDirectory(dir,NULL));CHECK(!Put(dir,"owner.txt","Someone else's directory\n"));CHECK(Apply(plan,NULL,NULL,&report)==UMI_STATUS_INVALID_STATE);}
    else if(!strcmp(name,"malformed-pending")){CHECK(!Put(fixture->root,UMI_SETUP_MAINTENANCE_PENDING,"unfinished or corrupt marker\n"));CHECK(UmiSetupVerifyInstallation(fixture->root,NULL)==UMI_STATUS_BUSY);CHECK(UmiSetupMaintenancePending(fixture->root,&report)==UMI_STATUS_PARSE_ERROR);CHECK(Apply(plan,NULL,NULL,&report)!=UMI_STATUS_OK);UmiSetupMaintenancePlanDestroy(plan);return 0;}
    else{UmiSetupMaintenancePlanDestroy(plan);return 2;}
    status=UmiSetupMaintenancePending(fixture->root,&report);CHECK(status==UMI_STATUS_NOT_FOUND);CHECK(!Previous(fixture));UmiSetupMaintenancePlanDestroy(plan);return 0;
}
static const char *Phases[]={"Maintenance pending; ordinary receipt readers are now blocked.","Previous receipt archived.","Original file archived.","Replacement file installed.","New receipt published; completion marker is next."};
static int Interrupted(Fixture *fixture,unsigned phase,int again,int undo)
{
    UmiSetupMaintenancePlan *plan=NULL;UmiSetupMaintenanceReport report={0};
    OK(Plan(fixture,UMI_SETUP_MAINTENANCE_UPDATE,0,&plan,&report));
    StopAt stop={Phases[phase],1,0,0};CHECK(Apply(plan,Stop,&stop,&report)==UMI_STATUS_CANCELLED);CHECK(stop.seen==1&&report.recoveryRequired);
    char id[65];strcpy(id,report.transaction);OK(UmiSetupMaintenancePending(fixture->root,&report));CHECK(!strcmp(id,report.transaction));CHECK(UmiSetupVerifyInstallation(fixture->root,NULL)==UMI_STATUS_BUSY);
    if(again){StopAt recover={"Previous file state restored.",2,0,0};CHECK(UmiSetupMaintenanceRestore(fixture->root,id,1,Stop,&recover,&report)==UMI_STATUS_CANCELLED);CHECK(report.recoveryRequired);}
    OK(UmiSetupMaintenanceRestore(fixture->root,id,1,NULL,NULL,&report));CHECK(report.restored&&!report.recoveryRequired);CHECK(!Previous(fixture));
    if(undo){UmiSetupMaintenancePlanDestroy(plan);plan=NULL;OK(Plan(fixture,UMI_SETUP_MAINTENANCE_UPDATE,0,&plan,&report));OK(Apply(plan,NULL,NULL,&report));CHECK(strcmp(id,report.transaction)!=0);}
    UmiSetupMaintenancePlanDestroy(plan);return 0;
}
static int RecoveryConflict(Fixture *fixture,const char *name)
{
    if(strcmp(name,"journal-tamper")&&strcmp(name,"before-tamper")&&strcmp(name,"live-recovery-edit")&&strcmp(name,"wrong-transaction"))return 2;
    UmiSetupMaintenancePlan *plan=NULL;UmiSetupMaintenanceReport report={0};OK(Plan(fixture,UMI_SETUP_MAINTENANCE_UPDATE,0,&plan,&report));
    StopAt stop={Phases[3],1,0,0};CHECK(Apply(plan,Stop,&stop,&report)==UMI_STATUS_CANCELLED);
    char id[65],archive[UMI_SETUP_PATH_CAPACITY];strcpy(id,report.transaction);strcpy(archive,report.archive);
    if(!strcmp(name,"journal-tamper")){CHECK(!Change(archive,"plan.umi","damaged journal\n"));CHECK(UmiSetupMaintenanceRestore(fixture->root,id,1,NULL,NULL,&report)!=UMI_STATUS_OK);CHECK(!Change(archive,"plan.umi",plan->journal));}
    else if(!strcmp(name,"before-tamper")){CHECK(!Change(archive,"before.umi","damaged before receipt\n"));CHECK(UmiSetupMaintenanceRestore(fixture->root,id,1,NULL,NULL,&report)!=UMI_STATUS_OK);CHECK(!Change(archive,"before.umi",plan->beforeText));}
    else if(!strcmp(name,"live-recovery-edit")){CHECK(!Change(fixture->root,"bin/common.dll","New personal change\n"));CHECK(UmiSetupMaintenanceRestore(fixture->root,id,1,NULL,NULL,&report)!=UMI_STATUS_OK);CHECK(!Is(fixture->root,"bin/common.dll","New personal change\n"));CHECK(!Change(fixture->root,"bin/common.dll","Shared runtime two\n"));}
    else if(!strcmp(name,"wrong-transaction")){char wrong[65];memset(wrong,'a',64);wrong[64]=0;CHECK(UmiSetupMaintenanceRestore(fixture->root,wrong,1,NULL,NULL,&report)!=UMI_STATUS_OK);}
    else{UmiSetupMaintenancePlanDestroy(plan);return 2;}
    CHECK(UmiSetupVerifyInstallation(fixture->root,NULL)==UMI_STATUS_BUSY);OK(UmiSetupMaintenanceRestore(fixture->root,id,1,NULL,NULL,&report));CHECK(!Previous(fixture));UmiSetupMaintenancePlanDestroy(plan);return 0;
}
static int Codec(Fixture *fixture,const char *name)
{
    if(strcmp(name,"journal-truncation")&&strcmp(name,"journal-mutations")&&strcmp(name,"journal-nul")&&strcmp(name,"journal-roundtrip"))return 2;
    UmiSetupMaintenancePlan *plan=NULL,*decoded=NULL;UmiSetupMaintenanceReport report={0};OK(Plan(fixture,UMI_SETUP_MAINTENANCE_UPDATE,0,&plan,&report));
    OK(SmJournalRead(plan->journal,plan->journalLength,&decoded));CHECK(!strcmp(decoded->summary.fingerprint,plan->summary.fingerprint));UmiSetupMaintenancePlanDestroy(decoded);decoded=NULL;
    if(!strcmp(name,"journal-truncation")){for(size_t n=0;n<plan->journalLength;++n){CHECK(SmJournalRead(plan->journal,n,&decoded)!=UMI_STATUS_OK);CHECK(decoded==NULL);}}
    else if(!strcmp(name,"journal-mutations")){uint32_t random=12345U;char *copy=malloc(plan->journalLength);CHECK(copy!=NULL);for(size_t i=0;i<6000U;++i){memcpy(copy,plan->journal,plan->journalLength);random=random*1664525U+1013904223U;size_t at=(size_t)random%plan->journalLength;random=random*1664525U+1013904223U;copy[at]=(char)(random>>24);UmiStatus status=SmJournalRead(copy,plan->journalLength,&decoded);if(status==UMI_STATUS_OK)CHECK(decoded!=NULL&&decoded->journalLength==plan->journalLength&&!memcmp(decoded->journal,copy,plan->journalLength));UmiSetupMaintenancePlanDestroy(decoded);decoded=NULL;}free(copy);}
    else if(!strcmp(name,"journal-nul")){char *copy=malloc(plan->journalLength);CHECK(copy!=NULL);memcpy(copy,plan->journal,plan->journalLength);copy[plan->journalLength/2U]=0;CHECK(SmJournalRead(copy,plan->journalLength,&decoded)==UMI_STATUS_PARSE_ERROR);free(copy);}
    else if(strcmp(name,"journal-roundtrip")){UmiSetupMaintenancePlanDestroy(plan);return 2;}
    UmiSetupMaintenancePlanDestroy(plan);return 0;
}
#ifndef _WIN32
static int Native(Fixture *fixture,const char *name)
{
    char live[UMI_SETUP_PATH_CAPACITY],other[UMI_SETUP_PATH_CAPACITY];CHECK(Path(fixture->root,"bin/notes.exe",live));CHECK(Path(fixture->base,"outside",other));
    UmiSetupMaintenancePlan *plan=NULL;UmiSetupMaintenanceReport report={0};
    if(!strcmp(name,"symlink-file")){CHECK(!Change(fixture->root,"bin/notes.exe",NULL));CHECK(!Put(fixture->base,"outside","Outside data\n"));CHECK(symlink(other,live)==0);CHECK(Plan(fixture,UMI_SETUP_MAINTENANCE_REPAIR,0,&plan,&report)==UMI_STATUS_PERMISSION_DENIED);CHECK(!Is(fixture->base,"outside","Outside data\n"));}
    else if(!strcmp(name,"hardlink-file")){CHECK(link(live,other)==0);CHECK(Plan(fixture,UMI_SETUP_MAINTENANCE_REPAIR,0,&plan,&report)==UMI_STATUS_PERMISSION_DENIED);}
    else if(!strcmp(name,"fifo-file")){CHECK(!Change(fixture->root,"bin/notes.exe",NULL));CHECK(mkfifo(live,0600)==0);CHECK(Plan(fixture,UMI_SETUP_MAINTENANCE_REPAIR,0,&plan,&report)==UMI_STATUS_PERMISSION_DENIED);}
    else if(!strcmp(name,"directory-file")){CHECK(!Change(fixture->root,"bin/notes.exe",NULL));CHECK(mkdir(live,0700)==0);CHECK(Plan(fixture,UMI_SETUP_MAINTENANCE_REPAIR,0,&plan,&report)==UMI_STATUS_PERMISSION_DENIED);}
    else if(!strcmp(name,"writable-root")){CHECK(chmod(fixture->root,0777)==0);CHECK(Plan(fixture,UMI_SETUP_MAINTENANCE_REPAIR,0,&plan,&report)==UMI_STATUS_PERMISSION_DENIED);}
    else if(!strcmp(name,"move-no-replace")){CHECK(!Put(fixture->base,"outside","Do not replace\n"));CHECK(SmMoveNew(live,other,NULL)==UMI_STATUS_ALREADY_EXISTS);CHECK(!Is(fixture->base,"outside","Do not replace\n"));CHECK(!Previous(fixture));}
    else if(!strcmp(name,"exclusive-lock")){char base[UMI_SETUP_PATH_CAPACITY],path[UMI_SETUP_PATH_CAPACITY];CHECK(Path(fixture->root,UMI_SETUP_MAINTENANCE_DIRECTORY,base));OK(ScMakeDirectory(base,NULL));CHECK(!Put(base,"owner.txt",SM_OWNER));CHECK(Path(base,"lock",path));void *lock=NULL;OK(SmLock(path,&lock,NULL));OK(Plan(fixture,UMI_SETUP_MAINTENANCE_UPDATE,0,&plan,&report));CHECK(Apply(plan,NULL,NULL,&report)==UMI_STATUS_BUSY);SmUnlock(lock);CHECK(!Previous(fixture));}
    else if(!strcmp(name,"process-crash")){pid_t child=fork();CHECK(child>=0);if(child==0){if(Plan(fixture,UMI_SETUP_MAINTENANCE_UPDATE,0,&plan,&report)!=UMI_STATUS_OK)_exit(88);StopAt stop={Phases[2],1,0,1};(void)Apply(plan,Stop,&stop,&report);_exit(89);}int code=0;CHECK(waitpid(child,&code,0)==child&&WIFEXITED(code)&&WEXITSTATUS(code)==79);OK(UmiSetupMaintenancePending(fixture->root,&report));char id[65];strcpy(id,report.transaction);OK(UmiSetupMaintenanceRestore(fixture->root,id,1,NULL,NULL,&report));CHECK(!Previous(fixture));}
    else return 2;
    UmiSetupMaintenancePlanDestroy(plan);return 0;
}
#endif
static int ReplaceText(const char *root,const char *name,const char *from,const char *to)
{
    char path[UMI_SETUP_PATH_CAPACITY],*text=NULL;size_t length=0U;CHECK(Path(root,name,path));
    OK(ScRead(path,100000U,&text,&length,NULL));ScText changed;ScTextInit(&changed);
    const char *cursor=text;size_t fromLength=strlen(from);CHECK(fromLength!=0U);
    for(;;){const char *found=strstr(cursor,from);if(!found){ScPrint(&changed,"%s",cursor);break;}
        size_t prefix=(size_t)(found-cursor);CHECK(prefix<(size_t)INT_MAX);ScPrint(&changed,"%.*s%s",(int)prefix,cursor,to);cursor=found+fromLength;}
    CHECK(changed.status==UMI_STATUS_OK);CHECK(!Change(root,name,changed.data));free(text);ScTextFree(&changed);return 0;
}
static int Extra(Fixture *fixture,const char *name)
{
    const char *names[]={"bound-list","stale-list","owned-config","missing-app","changed-entry","reserved-control","recover-completed","undo-local-edit","undo-newer","undo-selective","repair-title-change","cancel-before-pending","undo-interrupted","missing-backup","identity-tamper","null-contracts"};
    int found=0;for(size_t i=0U;i<sizeof names/sizeof names[0];++i)if(!strcmp(name,names[i]))found=1;if(!found)return 2;
    UmiSetupMaintenancePlan *plan=NULL;UmiSetupMaintenanceReport report={0};char id[65];
    if(!strcmp(name,"null-contracts")){
        CHECK(UmiSetupMaintenancePlanSummary(NULL)==NULL&&UmiSetupMaintenancePlanRow(NULL,0U)==NULL);
        CHECK(UmiSetupMaintenanceReceiptIdentity(NULL)==NULL);UmiSetupMaintenancePlanDestroy(NULL);
        CHECK(UmiSetupMaintenancePlanCreate(NULL,&plan,NULL,NULL,&report)==UMI_STATUS_INVALID_ARGUMENT&&plan==NULL);
        CHECK(UmiSetupMaintenanceApply(NULL,NULL,NULL,NULL,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiSetupMaintenancePending(NULL,&report)==UMI_STATUS_INVALID_ARGUMENT);return 0;
    }
    if(!strcmp(name,"bound-list")||!strcmp(name,"stale-list")||!strcmp(name,"owned-config")){
        UmiSetupBundle *installed=NULL;OK(UmiSetupInstalledBundleOpen(fixture->root,&installed,NULL));
        char receipt[65];strcpy(receipt,UmiSetupMaintenanceReceiptIdentity(installed));
        UmiSetupMaintenanceConfig config={fixture->root,fixture->newRelease,UMI_SETUP_MAINTENANCE_UPDATE,0U,receipt};
        if(!strcmp(name,"stale-list")){receipt[0]=receipt[0]=='a'?'b':'a';CHECK(UmiSetupMaintenancePlanCreate(&config,&plan,NULL,NULL,&report)==UMI_STATUS_INVALID_STATE);}
        else {OK(UmiSetupMaintenancePlanCreate(&config,&plan,NULL,NULL,&report));UmiSetupBundleDestroy(installed);installed=NULL;
            if(!strcmp(name,"owned-config")){memset(receipt,'x',64U);config.releaseRoot=NULL;config.installationRoot=NULL;}
            OK(Apply(plan,NULL,NULL,&report));CHECK(UmiSetupMaintenancePlanRow(plan,999U)==NULL);}
        UmiSetupBundleDestroy(installed);UmiSetupMaintenancePlanDestroy(plan);return 0;
    }
    if(!strcmp(name,"missing-app")){CHECK(!ReplaceText(fixture->newRelease,UMI_SETUP_CATALOGUE,"\tstock\t","\treplacement\t"));CHECK(Plan(fixture,UMI_SETUP_MAINTENANCE_UPDATE,0U,&plan,&report)==UMI_STATUS_INVALID_ARGUMENT);return 0;}
    if(!strcmp(name,"changed-entry")){CHECK(!ReplaceText(fixture->newRelease,UMI_SETUP_CATALOGUE,"bin/notes.exe","bin/renamed.exe"));CHECK(Plan(fixture,UMI_SETUP_MAINTENANCE_UPDATE,0U,&plan,&report)==UMI_STATUS_INVALID_ARGUMENT);return 0;}
    if(!strcmp(name,"reserved-control")){CHECK(!ReplaceText(fixture->newRelease,UMI_SETUP_CATALOGUE,"bin/notes.exe",UMI_SETUP_MAINTENANCE_PENDING));CHECK(Plan(fixture,UMI_SETUP_MAINTENANCE_UPDATE,0U,&plan,&report)!=UMI_STATUS_OK);return 0;}
    if(!strcmp(name,"repair-title-change")){CHECK(!ReplaceText(fixture->oldRelease,UMI_SETUP_CATALOGUE,"Umicom Notes","Umicom Notes revised label"));CHECK(!Change(fixture->root,"bin/notes.exe",NULL));OK(Plan(fixture,UMI_SETUP_MAINTENANCE_REPAIR,0U,&plan,&report));OK(Apply(plan,NULL,NULL,&report));CHECK(!Previous(fixture));UmiSetupMaintenancePlanDestroy(plan);return 0;}
    if(!strcmp(name,"undo-selective")){OK(Plan(fixture,UMI_SETUP_MAINTENANCE_REMOVE,1U,&plan,&report));OK(Apply(plan,NULL,NULL,&report));strcpy(id,report.transaction);OK(UmiSetupMaintenanceRestore(fixture->root,id,0,NULL,NULL,&report));CHECK(!Previous(fixture));UmiSetupMaintenancePlanDestroy(plan);return 0;}
    OK(Plan(fixture,UMI_SETUP_MAINTENANCE_UPDATE,0U,&plan,&report));
    if(!strcmp(name,"cancel-before-pending")){StopAt stop={"All replacements staged; installed files are still unchanged.",1U,0U,0};CHECK(Apply(plan,Stop,&stop,&report)==UMI_STATUS_CANCELLED);CHECK(!report.recoveryRequired&&report.io.outputCreated);CHECK(!Previous(fixture));UmiSetupMaintenancePlanDestroy(plan);return 0;}
    OK(Apply(plan,NULL,NULL,&report));strcpy(id,report.transaction);
    if(!strcmp(name,"recover-completed")){CHECK(UmiSetupMaintenanceRestore(fixture->root,id,1,NULL,NULL,&report)==UMI_STATUS_NOT_FOUND);CHECK(!Is(fixture->root,"bin/notes.exe","Notes binary generation two\n"));}
    else if(!strcmp(name,"undo-local-edit")){CHECK(!Change(fixture->root,"bin/notes.exe","A later local edit\n"));CHECK(UmiSetupMaintenanceRestore(fixture->root,id,0,NULL,NULL,&report)==UMI_STATUS_INVALID_STATE);CHECK(!Is(fixture->root,"bin/notes.exe","A later local edit\n"));CHECK(!report.recoveryRequired);}
    else if(!strcmp(name,"undo-newer")){
        UmiSetupMaintenancePlan *next=NULL;UmiSetupMaintenanceConfig config={fixture->root,fixture->oldRelease,UMI_SETUP_MAINTENANCE_UPDATE,0U,NULL};
        OK(UmiSetupMaintenancePlanCreate(&config,&next,NULL,NULL,&report));OK(Apply(next,NULL,NULL,&report));UmiSetupMaintenancePlanDestroy(next);
        CHECK(UmiSetupMaintenanceRestore(fixture->root,id,0,NULL,NULL,&report)==UMI_STATUS_INVALID_STATE);CHECK(!Previous(fixture));
    } else if(!strcmp(name,"undo-interrupted")){
        StopAt stop={"Previous file state restored.",2U,0U,0};CHECK(UmiSetupMaintenanceRestore(fixture->root,id,0,Stop,&stop,&report)==UMI_STATUS_CANCELLED);CHECK(report.recoveryRequired);
        OK(UmiSetupMaintenanceRestore(fixture->root,id,1,NULL,NULL,&report));CHECK(!Previous(fixture));
    } else if(!strcmp(name,"missing-backup")){
        char archive[UMI_SETUP_PATH_CAPACITY];strcpy(archive,report.archive);CHECK(!Change(archive,"old/0",NULL));CHECK(UmiSetupMaintenanceRestore(fixture->root,id,0,NULL,NULL,&report)==UMI_STATUS_INVALID_STATE);CHECK(!report.recoveryRequired);
    } else if(!strcmp(name,"identity-tamper")){
        char archive[UMI_SETUP_PATH_CAPACITY];strcpy(archive,report.archive);CHECK(!Change(archive,"identity.umi","modified identity\n"));CHECK(UmiSetupMaintenanceRestore(fixture->root,id,0,NULL,NULL,&report)==UMI_STATUS_PARSE_ERROR);CHECK(!report.recoveryRequired);
    }
    UmiSetupMaintenancePlanDestroy(plan);return 0;
}
typedef struct Inject {Fixture *fixture;UmiSetupMaintenanceReport *report;int collision;int failed;} Inject;
static int InjectFile(const UmiSetupReport *report,void *opaque)
{
    Inject *inject=opaque;
    if(!inject->collision&&!strcmp(report->detail,"All replacements staged; installed files are still unchanged."))
        inject->failed=Change(inject->report->archive,"new/0","Changed staged bytes\n");
    if(inject->collision&&!strcmp(report->detail,"Original file archived.")){
        inject->failed=Put(inject->fixture->root,"bin/common.dll","Unexpected new file\n");inject->collision=0;
    }
    return inject->failed;
}
static int Injected(Fixture *fixture,const char *name)
{
    if(strcmp(name,"staged-tamper")&&strcmp(name,"destination-race"))return 2;
    UmiSetupMaintenancePlan *plan=NULL;UmiSetupMaintenanceReport report={0};OK(Plan(fixture,UMI_SETUP_MAINTENANCE_UPDATE,0U,&plan,&report));
    int collision=!strcmp(name,"destination-race");Inject inject={fixture,&report,collision,0};
    CHECK(Apply(plan,InjectFile,&inject,&report)!=UMI_STATUS_OK&&inject.failed==0&&report.recoveryRequired);
    char id[65];strcpy(id,report.transaction);
    if(collision){CHECK(!Is(fixture->root,"bin/common.dll","Unexpected new file\n"));CHECK(UmiSetupMaintenanceRestore(fixture->root,id,1,NULL,NULL,&report)==UMI_STATUS_INVALID_STATE);
        /* The TEST alone removes the injected collision to exercise recovery;
         * production maintenance never overwrites or deletes that unknown file. */
        CHECK(!Change(fixture->root,"bin/common.dll",NULL));}
    OK(UmiSetupMaintenanceRestore(fixture->root,id,1,NULL,NULL,&report));CHECK(!Previous(fixture));UmiSetupMaintenancePlanDestroy(plan);return 0;
}
#ifndef _WIN32
static int RecoveryCrash(Fixture *fixture)
{
    UmiSetupMaintenancePlan *plan=NULL;UmiSetupMaintenanceReport report={0};OK(Plan(fixture,UMI_SETUP_MAINTENANCE_UPDATE,0U,&plan,&report));OK(Apply(plan,NULL,NULL,&report));char id[65];strcpy(id,report.transaction);UmiSetupMaintenancePlanDestroy(plan);
    pid_t child=fork();CHECK(child>=0);if(child==0){StopAt stop={"Previous file state restored.",2U,0U,1};(void)UmiSetupMaintenanceRestore(fixture->root,id,0,Stop,&stop,&report);_exit(88);}
    int code;CHECK(waitpid(child,&code,0)==child&&WIFEXITED(code)&&WEXITSTATUS(code)==79);OK(UmiSetupMaintenanceRestore(fixture->root,id,1,NULL,NULL,&report));CHECK(!Previous(fixture));return 0;
}
#endif
int main(int argc,char **argv)
{
    if(argc!=3)return 2;
    Fixture fixture;
    if(Init(&fixture,argv[2],argv[1]))return 1;
    if(!strcmp(argv[1],"public-example")){OK(UmiMaintenanceReviewExample(fixture.root,fixture.newRelease,stdout));CHECK(!Previous(&fixture));return 0;}
    if(!strcmp(argv[1],"cli-pending-none")){
        char *args[]={"umicom-maintain","pending","--root",fixture.root};CHECK(UmiSetupMaintenanceMain(4,args)==0);CHECK(!Previous(&fixture));return 0;
    }
    if(!strcmp(argv[1],"cli-pending-present")){
        UmiSetupMaintenancePlan *plan=NULL;UmiSetupMaintenanceReport report={0};OK(Plan(&fixture,UMI_SETUP_MAINTENANCE_UPDATE,0U,&plan,&report));
        StopAt stop={Phases[0],1U,0U,0};CHECK(Apply(plan,Stop,&stop,&report)==UMI_STATUS_CANCELLED);
        char *args[]={"umicom-maintain","pending","--root",fixture.root};CHECK(UmiSetupMaintenanceMain(4,args)==3);UmiSetupMaintenancePlanDestroy(plan);return 0;
    }
    int status=Normal(&fixture,argv[1]);if(status!=2)return status;
    status=Rejection(&fixture,argv[1]);if(status!=2)return status;
    if(!strncmp(argv[1],"cancel-",7)){unsigned phase=(unsigned)strtoul(argv[1]+7,NULL,10);CHECK(phase<5U);return Interrupted(&fixture,phase,0,0);}
    if(!strcmp(argv[1],"recovery-interrupted"))return Interrupted(&fixture,3,1,0);
    if(!strcmp(argv[1],"repeat-after-recovery"))return Interrupted(&fixture,3,0,1);
    status=RecoveryConflict(&fixture,argv[1]);if(status!=2)return status;
    status=Codec(&fixture,argv[1]);if(status!=2)return status;
    status=Extra(&fixture,argv[1]);if(status!=2)return status;
    status=Injected(&fixture,argv[1]);if(status!=2)return status;
#ifndef _WIN32
    if(!strcmp(argv[1],"recovery-process-crash"))return RecoveryCrash(&fixture);
    status=Native(&fixture,argv[1]);if(status!=2)return status;
#endif
    return 2;
}
