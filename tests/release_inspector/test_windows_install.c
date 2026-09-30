/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_inspector/test_windows_install.c
 * PURPOSE:
 *   Actual Windows acceptance, not a PE fixture. Build and run only on Windows. The test
 *   installs the Notes laboratory into a new Unicode/spaced temporary path, strips
 *   development paths for its child, and runs from an unrelated CWD. It creates no shortcuts
 *   and never starts any financial application. Test directories are retained for inspection
 *   and never reused or reset.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Actual Windows acceptance, not a PE fixture. Build and run only on Windows.
 * The test installs the Notes laboratory into a new Unicode/spaced temporary
 * path, strips development paths for its child, and runs from an unrelated CWD.
 * It creates no shortcuts and never starts any financial application.
 * Test directories are retained for inspection and never reused or reset.
 *---------------------------------------------------------------------------*/
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wchar.h>
#include <limits.h>
#include "test_support.h"
#include "umicom/platform/process.h"
static int Utf8(const wchar_t *source,char *out,size_t capacity)
{
    if(capacity>(size_t)INT_MAX)return 0;
    return WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,source,-1,out,(int)capacity,NULL,NULL)>0;
}
static int Copy(const char *source,const char *root,const char *name)
{
    char target[UMI_SETUP_PATH_CAPACITY],hash[65];uint64_t bytes=0;
    UmiSetupReport report={0};
    OK(ScJoin(root,name,target));OK(ScDigest(source,hash,&bytes,NULL,NULL,&report));
    OK(ScCopy(source,target,hash,bytes,NULL,NULL,&report));return 0;
}
static int Run(const char *program,const char *directory,const char *path,int expected)
{
    const char *arguments[]={"--smoke-test"};
    UmiEnvironmentVariable environment[]={ {"PATH",path} };
    UmiProcessRequest request={0};UmiProcessResult *result=calloc(1,sizeof *result);CHECK(result);
    request.program=program;request.arguments=arguments;request.argument_count=1;
    request.environment=environment;request.environment_count=1;request.working_directory=directory;
    request.capture_stdout=1;request.capture_stderr=1;request.timeout_ms=10000;
    request.window_mode=UMI_PROCESS_WINDOW_HIDDEN;
    /* This single-threaded test owns its error mode for the launch. Windows
     * children inherit it, avoiding unattended missing-DLL error dialogs. */
    UINT previous=SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX|SEM_NOOPENFILEERRORBOX);
    UmiStatus status=UmiProcessExecuteWithLifetime(&request,UMI_PROCESS_LIFETIME_TREE,NULL,NULL,NULL,result);
    SetErrorMode(previous);
    int ok=expected==0?(status==UMI_STATUS_OK && result->launched && result->exit_code==0):
        expected==13?(result->launched && result->exit_code==13):
        (status!=UMI_STATUS_OK && !result->timed_out);
    if(!ok)fprintf(stderr,"startup status=%d launched=%d exit=%d timeout=%d\n%s\n",(int)status,result->launched,result->exit_code,result->timed_out,result->output);
    free(result);CHECK(ok);return 0;
}
int wmain(int argc,wchar_t **argv)
{
    CHECK(argc==5);
    char mode[64],exe[UMI_SETUP_PATH_CAPACITY],dll[UMI_SETUP_PATH_CAPACITY],resource[UMI_SETUP_PATH_CAPACITY];
    CHECK(Utf8(argv[1],mode,sizeof mode));CHECK(Utf8(argv[2],exe,sizeof exe));
    CHECK(Utf8(argv[3],dll,sizeof dll));CHECK(Utf8(argv[4],resource,sizeof resource));
    wchar_t temporary[2048],system[2048];char parent[UMI_SETUP_PATH_CAPACITY],root[UMI_SETUP_PATH_CAPACITY];
    DWORD n=GetTempPathW(2048,temporary);CHECK(n && n<2048);
    CHECK(Utf8(temporary,parent,sizeof parent));size_t length=strlen(parent);
    while(length && (parent[length-1U]=='\\'||parent[length-1U]=='/'))parent[--length]=0;
    char leaf[128];(void)snprintf(leaf,sizeof leaf,"Umicom release caf\xc3\xa9 %lu %llu",(unsigned long)GetCurrentProcessId(),(unsigned long long)GetTickCount64());
    OK(ScJoin(parent,leaf,root));UmiSetupReport report={0};OK(ScMakeDirectory(root,&report));
    char release[UMI_SETUP_PATH_CAPACITY],install[UMI_SETUP_PATH_CAPACITY],unrelated[UMI_SETUP_PATH_CAPACITY];
    OK(ScJoin(root,"offline release",release));OK(ScJoin(root,"installed application",install));
    OK(ScJoin(root,"unrelated working folder",unrelated));OK(ScMakeDirectory(unrelated,&report));
    n=GetSystemDirectoryW(system,2048);CHECK(n && n<2048);
    char cleanPath[UMI_SETUP_PATH_CAPACITY];CHECK(Utf8(system,cleanPath,sizeof cleanPath));
    char exeHash[65],dllHash[65],dataHash[65];uint64_t bytes=0;
    OK(ScDigest(exe,exeHash,&bytes,NULL,NULL,&report));OK(ScDigest(dll,dllHash,&bytes,NULL,NULL,&report));
    OK(ScDigest(resource,dataHash,&bytes,NULL,NULL,&report));
    char runtime[UMI_SETUP_PATH_CAPACITY],list[UMI_SETUP_PATH_CAPACITY];
    OK(ScJoin(root,"notes.runtime.cmake",runtime));OK(ScJoin(root,"inputs.tsv",list));
    ScText text;ScTextInit(&text);
    ScPrint(&text,"set(UMI_REPORT_EXECUTABLE [==[%s]==])\nset(UMI_REPORT_EXECUTABLE_SHA256 [==[%s]==])\n"
        "set(UMI_REPORT_TARGET [==[notes]==])\nset(UMI_REPORT_PRODUCT [==[Umicom Notes release laboratory]==])\n"
        "set(UMI_REPORT_FILES [==[bin/umicom-release-notes-model.dll|%s|%s;bin/example-note.txt|%s|%s]==])\n",
        exe,exeHash,dll,dllHash,resource,dataHash);
    OK(text.status);OK(ScWriteNew(runtime,text.data,text.size,&report));ScTextFree(&text);ScTextInit(&text);
    ScPrint(&text,"UMICOM_SETUP_INPUT\t1\napp\tnotes\tUmicom Notes release laboratory\t%s\n",runtime);
    OK(text.status);OK(ScWriteNew(list,text.data,text.size,&report));ScTextFree(&text);
    OK(UmiSetupPack(list,release,NULL,NULL,NULL,&report));
    UmiSetupBundle *bundle=NULL;OK(UmiSetupBundleOpen(release,&bundle,&report));
    UmiReleaseInspection checked={0};OK(UmiReleaseInspectBundle(bundle,1,NULL,NULL,&checked));
    CHECK(!checked.runtimeTested && checked.complete && !checked.issues);
    char fingerprint[65];OK(UmiSetupReview(bundle,1,install,fingerprint,NULL,NULL,&report));
    OK(UmiSetupInstall(bundle,1,install,fingerprint,NULL,NULL,&report));UmiSetupBundleDestroy(bundle);
    OK(UmiReleaseInspectInstallation(install,NULL,NULL,&checked));
    char program[UMI_SETUP_PATH_CAPACITY];OK(ScJoin(install,"bin/umicom-release-notes.exe",program));
    CHECK(!Run(program,unrelated,cleanPath,0));
    if(strcmp(mode,"installed_startup")) {
        char incomplete[UMI_SETUP_PATH_CAPACITY];OK(ScJoin(root,"incomplete copy",incomplete));
        OK(ScMakeDirectory(incomplete,&report));CHECK(!Copy(exe,incomplete,"umicom-release-notes.exe"));
        int missingDll=!strcmp(mode,"missing_dll");CHECK(missingDll || !strcmp(mode,"missing_resource"));
        if(missingDll)CHECK(!Copy(resource,incomplete,"example-note.txt"));
        else CHECK(!Copy(dll,incomplete,"umicom-release-notes-model.dll"));
        OK(ScJoin(incomplete,"umicom-release-notes.exe",program));
        CHECK(!Run(program,unrelated,cleanPath,missingDll?1:13));
    }
    printf("PASS Windows installation acceptance: %s\nEvidence directory: %s\n",mode,root);
    return 0;
}
