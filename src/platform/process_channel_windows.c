/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/process_channel_windows.c
 * PURPOSE:
 *   Windows live child channel. Only three standard handles cross CreateProcess; an owned
 *   kill-on-close job and suspended launch prevent an unowned child. stdin uses overlapped
 *   I/O, so a non-reading child cannot block the GUI forever.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Windows live child channel. Only three standard handles cross CreateProcess;
 * an owned kill-on-close job and suspended launch prevent an unowned child.
 * stdin uses overlapped I/O, so a non-reading child cannot block the GUI forever.
 *---------------------------------------------------------------------------*/
#define WIN32_LEAN_AND_MEAN
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0602
#endif
#include <windows.h>
#include <bcrypt.h>
#include <wchar.h>
#include "process_channel_internal.h"
uint64_t PcMilliseconds(void){
    return (uint64_t)GetTickCount64();
}
static wchar_t *Wide(const char *s){
    int n=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,s,-1,NULL,0);
    if(n<1)return NULL;
    wchar_t *p=malloc((size_t)n*sizeof *p);
    if(p&&!MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,s,-1,p,n)){
        free(p);
        p=NULL;
    }
    return p;
}
static void Close(void **p){
    if(*p&&*p!=INVALID_HANDLE_VALUE)CloseHandle((HANDLE)*p);
    *p=NULL;
}
static void Drain(UmiProcessChannel *c){
    char bytes[2048];
    for(unsigned i=0;i<32U&&c->error;++i){
        DWORD available=0,n=0;
        if(!PeekNamedPipe(c->error,NULL,0,NULL,&available,NULL)){
            Close(&c->error);
            break;
        }
        if(!available)break;
        if(available>sizeof bytes)available=sizeof bytes;
        if(!ReadFile(c->error,bytes,available,&n,NULL)||!n){
            Close(&c->error);
            break;
        }
        PcDiagnostic(c,bytes,n);
    }
}
static void Reap(UmiProcessChannel *c){
    if(!c->snapshot.running)return;
    if(WaitForSingleObject(c->process,0)==WAIT_OBJECT_0){
        DWORD code=0;
        GetExitCodeProcess(c->process,&code);
        c->snapshot.exitCode=(int)code;
        c->snapshot.running=0;
        /* The root exited. Its job still owns any descendants and pipe handles. */
        if(c->job)TerminateJobObject(c->job,1);
    }
}
static int Quote(wchar_t *out,size_t cap,size_t *used,const wchar_t *s){
    if(*used+2U>=cap)return 0;
    out[(*used)++]=L'"';
    for(size_t i=0;;){
        size_t slashes=0;
        while(s[i]==L'\\'){
            ++slashes;
            ++i;
        }
        size_t need=slashes*((s[i]==L'"'||s[i]==0)?2U:1U)+(s[i]==L'"'?1U:0U);
        if(need>cap-*used-2U)return 0;
        for(size_t k=0;k<need;++k)out[(*used)++]=L'\\';
        if(!s[i])break;
        if(*used+2U>=cap)return 0;
        out[(*used)++]=s[i++];
    }
    out[(*used)++]=L'"';
    out[*used]=0;
    return 1;
}
static wchar_t *Environment(void){
    const wchar_t *names[]={
        L"SystemRoot",L"TEMP",L"TMP",L"USERPROFILE",L"WINDIR"
    };
    wchar_t *e=calloc(32768U,sizeof *e);
    if(!e)return NULL;
    size_t used=0;
    for(size_t i=0;i<5U;++i){
        wchar_t value[4096];
        DWORD n=GetEnvironmentVariableW(names[i],value,4096);
        if(!n||n>=4096)continue;
        size_t key=wcslen(names[i]);
        if(used+key+n+2U>=32767U){
            free(e);
            return NULL;
        }
        memcpy(e+used,names[i],key*sizeof *e);
        used+=key;
        e[used++]=L'=';
        memcpy(e+used,value,(n+1U)*sizeof *e);
        used+=n+1U;
    }
    /* No developer PATH, injected DLL variables, QEMU variables or credentials. */
    e[used]=0;
    return e;
}
/* Launch environment policy now belongs to explicit wrappers; native pipe and process ownership remain shared. The restricted launch implementation is retained for review. The previous implementation is retained for engineering review. */
#if 0
UmiStatus UmiProcessChannelOpen(const UmiProcessChannelRequest *r,UmiProcessChannel **out){
    if(!out)return UMI_STATUS_INVALID_ARGUMENT;
    *out=NULL;
    UmiStatus s=PcValidate(r);
    if(s!=UMI_STATUS_OK)return s;
    wchar_t *program=Wide(r->program),*cwd=Wide(r->workingDirectory),*env=Environment();
    wchar_t *command=calloc(32768U,sizeof *command);
    size_t used=0;
    UmiProcessChannel *c=calloc(1,sizeof *c);
    HANDLE childIn=NULL,childOut=NULL,childErr=NULL;
    STARTUPINFOEXW start;
    memset(&start,0,sizeof start);
    SIZE_T attributeSize=0;
    if(!program||!cwd||!env||!command||!c){
        s=UMI_STATUS_OUT_OF_MEMORY;
        goto finish;
    }
    c->snapshot.exitCode=-1;
    if(!Quote(command,32768U,&used,program)){
        s=UMI_STATUS_CAPACITY_EXCEEDED;
        goto finish;
    }
    for(size_t i=0;i<r->argumentCount;++i){
        wchar_t *a=Wide(r->arguments[i]);
        if(!a){
            s=UMI_STATUS_INVALID_ARGUMENT;
            goto finish;
        }
        command[used++]=L' ';
        int ok=Quote(command,32768U,&used,a);
        free(a);
        if(!ok){
            s=UMI_STATUS_CAPACITY_EXCEEDED;
            goto finish;
        }
    }
    SECURITY_ATTRIBUTES sa={
        sizeof sa,NULL,TRUE
    };
    unsigned char random[16];
    if(BCryptGenRandom(NULL,random,sizeof random,BCRYPT_USE_SYSTEM_PREFERRED_RNG)!=0){
        s=UMI_STATUS_IO_ERROR;
        goto finish;
    }
    wchar_t name[100];
    size_t pos=(size_t)swprintf(name,100,L"\\\\.\\pipe\\umicom-channel-");
    const wchar_t hex[]=L"0123456789abcdef";
    for(size_t i=0;i<sizeof random;++i){
        name[pos++]=hex[random[i]>>4];
        name[pos++]=hex[random[i]&15U];
    }
    name[pos]=0;
    c->input=CreateNamedPipeW(name,PIPE_ACCESS_OUTBOUND|FILE_FLAG_OVERLAPPED|FILE_FLAG_FIRST_PIPE_INSTANCE,         PIPE_TYPE_BYTE|PIPE_READMODE_BYTE|PIPE_WAIT|PIPE_REJECT_REMOTE_CLIENTS,1,8192,8192,0,NULL);
    if(c->input==INVALID_HANDLE_VALUE){
        c->input=NULL;
        s=UMI_STATUS_IO_ERROR;
        goto finish;
    }
    childIn=CreateFileW(name,GENERIC_READ,0,&sa,OPEN_EXISTING,0,NULL);
    if(childIn==INVALID_HANDLE_VALUE){
        childIn=NULL;
        s=UMI_STATUS_IO_ERROR;
        goto finish;
    }
    /* The client end is already open. A connected result is success even
     * when ConnectNamedPipe reports ERROR_PIPE_CONNECTED. */
    {
        OVERLAPPED connection={0};
        connection.hEvent=CreateEventW(NULL,TRUE,FALSE,NULL);
        if(!connection.hEvent){s=UMI_STATUS_IO_ERROR;goto finish;}
        BOOL connected=ConnectNamedPipe(c->input,&connection);
        DWORD error=connected?ERROR_SUCCESS:GetLastError();
        if(error==ERROR_IO_PENDING){
            DWORD transferred=0;
            if(WaitForSingleObject(connection.hEvent,1000)==WAIT_OBJECT_0)
                connected=GetOverlappedResult(c->input,&connection,&transferred,FALSE);
            else{
                CancelIoEx(c->input,&connection);
                (void)GetOverlappedResult(c->input,&connection,&transferred,TRUE);
                connected=FALSE;
            }
        }else connected=connected||error==ERROR_PIPE_CONNECTED;
        CloseHandle(connection.hEvent);
        if(!connected){s=UMI_STATUS_IO_ERROR;goto finish;}
    }
    if(!CreatePipe((PHANDLE)&c->output,&childOut,&sa,65536)||!CreatePipe((PHANDLE)&c->error,&childErr,&sa,65536)){
        s=UMI_STATUS_IO_ERROR;
        goto finish;
    }
    if(!SetHandleInformation(c->output,HANDLE_FLAG_INHERIT,0)||!SetHandleInformation(c->error,HANDLE_FLAG_INHERIT,0)){
        s=UMI_STATUS_IO_ERROR;
        goto finish;
    }
    InitializeProcThreadAttributeList(NULL,1,0,&attributeSize);
    start.lpAttributeList=malloc(attributeSize);
    if(!start.lpAttributeList){
        s=UMI_STATUS_OUT_OF_MEMORY;
        goto finish;
    }
    if(!InitializeProcThreadAttributeList(start.lpAttributeList,1,0,&attributeSize)){
        free(start.lpAttributeList);
        start.lpAttributeList=NULL;
        s=UMI_STATUS_IO_ERROR;
        goto finish;
    }
    HANDLE handles[]={
        childIn,childOut,childErr
    };
    if(!UpdateProcThreadAttribute(start.lpAttributeList,0,PROC_THREAD_ATTRIBUTE_HANDLE_LIST,handles,sizeof handles,NULL,NULL)){
        s=UMI_STATUS_IO_ERROR;
        goto finish;
    }
    c->job=CreateJobObjectW(NULL,NULL);
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limit;
    memset(&limit,0,sizeof limit);
    limit.BasicLimitInformation.LimitFlags=JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if(!c->job||!SetInformationJobObject(c->job,JobObjectExtendedLimitInformation,&limit,sizeof limit)){
        s=UMI_STATUS_IO_ERROR;
        goto finish;
    }
    start.StartupInfo.cb=sizeof start;
    start.StartupInfo.dwFlags=STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
    start.StartupInfo.wShowWindow=SW_HIDE;
    start.StartupInfo.hStdInput=childIn;
    start.StartupInfo.hStdOutput=childOut;
    start.StartupInfo.hStdError=childErr;
    PROCESS_INFORMATION info;
    memset(&info,0,sizeof info);
    if(!CreateProcessW(program,command,NULL,NULL,TRUE,EXTENDED_STARTUPINFO_PRESENT|CREATE_SUSPENDED|CREATE_NO_WINDOW|CREATE_UNICODE_ENVIRONMENT,env,cwd,&start.StartupInfo,&info)){
        s=UMI_STATUS_UNAVAILABLE;
        goto finish;
    }
    c->process=info.hProcess;
    c->snapshot.processId=info.dwProcessId;
    c->snapshot.running=1;
    if(!AssignProcessToJobObject(c->job,c->process)||ResumeThread(info.hThread)==(DWORD)-1){
        TerminateProcess(c->process,1);
        CloseHandle(info.hThread);
        s=UMI_STATUS_IO_ERROR;
        goto finish;
    }
    CloseHandle(info.hThread);
    *out=c;
    c=NULL;
    s=UMI_STATUS_OK;
    finish:     if(start.lpAttributeList){
        DeleteProcThreadAttributeList(start.lpAttributeList);
        free(start.lpAttributeList);
    }
    if(childIn)CloseHandle(childIn);
    if(childOut)CloseHandle(childOut);
    if(childErr)CloseHandle(childErr);
    free(program);
    free(cwd);
    free(env);
    free(command);
    UmiProcessChannelDestroy(c);
    return s;
}
#endif
#include "process_windows_environment.inc"
/* Both launch policies use the same handle allowlist and job ownership. The
 * environment belongs to the wrapper and is borrowed only by CreateProcessW. */
