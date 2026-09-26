/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native pack -> static PE inspection -> install -> update -> inspection -> undo.
 * Images are linker-produced FORMAT fixtures and are NEVER executed here.
 *---------------------------------------------------------------------------*/
#define _POSIX_C_SOURCE 200809L
#include "maintenance_internal.h"
#include "umicom/release_inspector/inspection.h"
#include <unistd.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); return 1; } } while (0)
#define OK(x) CHECK((x)==UMI_STATUS_OK)
static int Prepare(const char *root,const char *name,const char *app,const char *dll,
    const char *bootstrap,char release[UMI_SETUP_PATH_CAPACITY])
{
    char inputRoot[UMI_SETUP_PATH_CAPACITY],executable[UMI_SETUP_PATH_CAPACITY];
    char report[UMI_SETUP_PATH_CAPACITY],listPath[UMI_SETUP_PATH_CAPACITY],hash[65],dllHash[65];
    uint64_t bytes;OK(ScJoin(root,name,inputRoot));OK(ScMakeDirectory(inputRoot,NULL));
    OK(ScJoin(inputRoot,"notes.exe",executable));OK(ScDigest(app,hash,&bytes,NULL,NULL,NULL));
    OK(ScCopy(app,executable,hash,bytes,NULL,NULL,NULL));OK(ScDigest(dll,dllHash,&bytes,NULL,NULL,NULL));
    OK(ScJoin(inputRoot,"notes.runtime.cmake",report));ScText content;ScTextInit(&content);
    ScPrint(&content,"set(UMI_REPORT_EXECUTABLE [==[%s]==])\nset(UMI_REPORT_EXECUTABLE_SHA256 [==[%s]==])\n"
        "set(UMI_REPORT_TARGET [==[notes]==])\nset(UMI_REPORT_PRODUCT [==[Umicom Notes]==])\n"
        "set(UMI_REPORT_FILES [==[bin/umicom-practice-storage.dll|%s|%s]==])\n",executable,hash,dll,dllHash);
    OK(content.status);OK(ScWriteNew(report,content.data,content.size,NULL));ScTextFree(&content);
    ScText list;ScTextInit(&list);ScPrint(&list,"UMICOM_SETUP_INPUT\t1\napp\tnotes\tUmicom Notes\t%s\n",report);
    OK(ScJoin(inputRoot,"input.tsv",listPath));OK(list.status);OK(ScWriteNew(listPath,list.data,list.size,NULL));ScTextFree(&list);
    OK(ScJoin(inputRoot,"release",release));OK(UmiSetupPack(listPath,release,bootstrap,NULL,NULL,NULL));
    UmiSetupBundle *bundle=NULL;OK(UmiSetupBundleOpen(release,&bundle,NULL));UmiReleaseInspection inspection={0};
    OK(UmiReleaseInspectBundle(bundle,1U,NULL,NULL,&inspection));CHECK(inspection.complete&&!inspection.issues&&!inspection.runtimeTested);
    UmiSetupBundleDestroy(bundle);return 0;
}
int main(int argc,char **argv)
{
    if(argc!=5)return 2;
    char root[]="/tmp/umicom-maintenance-pe-XXXXXX";CHECK(mkdtemp(root)!=NULL);
    char first[UMI_SETUP_PATH_CAPACITY],next[UMI_SETUP_PATH_CAPACITY],installed[UMI_SETUP_PATH_CAPACITY];
    CHECK(!Prepare(root,"first",argv[1],argv[3],argv[4],first));CHECK(!Prepare(root,"next",argv[2],argv[3],argv[4],next));
    OK(ScJoin(root,"installed cafe\xc3\xa9",installed));UmiSetupBundle *bundle=NULL;char hash[65];
    OK(UmiSetupBundleOpen(first,&bundle,NULL));OK(UmiSetupReview(bundle,1U,installed,hash,NULL,NULL,NULL));
    OK(UmiSetupInstall(bundle,1U,installed,hash,NULL,NULL,NULL));UmiSetupBundleDestroy(bundle);
    UmiSetupMaintenanceConfig config={installed,next,UMI_SETUP_MAINTENANCE_UPDATE,0U,NULL};
    UmiSetupMaintenancePlan *plan=NULL;UmiSetupMaintenanceReport report={0};
    OK(UmiSetupMaintenancePlanCreate(&config,&plan,NULL,NULL,&report));
    CHECK(UmiSetupMaintenancePlanSummary(plan)->filesToWrite==1U);
    strcpy(hash,UmiSetupMaintenancePlanSummary(plan)->fingerprint);OK(UmiSetupMaintenanceApply(plan,hash,NULL,NULL,&report));
    UmiReleaseInspection inspection={0};OK(UmiReleaseInspectInstallation(installed,NULL,NULL,&inspection));
    CHECK(inspection.complete&&!inspection.runtimeTested&&inspection.privateImports==1U);
    char transaction[65];strcpy(transaction,report.transaction);OK(UmiSetupMaintenanceRestore(installed,transaction,0,NULL,NULL,&report));
    OK(UmiReleaseInspectInstallation(installed,NULL,NULL,&inspection));CHECK(!inspection.runtimeTested);
    UmiSetupMaintenancePlanDestroy(plan);
    puts("PASS native package, inspection, update and undo with real linker-produced PE records. No image was executed.");
    puts(root);return 0;
}
