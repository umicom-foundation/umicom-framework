/*-----------------------------------------------------------------------------
 * Umicom Framework tests | Sammy Hegab, Umicom Foundation | MIT
 * Synthetic PE/package fixtures exercise inspection, not Windows loading.
 * Tests create only uniquely named temporary directories, retained on failure.
 *---------------------------------------------------------------------------*/
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "test_support.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif

typedef struct Fixture {
    char parent[UMI_SETUP_PATH_CAPACITY];
    char root[UMI_SETUP_PATH_CAPACITY];
    char destination[UMI_SETUP_PATH_CAPACITY];
} Fixture;
static int Temporary(Fixture *f)
{
    memset(f,0,sizeof *f);
#ifdef _WIN32
    wchar_t temp[2048];char base[UMI_SETUP_PATH_CAPACITY];
    DWORD n=GetTempPathW(2048,temp);CHECK(n && n<2048);
    CHECK(WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,temp,-1,base,sizeof base,NULL,NULL)>0);
    size_t length=strlen(base);while(length && (base[length-1U]=='/'||base[length-1U]=='\\'))base[--length]=0;
    char name[128];(void)snprintf(name,sizeof name,"umicom-release-%lu-%llu",(unsigned long)GetCurrentProcessId(),(unsigned long long)GetTickCount64());
    OK(ScJoin(base,name,f->parent));UmiSetupReport r={0};OK(ScMakeDirectory(f->parent,&r));
#else
    strcpy(f->parent,"/tmp/umicom-release-XXXXXX");CHECK(mkdtemp(f->parent)!=NULL);
#endif
    OK(ScJoin(f->parent,"release",f->root));
    OK(ScJoin(f->parent,"installed-caf\xc3\xa9",f->destination));
    return 0;
}
static int AddFile(UmiSetupBundle *b,const char *root,const char *relative,unsigned owner,const void *data,size_t size)
{
    ScFile file={0};file.owner=owner;file.bytes=size;
    CHECK(strlen(relative)<sizeof file.relative);strcpy(file.relative,relative);
    OK(UmiNativeSha256Buffer(data,size,file.hash));
    OK(ScAddFile(b,&file));UmiSetupReport report={0};
    char payload[UMI_SETUP_PATH_CAPACITY],path[UMI_SETUP_PATH_CAPACITY];
    OK(ScJoin(root,"payload",payload));OK(ScParents(payload,relative,&report));
    OK(ScJoin(payload,relative,path));OK(ScWriteNew(path,data,size,&report));return 0;
}
static int Build(Fixture *f,const char *name)
{
    CHECK(!Temporary(f));UmiSetupReport report={0};
    OK(ScMakeDirectory(f->root,&report));char path[UMI_SETUP_PATH_CAPACITY];
    OK(ScJoin(f->root,"payload",path));OK(ScMakeDirectory(path,&report));
    UmiSetupBundle *b=calloc(1,sizeof *b);CHECK(b);
    b->appCount=2U;strcpy(b->apps[0].id,"notes");strcpy(b->apps[0].title,"Umicom Notes practice");
    strcpy(b->apps[0].entry,!strcmp(name,"release.entry_nested")?"apps/notes.exe":"bin/notes.exe");
    strcpy(b->apps[1].id,"review");strcpy(b->apps[1].title,"Umicom Review practice");strcpy(b->apps[1].entry,"bin/review.exe");
    const char *dependency="umicom-practice-storage.dll";
    int gtk=!strcmp(name,"release.resource_missing")||!strcmp(name,"release.resource_present");
    if(gtk)dependency="libgtk-4-1.dll";
    int sourceView=!strcmp(name,"release.sourceview_missing")||!strcmp(name,"release.sourceview_present");
    if(sourceView)dependency="GTKSourceView-5-0.dll";
    if(!strcmp(name,"release.private_shadow"))dependency="kernel32.dll";
    const char *request=!strcmp(name,"release.unknown_system")?"madeup-system.dll":dependency;
    if(!strcmp(name,"release.system_deferred"))request="api-ms-win-core-file-l1-1-0.dll";
    unsigned char bytes[TEST_PE_BYTES];
    TestPe(bytes,0,request,!strcmp(name,"release.delay_missing")?"optional-study.dll":NULL);
    if(!strcmp(name,"release.wrong_machine"))TestWord(bytes+132,0xaa64U);
    if(!strcmp(name,"release.invalid_image"))bytes[0]='!';
    if(!strcmp(name,"release.bad_type"))TestWord(bytes+150,0x2022U);
    CHECK(!AddFile(b,f->root,b->apps[0].entry,0U,bytes,sizeof bytes));
    TestPe(bytes,0,"kernel32.dll",NULL);
    CHECK(!AddFile(b,f->root,b->apps[1].entry,1U,bytes,sizeof bytes));
    if(strcmp(name,"release.missing_dll")) {
        TestPe(bytes,1,"kernel32.dll",NULL);
        char relative[UMI_SETUP_RELATIVE_CAPACITY];
        (void)snprintf(relative,sizeof relative,"bin/%s",dependency);
        CHECK(!AddFile(b,f->root,relative,!strcmp(name,"release.unselected_dependency")?1U:SC_SHARED,bytes,sizeof bytes));
    }
    if(!strcmp(name,"release.module_nested")) {
        TestPe(bytes,1,dependency,NULL);
        CHECK(!AddFile(b,f->root,"lib/gio/modules/practice.dll",SC_SHARED,bytes,sizeof bytes));
    }
    if(!strcmp(name,"release.resource_present")) {
        const char *resources[]={"share/umicom/runtime/deployment.marker","share/glib-2.0/schemas/gschemas.compiled",
            "bin/branding/umicom-icon.svg","bin/branding/umicom-logo-on-dark.svg"};
        for(size_t i=0U;i<4U;++i)CHECK(!AddFile(b,f->root,resources[i],SC_SHARED,"synthetic resource presence",27U));
    }
    if(!strcmp(name,"release.sourceview_present"))
        CHECK(!AddFile(b,f->root,"share/gtksourceview-5/language-specs/language2.rng",SC_SHARED,"fixture schema",14U));
    if(!strncmp(name,"bootstrap.",10U)) {
        TestPe(bytes,0,!strcmp(name,"bootstrap.private_import")?dependency:"kernel32.dll",NULL);
        CHECK(!AddFile(b,f->root,"bin/umicom-setup-centre.exe",SC_SHARED,bytes,sizeof bytes));
        if(strcmp(name,"bootstrap.missing")) {
            OK(ScJoin(f->root,"Umicom-Setup.exe",path));
            if(!strcmp(name,"bootstrap.changed"))bytes[120]^=1U;
            OK(ScWriteNew(path,bytes,sizeof bytes,&report));
        }
    }
    ScText encoded;ScTextInit(&encoded);OK(ScEncode(b,3U,0,&encoded));
    OK(ScJoin(f->root,UMI_SETUP_CATALOGUE,path));OK(ScWriteNew(path,encoded.data,encoded.size,&report));
    ScTextFree(&encoded);UmiSetupBundleDestroy(b);return 0;
}
static int Change(const char *path)
{
#ifdef _WIN32
    wchar_t w[UMI_SETUP_PATH_CAPACITY];
    CHECK(MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,path,-1,w,UMI_SETUP_PATH_CAPACITY)>0);
    FILE *f=_wfopen(w,L"ab");