static UmiStatus ChannelOpenNative(const UmiProcessChannelRequest *r, wchar_t *env, UmiProcessChannel **out){
    if(!out)return UMI_STATUS_INVALID_ARGUMENT;
    *out=NULL;
    UmiStatus s=PcValidate(r);
    if(s!=UMI_STATUS_OK)return s;
    wchar_t *program=NULL,*cwd=NULL;
    s=UmiWindowsUtf16(r->program,&program);
    if(s==UMI_STATUS_OK)s=UmiWindowsUtf16(r->workingDirectory,&cwd);
    if(s!=UMI_STATUS_OK){free(program);free(cwd);return s;}
    wchar_t *command=calloc(32768U,sizeof *command);
    size_t used=0;
    UmiProcessChannel *c=calloc(1,sizeof *c);
    HANDLE childIn=NULL,childOut=NULL,childErr=NULL;
    STARTUPINFOEXW start;
    memset(&start,0,sizeof start);
    SIZE_T attributeSize=0;
    if(!program||!cwd||!command||!c){
        s=UMI_STATUS_OUT_OF_MEMORY;
        goto finish;
    }
    c->snapshot.exitCode=-1;
    if(!Quote(command,32768U,&used,program)){
        s=UMI_STATUS_CAPACITY_EXCEEDED;
        goto finish;
    }
    for(size_t i=0;i<r->argumentCount;++i){
        wchar_t *a=Wide(r->arguments[i]);
        if(!a){
            s=UMI_STATUS_INVALID_ARGUMENT;
            goto finish;
        }
        command[used++]=L' ';
        int ok=Quote(command,32768U,&used,a);
        free(a);
        if(!ok){
            s=UMI_STATUS_CAPACITY_EXCEEDED;
            goto finish;
        }
    }
    SECURITY_ATTRIBUTES sa={
        sizeof sa,NULL,TRUE
    };
    unsigned char random[16];
    if(BCryptGenRandom(NULL,random,sizeof random,BCRYPT_USE_SYSTEM_PREFERRED_RNG)!=0){
        s=UMI_STATUS_IO_ERROR;
        goto finish;
    }
    wchar_t name[100];
    size_t pos=(size_t)swprintf(name,100,L"\\\\.\\pipe\\umicom-channel-");
    const wchar_t hex[]=L"0123456789abcdef";
    for(size_t i=0;i<sizeof random;++i){
        name[pos++]=hex[random[i]>>4];
        name[pos++]=hex[random[i]&15U];
    }
    name[pos]=0;
    c->input=CreateNamedPipeW(name,PIPE_ACCESS_OUTBOUND|FILE_FLAG_OVERLAPPED|FILE_FLAG_FIRST_PIPE_INSTANCE,         PIPE_TYPE_BYTE|PIPE_READMODE_BYTE|PIPE_WAIT|PIPE_REJECT_REMOTE_CLIENTS,1,8192,8192,0,NULL);
    if(c->input==INVALID_HANDLE_VALUE){
        c->input=NULL;
        s=UMI_STATUS_IO_ERROR;
        goto finish;
    }
    childIn=CreateFileW(name,GENERIC_READ,0,&sa,OPEN_EXISTING,0,NULL);
    if(childIn==INVALID_HANDLE_VALUE){
        childIn=NULL;
        s=UMI_STATUS_IO_ERROR;
        goto finish;
    }
    /* The client end is already open. A connected result is success even
     * when ConnectNamedPipe reports ERROR_PIPE_CONNECTED. */
    {
        OVERLAPPED connection={0};
        connection.hEvent=CreateEventW(NULL,TRUE,FALSE,NULL);
        if(!connection.hEvent){s=UMI_STATUS_IO_ERROR;goto finish;}
        BOOL connected=ConnectNamedPipe(c->input,&connection);
        DWORD error=connected?ERROR_SUCCESS:GetLastError();
        if(error==ERROR_IO_PENDING){
            DWORD transferred=0;
            if(WaitForSingleObject(connection.hEvent,1000)==WAIT_OBJECT_0)
                connected=GetOverlappedResult(c->input,&connection,&transferred,FALSE);
            else{
                CancelIoEx(c->input,&connection);
                (void)GetOverlappedResult(c->input,&connection,&transferred,TRUE);
                connected=FALSE;
            }
        }else connected=connected||error==ERROR_PIPE_CONNECTED;
        CloseHandle(connection.hEvent);
        if(!connected){s=UMI_STATUS_IO_ERROR;goto finish;}
    }
    if(!CreatePipe((PHANDLE)&c->output,&childOut,&sa,65536)||!CreatePipe((PHANDLE)&c->error,&childErr,&sa,65536)){
        s=UMI_STATUS_IO_ERROR;
        goto finish;
    }
    if(!SetHandleInformation(c->output,HANDLE_FLAG_INHERIT,0)||!SetHandleInformation(c->error,HANDLE_FLAG_INHERIT,0)){
        s=UMI_STATUS_IO_ERROR;
        goto finish;
    }
    InitializeProcThreadAttributeList(NULL,1,0,&attributeSize);
    start.lpAttributeList=malloc(attributeSize);
    if(!start.lpAttributeList){
        s=UMI_STATUS_OUT_OF_MEMORY;
        goto finish;
    }
    if(!InitializeProcThreadAttributeList(start.lpAttributeList,1,0,&attributeSize)){
        free(start.lpAttributeList);
        start.lpAttributeList=NULL;
        s=UMI_STATUS_IO_ERROR;
        goto finish;
    }
    HANDLE handles[]={
        childIn,childOut,childErr
    };
    if(!UpdateProcThreadAttribute(start.lpAttributeList,0,PROC_THREAD_ATTRIBUTE_HANDLE_LIST,handles,sizeof handles,NULL,NULL)){
        s=UMI_STATUS_IO_ERROR;
        goto finish;
    }
    c->job=CreateJobObjectW(NULL,NULL);
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limit;
    memset(&limit,0,sizeof limit);
    limit.BasicLimitInformation.LimitFlags=JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if(!c->job||!SetInformationJobObject(c->job,JobObjectExtendedLimitInformation,&limit,sizeof limit)){
        s=UMI_STATUS_IO_ERROR;
        goto finish;
    }
    start.StartupInfo.cb=sizeof start;
    start.StartupInfo.dwFlags=STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
    start.StartupInfo.wShowWindow=SW_HIDE;
    start.StartupInfo.hStdInput=childIn;
    start.StartupInfo.hStdOutput=childOut;
    start.StartupInfo.hStdError=childErr;
    PROCESS_INFORMATION info;
    memset(&info,0,sizeof info);
    if(!CreateProcessW(program,command,NULL,NULL,TRUE,EXTENDED_STARTUPINFO_PRESENT|CREATE_SUSPENDED|CREATE_NO_WINDOW|CREATE_UNICODE_ENVIRONMENT,env,cwd,&start.StartupInfo,&info)){
        s=UMI_STATUS_UNAVAILABLE;
        goto finish;
    }
    c->process=info.hProcess;
    c->snapshot.processId=info.dwProcessId;
    c->snapshot.running=1;
    if(!AssignProcessToJobObject(c->job,c->process)||ResumeThread(info.hThread)==(DWORD)-1){
        TerminateProcess(c->process,1);
        CloseHandle(info.hThread);
        s=UMI_STATUS_IO_ERROR;
        goto finish;
    }
    CloseHandle(info.hThread);
    *out=c;
    c=NULL;
    s=UMI_STATUS_OK;
    finish:     if(start.lpAttributeList){
        DeleteProcThreadAttributeList(start.lpAttributeList);
        free(start.lpAttributeList);
    }
    if(childIn)CloseHandle(childIn);
    if(childOut)CloseHandle(childOut);
    if(childErr)CloseHandle(childErr);
    free(program);
    free(cwd);
    free(command);
    UmiProcessChannelDestroy(c);
    return s;
}
UmiStatus UmiProcessChannelOpen(const UmiProcessChannelRequest *request, UmiProcessChannel **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiStatus status = PcValidate(request);
    if (status != UMI_STATUS_OK) return status;
    wchar_t *environment = Environment();
    if (environment == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = ChannelOpenNative(request, environment, out);
    free(environment);
    return status;
}
UmiStatus UmiProcessChannelOpenProgram(const UmiProcessChannelRequest *request,
    const UmiEnvironmentVariable *environment, size_t count, UmiProcessChannel **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiStatus status = PcValidate(request);
    if (status == UMI_STATUS_OK) status = PcEnvironmentValidate(environment, count);
    if (status != UMI_STATUS_OK) return status;
    UmiProcessRequest values = {0};
    values.environment = environment;
    values.environment_count = count;
    wchar_t *block = NULL;
    status = umi_windows_environment_block(&values, &block);
    if (status == UMI_STATUS_OK) status = ChannelOpenNative(request, block, out);
    free(block);
    return status;
}
UmiStatus UmiProcessChannelCloseInput(UmiProcessChannel *channel)
{
    if (channel == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    Close(&channel->input);
    return UMI_STATUS_OK;
}

UmiStatus UmiProcessChannelRead(UmiProcessChannel *c,void *b,size_t cap,size_t *out,unsigned timeout){
    if(!c||!b||!out||!cap||cap>65536U||timeout>60000U)return UMI_STATUS_INVALID_ARGUMENT;
    *out=0;
    uint64_t end=PcMilliseconds()+timeout;
    for(;;){
        Drain(c);
        Reap(c);
        if(!c->output)return UMI_STATUS_OK;
        DWORD available=0,n=0;
        if(!PeekNamedPipe(c->output,NULL,0,NULL,&available,NULL)){
            if(GetLastError()==ERROR_BROKEN_PIPE){
                Close(&c->output);
                return UMI_STATUS_OK;
            }
            return UMI_STATUS_IO_ERROR;
        }
        if(available){
            if(available>cap)available=(DWORD)cap;
            if(!ReadFile(c->output,b,available,&n,NULL))return UMI_STATUS_IO_ERROR;
            *out=n;
            return UMI_STATUS_OK;
        }
        if(PcMilliseconds()>=end)return UMI_STATUS_TIMEOUT;
        Sleep(2);
    }
}
/* Explicit end-of-input now rejects later writes before accessing a closed handle. The prior writer is retained for review. The previous implementation is retained for engineering review. */
#if 0
UmiStatus UmiProcessChannelWrite(UmiProcessChannel *c,const void *p,size_t n,unsigned timeout){
    if(!c||(!p&&n)||n>4096U||timeout>60000U)return UMI_STATUS_INVALID_ARGUMENT;
    if(!n)return UMI_STATUS_OK;
    Drain(c);
    Reap(c);
    if(!c->snapshot.running)return UMI_STATUS_INVALID_STATE;
    OVERLAPPED io;
    memset(&io,0,sizeof io);
    io.hEvent=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!io.hEvent)return UMI_STATUS_IO_ERROR;
    DWORD sent=0;
    UmiStatus s=UMI_STATUS_OK;
    if(!WriteFile(c->input,p,(DWORD)n,&sent,&io)){
        if(GetLastError()!=ERROR_IO_PENDING)s=UMI_STATUS_IO_ERROR;
        else if(WaitForSingleObject(io.hEvent,timeout)!=WAIT_OBJECT_0){
            CancelIoEx(c->input,&io);
            WaitForSingleObject(io.hEvent,INFINITE);
            s=UMI_STATUS_TIMEOUT;
        }
        else if(!GetOverlappedResult(c->input,&io,&sent,FALSE))s=UMI_STATUS_IO_ERROR;
    }
    if(s==UMI_STATUS_OK&&sent!=n)s=UMI_STATUS_IO_ERROR;
    CloseHandle(io.hEvent);
    return s;
}
#endif
UmiStatus UmiProcessChannelWrite(UmiProcessChannel *c,const void *p,size_t n,unsigned timeout){
    if(!c||(!p&&n)||n>4096U||timeout>60000U)return UMI_STATUS_INVALID_ARGUMENT;
    if(!c->input)return UMI_STATUS_INVALID_STATE;
    if(!n)return UMI_STATUS_OK;
    Drain(c);
    Reap(c);
    if(!c->snapshot.running)return UMI_STATUS_INVALID_STATE;
    OVERLAPPED io;
    memset(&io,0,sizeof io);
    io.hEvent=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!io.hEvent)return UMI_STATUS_IO_ERROR;
    DWORD sent=0;
    UmiStatus s=UMI_STATUS_OK;
    if(!WriteFile(c->input,p,(DWORD)n,&sent,&io)){
        if(GetLastError()!=ERROR_IO_PENDING)s=UMI_STATUS_IO_ERROR;
        else if(WaitForSingleObject(io.hEvent,timeout)!=WAIT_OBJECT_0){
            CancelIoEx(c->input,&io);
            WaitForSingleObject(io.hEvent,INFINITE);
            s=UMI_STATUS_TIMEOUT;
        }
        else if(!GetOverlappedResult(c->input,&io,&sent,FALSE))s=UMI_STATUS_IO_ERROR;
    }
    if(s==UMI_STATUS_OK&&sent!=n)s=UMI_STATUS_IO_ERROR;
    CloseHandle(io.hEvent);
    return s;
}
UmiStatus UmiProcessChannelPoll(UmiProcessChannel *c,UmiProcessChannelSnapshot *out){
    if(!c||!out)return UMI_STATUS_INVALID_ARGUMENT;
    Drain(c);
    Reap(c);
    *out=c->snapshot;
    return UMI_STATUS_OK;
}
UmiStatus UmiProcessChannelTerminate(UmiProcessChannel *c){
    if(!c)return UMI_STATUS_INVALID_ARGUMENT;
    Reap(c);
    if(c->snapshot.running){
        c->snapshot.terminated=1;
        if(!TerminateJobObject(c->job,1))return UMI_STATUS_IO_ERROR;
        if(WaitForSingleObject(c->process,5000)!=WAIT_OBJECT_0)return UMI_STATUS_TIMEOUT;
        Reap(c);
    }
    return UMI_STATUS_OK;
}
void UmiProcessChannelDestroy(UmiProcessChannel *c){
    if(!c)return;
    (void)UmiProcessChannelTerminate(c);
    Close(&c->input);
    Close(&c->output);
    Close(&c->error);
    Close(&c->process);
    Close(&c->job);
    free(c);
}
