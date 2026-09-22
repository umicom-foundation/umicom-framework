/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/native_process/test_native_process.c
 * PURPOSE: Verify actual argv, ownership, cancellation and supervisor behaviour.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "umicom/platform/process.h"
#include "umicom/platform/process_supervisor.h"
#include "umicom/platform/threading.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wchar.h>
#else
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#ifdef __linux__
#include <sys/prctl.h>
#endif
#endif

#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); failed=1; goto cleanup; } } while(0)

typedef struct Observation {
    UmiCancellationToken *token;
    size_t bytes;
    size_t callbacks;
    int stopOnReady;
    int stopOnOutput;
} Observation;

static void Observe(const UmiProcessResult *result, void *context)
{
    Observation *state = context;
    ++state->callbacks;
    if ((state->stopOnReady && strstr(result->output, "TREE_READY") != NULL) ||
        (state->stopOnOutput && state->bytes >= 1024U * 1024U))
        umi_cancellation_token_request(state->token);
}
static void Raw(const char *bytes, size_t length, void *context)
{
    Observation *state = context;
    (void)bytes;
    state->bytes += length;
}

static int MemberStopped(unsigned long id)
{
#ifdef _WIN32
    HANDLE handle = OpenProcess(SYNCHRONIZE, FALSE, (DWORD)id);
    if (handle == NULL) return GetLastError() == ERROR_INVALID_PARAMETER;
    int stopped = WaitForSingleObject(handle, 0U) == WAIT_OBJECT_0;
    (void)CloseHandle(handle);
    return stopped;
#else
    int status;
    if (waitpid((pid_t)id, &status, WNOHANG) == (pid_t)id) return 1;
    if (kill((pid_t)id, 0) < 0 && errno == ESRCH) return 1;
#ifdef __linux__
    char path[128], line[1024];
    (void)snprintf(path,sizeof(path),"/proc/%lu/stat",id);
    FILE *file = fopen(path,"r");
    if (file != NULL) {
        char *read = fgets(line,sizeof(line),file); fclose(file);
        char *end = read != NULL ? strrchr(line,')') : NULL;
        if (end != NULL && end[1]==' ' && end[2]=='Z') return 1;
    }
#endif
    return 0;
#endif
}

static size_t Members(const char *output, unsigned long ids[8])
{
    size_t count = 0U;
    const char *cursor = output;
    while ((cursor = strstr(cursor,"MEMBER=")) != NULL && count < 8U) {
        char *end = NULL;
        unsigned long value = strtoul(cursor + 7U,&end,10);
        if (value != 0U) ids[count++] = value;
        cursor = end != NULL ? end : cursor + 7U;
    }
    return count;
}

/* Fixtures name only the children they created; no name-based system-wide kill. */
static void CleanupMembers(const char *output)
{
    unsigned long ids[8]; size_t count=Members(output,ids);
    for (size_t i=0U;i<count;++i) {
        if (!MemberStopped(ids[i])) {
#ifdef _WIN32
            HANDLE process=OpenProcess(PROCESS_TERMINATE|SYNCHRONIZE,FALSE,(DWORD)ids[i]);
            if(process!=NULL){(void)TerminateProcess(process,125U);(void)WaitForSingleObject(process,2000U);CloseHandle(process);}
#else
            (void)kill((pid_t)ids[i],SIGKILL);
#endif
        }
#ifndef _WIN32
        int status; (void)waitpid((pid_t)ids[i],&status,WNOHANG);
#endif
    }
}

static int AllMembersStopped(const char *output)
{
    unsigned long ids[8]; size_t count=Members(output,ids);
    if (count < 3U) return 0;
    for(unsigned attempt=0U;attempt<200U;++attempt){
        int done=1;
        for(size_t i=0U;i<count;++i) if(!MemberStopped(ids[i])) done=0;
        if(done)return 1;
        umi_thread_sleep_ms(5U);
    }
    return 0;
}

static int MakeDirectory(const char *path)
{
#ifdef _WIN32
    wchar_t wide[32768];
    if(MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,path,-1,wide,32768)<=0)return 0;
    return CreateDirectoryW(wide,NULL)!=0;