#else
    FILE *f=fopen(path,"ab");
#endif
    CHECK(f);CHECK(fputc('x',f)!=EOF);CHECK(fclose(f)==0);return 0;
}
static int Delete(const char *path)
{
#ifdef _WIN32
    wchar_t w[UMI_SETUP_PATH_CAPACITY];
    CHECK(MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,path,-1,w,UMI_SETUP_PATH_CAPACITY)>0);
    CHECK(DeleteFileW(w));
#else
    CHECK(remove(path)==0);
#endif
    return 0;
}
typedef struct Observed {size_t files,issues,privateCount,systemCount;int stop;} Observed;
static int Observer(const UmiReleaseObservation *item,void *opaque)
{
    Observed *o=opaque;
    if(o->stop==1)return 1;
    if(!item->path[0])return 0;
    if(item->kind==UMI_RELEASE_FILE_CHECKED)++o->files;
    if(item->kind==UMI_RELEASE_ISSUE)++o->issues;
    if(item->kind==UMI_RELEASE_PRIVATE_IMPORT)++o->privateCount;
    if(item->kind==UMI_RELEASE_SYSTEM_IMPORT)++o->systemCount;
    return o->stop==2 && o->files>=2U;
}
int TestFilesCase(const char *name)
{
    if(!strcmp(name,"summary.capacity")) {
        UmiReleaseInspection r={0};r.complete=1;char out[512],tiny[1];
        OK(UmiReleaseInspectionSummary(&r,out,sizeof out));CHECK(strstr(out,"No application was launched"));
        CHECK(UmiReleaseInspectionSummary(&r,tiny,1U)==UMI_STATUS_CAPACITY_EXCEEDED && tiny[0]==0);
        CHECK(UmiReleaseInspectionSummary(NULL,out,sizeof out)==UMI_STATUS_INVALID_ARGUMENT);return 0;
    }
    Fixture f;CHECK(!Build(&f,name));UmiSetupReport io={0};UmiSetupBundle *b=NULL;
    OK(UmiSetupBundleOpen(f.root,&b,&io));char path[UMI_SETUP_PATH_CAPACITY];
    uint64_t selected=!strcmp(name,"release.unselected_dependency")?1U:3U;
    if(!strcmp(name,"release.changed")) {OK(ScJoin(f.root,"payload/bin/notes.exe",path));CHECK(!Change(path));}
    if(!strcmp(name,"release.catalogue_changed")) {OK(ScJoin(f.root,UMI_SETUP_CATALOGUE,path));CHECK(!Change(path));}
    if(!strcmp(name,"release.unselected_missing")) {
        OK(ScJoin(f.root,"payload/bin/review.exe",path));CHECK(!Delete(path));selected=1U;
    }
    UmiReleaseInspection r={0};Observed seen={0};
    if(!strcmp(name,"release.cancel"))seen.stop=1;
    if(!strcmp(name,"release.cancel_mid"))seen.stop=2;
    if(!strncmp(name,"installation.",13U)) {
        char plan[65];OK(UmiSetupReview(b,1U,f.destination,plan,NULL,NULL,&io));
        OK(UmiSetupInstall(b,1U,f.destination,plan,NULL,NULL,&io));
        if(!strcmp(name,"installation.tamper")) {OK(ScJoin(f.destination,"bin/notes.exe",path));CHECK(!Change(path));}
        if(!strcmp(name,"installation.personal_files")) {
            OK(ScJoin(f.destination,"my-notes.txt",path));OK(ScWriteNew(path,"my own work",11U,&io));
        }
        if(!strcmp(name,"installation.missing_receipt")) {OK(ScJoin(f.destination,UMI_SETUP_RECEIPT,path));CHECK(!Delete(path));}
        UmiStatus status=UmiReleaseInspectInstallation(f.destination,Observer,&seen,&r);
        int fail=!strcmp(name,"installation.tamper")||!strcmp(name,"installation.missing_receipt");
        CHECK((status!=UMI_STATUS_OK)==fail);
        if(!fail)CHECK(r.complete && !r.issues && r.imagesChecked==2U && !r.runtimeTested);
        if(!strcmp(name,"installation.personal_files"))OK(ScFileRegular(path,0,&io));
    } else if(!strncmp(name,"cli.",4U)) {
        OK(ScJoin(f.parent,"observations.json",path));
        char *args[]={"umicom-release-inspect","release","--root",f.root,"--apps","notes","--output",path};
        CHECK(UmiReleaseInspectorMain(8,args)==0);
        char *data=NULL;size_t size=0U;OK(ScRead(path,UMI_SETUP_MANIFEST_LIMIT,&data,&size,&io));
        CHECK(size && strstr(data,"\"runtime_tested\":false") && strstr(data,"\"complete\":true"));
        CHECK(strstr(data,"\"system-deferred\"") && strstr(data,"\"private-import\""));free(data);
        if(!strcmp(name,"cli.repeat_report")) {
            char before[65],after[65];uint64_t bytes;
            OK(ScDigest(path,before,&bytes,NULL,NULL,&io));CHECK(UmiReleaseInspectorMain(8,args)!=0);
            OK(ScDigest(path,after,&bytes,NULL,NULL,&io));CHECK(!strcmp(before,after));
        } else if(!strcmp(name,"cli.malformed_options")) {
            char *bad[]={"inspect","release","--root",f.root,"--root",f.root};CHECK(UmiReleaseInspectorMain(6,bad)==2);
            args[5]="notes,notes";args[6]="--output";OK(ScJoin(f.parent,"invalid-selection.json",path));
            CHECK(UmiReleaseInspectorMain(8,args)!=0);
        }
    } else {
        UmiStatus status=UmiReleaseInspectBundle(b,selected,Observer,&seen,&r);
        if(seen.stop)CHECK(status==UMI_STATUS_CANCELLED && !r.complete);
        else if(!strcmp(name,"release.sourceview_present")||!strcmp(name,"bootstrap.clean")||!strcmp(name,"release.clean")||!strcmp(name,"release.no_outputs")||
                !strcmp(name,"release.resource_present")||!strcmp(name,"release.module_nested")||
                !strcmp(name,"release.system_deferred")||!strcmp(name,"release.unselected_missing")) {
            OK(status);CHECK(r.complete && !r.issues && !r.runtimeTested);
            CHECK(r.filesChecked==seen.files && r.systemImports==seen.systemCount);
            if(!strcmp(name,"release.clean"))CHECK(r.filesChecked==3U && r.imagesChecked==3U && r.privateImports==1U && r.systemImports==2U);
            if(!strcmp(name,"release.resource_present"))CHECK(r.resourcesChecked==4U);
            if(!strcmp(name,"release.sourceview_present"))CHECK(r.resourcesChecked==1U);
            if(!strcmp(name,"release.module_nested"))CHECK(r.imagesChecked==4U && r.privateImports==2U);
            if(!strcmp(name,"release.no_outputs"))CHECK(ScFileRegular(f.destination,1,&io)==UMI_STATUS_NOT_FOUND);
        } else {
            CHECK(status!=UMI_STATUS_OK && r.issues && seen.issues && r.firstIssue[0]);
            if(!strcmp(name,"release.delay_missing"))CHECK(r.delayedImports==1U);
            if(!strcmp(name,"release.catalogue_changed"))CHECK(!r.complete && r.filesChecked==0U);
        }
    }
    UmiSetupBundleDestroy(b);return 0;
}