#else
    return mkdir(path,0700)==0;
#endif
}

static int CopyExecutable(const char *source,const char *destination)
{
#ifdef _WIN32
    wchar_t a[32768],b[32768];
    if(MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,source,-1,a,32768)<=0 ||
       MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,destination,-1,b,32768)<=0)return 0;
    return CopyFileW(a,b,TRUE)!=0;
#else
    FILE *in=fopen(source,"rb"); if(in==NULL)return 0;
    FILE *out=fopen(destination,"wb");if(out==NULL){fclose(in);return 0;}
    char bytes[8192];size_t count;int good=1;
    while((count=fread(bytes,1U,sizeof(bytes),in))!=0U) if(fwrite(bytes,1U,count,out)!=count){good=0;break;}
    if(ferror(in))good=0;
    if(fclose(in)!=0)good=0;
    if(fclose(out)!=0)good=0;
    return good && chmod(destination,0700)==0;
#endif
}

static int ProgramMain(int argc,char **argv)
{
    if(argc!=4 && argc!=6)return 2;
    const char *scenario=argv[1], *helper=argv[2], *fixtureRoot=argv[3];
    UmiProcessResult *result=calloc(1U,sizeof(*result));
    UmiCancellationToken *token=NULL;
    UmiProcessSupervisor *supervisor=NULL;
    UmiProcessJobSnapshot snapshot={0};
    UmiProcessJobId job=0U,other=0U;
    UmiProcessRequest request={0};
    Observation observed={0};
    const char *args[12]={"exit","0"};
    UmiEnvironmentVariable environment[3]={{"UMICOM_NATIVE_TEST","first"},{"UMICOM_NATIVE_TEST","last"}};
    char directory[8192], program[8192];
    char retainedOutput[UMI_PROCESS_OUTPUT_CAPACITY]={0};
    int failed=0;
    UmiStatus status;
#ifdef __linux__
    /* Reap only this fixture's orphaned descendants, without changing production policy. */
    CHECK(prctl(PR_SET_CHILD_SUBREAPER,1,0,0,0)==0);
#endif
    CHECK(result!=NULL && umi_cancellation_token_create(&token)==UMI_STATUS_OK);
    observed.token=token;
    request.program=helper;request.arguments=args;request.argument_count=2U;
    request.capture_stdout=1;request.capture_stderr=1;request.cancellation=token;
    request.timeout_ms=8000U;request.poll_interval_ms=2U;
    request.window_mode=UMI_PROCESS_WINDOW_HIDDEN;
#ifndef _WIN32
    if(strcmp(scenario,"ignored-sigchld")==0){
        struct sigaction ignored, previous;
        memset(&ignored,0,sizeof(ignored));ignored.sa_handler=SIG_IGN;
        CHECK(sigemptyset(&ignored.sa_mask)==0 && sigaction(SIGCHLD,&ignored,&previous)==0);
        status=UmiProcessExecuteWithLifetime(&request,UMI_PROCESS_LIFETIME_TREE,NULL,NULL,NULL,result);
        int restored=sigaction(SIGCHLD,&previous,NULL);
        CHECK(restored==0 && status==UMI_STATUS_INVALID_STATE && !result->launched);
        goto cleanup;
    }
#endif
    if(strcmp(scenario,"ctest-stop")==0){
        CHECK(argc==6);
        request.program=argv[4];
        args[0]="--test-dir";args[1]=argv[5];args[2]="-V";
        args[3]="--no-tests=error";args[4]="-R";args[5]="^notes.helper-tree$";
        request.argument_count=6U;observed.stopOnReady=1;
    } else if(strcmp(scenario,"nonzero")==0)args[1]="19";
    else if(strcmp(scenario,"arguments")==0){
        args[0]="echo";args[1]="";args[2]="a b";args[3]="a\"b";args[4]="a b\\";
        args[5]=";$(not-a-command)&";args[6]="\xCE\xBB \xF0\x9F\x9A\x80";args[7]="line\nnext";
        request.argument_count=8U;
    } else if(strcmp(scenario,"environment")==0 || strcmp(scenario,"duplicate-environment")==0 ||
              strcmp(scenario,"empty-environment")==0){
        args[0]="env";args[1]="UMICOM_NATIVE_TEST";request.environment=environment;
        request.environment_count=strcmp(scenario,"duplicate-environment")==0?2U:1U;
        if(strcmp(scenario,"environment")==0)environment[0].value="\xCE\xBB \xF0\x9F\x9A\x80";
        if(strcmp(scenario,"empty-environment")==0)environment[0].value="";
    } else if(strcmp(scenario,"working-directory")==0 || strcmp(scenario,"unicode-path")==0){
#ifdef _WIN32
        unsigned long pid=(unsigned long)GetCurrentProcessId();
#else
        unsigned long pid=(unsigned long)getpid();
#endif
        CHECK(snprintf(directory,sizeof(directory),"%s/run-%lu-\xCE\xBB-\xF0\x9F\x9A\x80",fixtureRoot,pid)>0);
        CHECK(MakeDirectory(directory));
        request.working_directory=directory;args[0]="cwd";request.argument_count=1U;
        if(strcmp(scenario,"unicode-path")==0){
            CHECK(snprintf(program,sizeof(program),"%s/notes \xCE\xBB.exe",directory)>0);
            CHECK(CopyExecutable(helper,program));request.program=program;
        }
    } else if(strcmp(scenario,"missing-program")==0)request.program="umicom-nonexistent-tool-84328912";
    else if(strcmp(scenario,"missing-directory")==0)request.working_directory="umicom-missing-directory-84328912";
    else if(strcmp(scenario,"invalid-request")==0)request.arguments=NULL;
    else if(strcmp(scenario,"path-override")==0){
        const char *slash=strrchr(helper,'/'), *backslash=strrchr(helper,'\\');
        if(backslash!=NULL && (slash==NULL || backslash>slash))slash=backslash;
        CHECK(slash!=NULL && (size_t)(slash-helper)<sizeof(directory));
        size_t prefix=(size_t)(slash-helper);
        memcpy(directory,helper,prefix);directory[prefix]='\0';
        request.program=slash+1;
        environment[0].name="PATH";environment[0].value=directory;
        request.environment=environment;request.environment_count=1U;
    }
    else if(strcmp(scenario,"invalid-environment")==0){environment[0].name="BAD=KEY";request.environment=environment;request.environment_count=1U;}
    else if(strcmp(scenario,"pre-cancel")==0)umi_cancellation_token_request(token);
    else if(strcmp(scenario,"owned-cancel")==0 || strcmp(scenario,"owned-timeout")==0 ||
            strcmp(scenario,"child-compatibility")==0){
        args[0]="tree";request.argument_count=1U;
        if(strcmp(scenario,"owned-timeout")==0)request.timeout_ms=1500U;
        else observed.stopOnReady=1;
    } else if(strcmp(scenario,"owned-return")==0 || strcmp(scenario,"owned-failure")==0){
        args[0]=strcmp(scenario,"owned-return")==0?"orphan":"orphan-fail";request.argument_count=1U;
    } else if(strcmp(scenario,"flood-stop")==0){args[0]="flood";request.argument_count=1U;observed.stopOnOutput=1;}
    else if(strcmp(scenario,"raw-output")==0){args[0]="stream";request.argument_count=1U;}
    else if(strncmp(scenario,"supervisor-",11U)==0){
        CHECK(umi_process_supervisor_create(NULL,&supervisor)==UMI_STATUS_OK);
        if(strcmp(scenario,"supervisor-output-tail")==0){
            args[0]="diagnostic-tail";request.argument_count=1U;
            CHECK(UmiProcessSupervisorSubmitWithLifetime(supervisor,"Compiler diagnostic",&request,
                UMI_PROCESS_LIFETIME_TREE,&job)==UMI_STATUS_OK);
            CHECK(umi_process_supervisor_wait(supervisor,job,5000U)==UMI_STATUS_OK);
            CHECK(umi_process_supervisor_snapshot(supervisor,job,&snapshot)==UMI_STATUS_OK);
            CHECK(snapshot.state==UMI_PROCESS_JOB_FAILED && snapshot.exit_code==23 && snapshot.output_truncated);
            CHECK(strstr(snapshot.output,"FINAL_NATIVE_DIAGNOSTIC")!=NULL);
            goto cleanup;
        }
        if(strcmp(scenario,"supervisor-invalid-lifetime")==0){
            CHECK(UmiProcessSupervisorSubmitWithLifetime(supervisor,"Invalid policy",&request,
                (UmiProcessLifetime)999,&job)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(umi_process_supervisor_stats(supervisor).jobs==0U);
            goto cleanup;
        }
        args[0]="tree";request.argument_count=1U;
        if(strcmp(scenario,"supervisor-child-compatibility")==0)
            CHECK(umi_process_supervisor_submit(supervisor,"Independent children",&request,&job)==UMI_STATUS_OK);
        else CHECK(UmiProcessSupervisorSubmitWithLifetime(supervisor,"Notes native tool",&request,UMI_PROCESS_LIFETIME_TREE,&job)==UMI_STATUS_OK);
        for(unsigned n=0U;n<2000U;++n){
            CHECK(umi_process_supervisor_snapshot(supervisor,job,&snapshot)==UMI_STATUS_OK);
            if(strstr(snapshot.output,"TREE_READY")!=NULL)break;
            umi_thread_sleep_ms(2U);
        }
        CHECK(strstr(snapshot.output,"TREE_READY")!=NULL);
        (void)snprintf(retainedOutput,sizeof(retainedOutput),"%s",snapshot.output);
        if(strcmp(scenario,"supervisor-independent")==0){
            args[0]="sleep";args[1]="1000";request.argument_count=2U;
            CHECK(UmiProcessSupervisorSubmitWithLifetime(supervisor,"Other project",&request,
                UMI_PROCESS_LIFETIME_TREE,&other)==UMI_STATUS_OK);
            for(unsigned n=0U;n<1000U;++n){
                CHECK(umi_process_supervisor_snapshot(supervisor,other,&snapshot)==UMI_STATUS_OK);
                if(snapshot.state==UMI_PROCESS_JOB_RUNNING)break;
                umi_thread_sleep_ms(1U);
            }
            CHECK(snapshot.state==UMI_PROCESS_JOB_RUNNING);
        }
        if(strcmp(scenario,"supervisor-shutdown")==0)CHECK(umi_process_supervisor_shutdown(supervisor)==UMI_STATUS_OK);
        else CHECK(umi_process_supervisor_cancel(supervisor,job)==UMI_STATUS_OK);
        CHECK(umi_process_supervisor_wait(supervisor,job,5000U)==UMI_STATUS_OK);
        CHECK(umi_process_supervisor_snapshot(supervisor,job,&snapshot)==UMI_STATUS_OK);
        CHECK(snapshot.state==UMI_PROCESS_JOB_CANCELLED);
        if(strcmp(scenario,"supervisor-child-compatibility")==0){
            unsigned long ids[8];size_t count=Members(retainedOutput,ids);int alive=0;
            CHECK(count>=3U);
            for(size_t i=1U;i<count;++i)if(!MemberStopped(ids[i]))alive=1;
            CHECK(alive);
        } else CHECK(AllMembersStopped(retainedOutput));
        if(other!=0U){
            CHECK(umi_process_supervisor_wait(supervisor,other,5000U)==UMI_STATUS_OK);
            CHECK(umi_process_supervisor_snapshot(supervisor,other,&snapshot)==UMI_STATUS_OK);
            CHECK(snapshot.state==UMI_PROCESS_JOB_SUCCEEDED);
        }
        goto cleanup;
    }
#ifdef _WIN32
    if(strcmp(scenario,"invalid-utf8")==0){args[0]="echo";args[1]="\xC0\xAF";}
#endif
    status=UmiProcessExecuteWithLifetime(&request,strcmp(scenario,"child-compatibility")==0?
        UMI_PROCESS_LIFETIME_CHILD:strcmp(scenario,"invalid-lifetime")==0?
        (UmiProcessLifetime)999:UMI_PROCESS_LIFETIME_TREE,Observe,Raw,&observed,result);
    (void)snprintf(retainedOutput,sizeof(retainedOutput),"%s",result->output);
    if(strcmp(scenario,"nonzero")==0)CHECK(status==UMI_STATUS_INTERNAL_ERROR && result->exit_code==19);
    else if(strcmp(scenario,"owned-failure")==0)CHECK(status==UMI_STATUS_INTERNAL_ERROR && result->exit_code==17 && AllMembersStopped(result->output));
    else if(strcmp(scenario,"owned-return")==0)CHECK(status==UMI_STATUS_OK && AllMembersStopped(result->output));
    else if(strcmp(scenario,"owned-timeout")==0)CHECK(status==UMI_STATUS_TIMEOUT && result->timed_out && result->termination_requested && AllMembersStopped(result->output));
    else if(strcmp(scenario,"owned-cancel")==0 || strcmp(scenario,"ctest-stop")==0)CHECK(status==UMI_STATUS_CANCELLED && result->cancelled && result->termination_requested && AllMembersStopped(result->output));
    else if(strcmp(scenario,"flood-stop")==0)CHECK(status==UMI_STATUS_CANCELLED && observed.bytes>=1024U*1024U && result->duration_ms<7000U);
    else if(strcmp(scenario,"child-compatibility")==0){
        unsigned long ids[8];size_t count=Members(result->output,ids);int alive=0;
        CHECK(status==UMI_STATUS_CANCELLED && count>=3U);
        for(size_t i=1U;i<count;++i)if(!MemberStopped(ids[i]))alive=1;
        CHECK(alive);
    } else if(strcmp(scenario,"pre-cancel")==0)CHECK(status==UMI_STATUS_CANCELLED && !result->launched);
    else if(strcmp(scenario,"invalid-request")==0 || strcmp(scenario,"invalid-environment")==0 || strcmp(scenario,"invalid-utf8")==0 || strcmp(scenario,"invalid-lifetime")==0)
        CHECK(status==UMI_STATUS_INVALID_ARGUMENT && !result->launched);
    else if(strcmp(scenario,"missing-program")==0 || strcmp(scenario,"missing-directory")==0)CHECK(status==UMI_STATUS_IO_ERROR && !result->launched);
    else {
        CHECK(status==UMI_STATUS_OK && result->exit_code==0 && result->launched);
        if(strcmp(scenario,"arguments")==0)CHECK(strcmp(result->output,
            "ARG[0]=\nARG[1]=612062\nARG[2]=612262\nARG[3]=6120625c\nARG[4]=3b24286e6f742d612d636f6d6d616e642926\nARG[5]=cebb20f09f9a80\nARG[6]=6c696e650a6e657874\n")==0);
        if(strcmp(scenario,"environment")==0)CHECK(strcmp(result->output,"VALUE=\xCE\xBB \xF0\x9F\x9A\x80\n")==0);
        if(strcmp(scenario,"duplicate-environment")==0)CHECK(strcmp(result->output,"VALUE=last\n")==0);
        if(strcmp(scenario,"empty-environment")==0)CHECK(strcmp(result->output,"VALUE=\n")==0);
        if(strcmp(scenario,"working-directory")==0 || strcmp(scenario,"unicode-path")==0)CHECK(strstr(result->output,"run-")!=NULL && strstr(result->output,"\xCE\xBB-\xF0\x9F\x9A\x80")!=NULL);
        if(strcmp(scenario,"raw-output")==0)CHECK(observed.bytes==321U*4096U && result->output_truncated && observed.callbacks>0U);
    }
cleanup:
    if(failed && result!=NULL)fprintf(stderr,"status result: launched=%d exit=%d cancelled=%d timeout=%d output=%s\n",result->launched,result->exit_code,result->cancelled,result->timed_out,result->output);
    umi_process_supervisor_destroy(supervisor);
    CleanupMembers(retainedOutput);
    umi_cancellation_token_destroy(token);free(result);
    return failed;
}
#include "utf8_entry.inc"
