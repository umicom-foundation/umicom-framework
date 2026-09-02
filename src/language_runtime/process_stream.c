/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/process_stream.c
 *
 * PURPOSE:
 *   Implement a cross-platform persistent child process with stdin/stdout pipes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#endif
#include "umicom/language_runtime/process_stream.h"
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
struct UmiLanguageRuntimeProcessStream{PROCESS_INFORMATION p;HANDLE in_w,out_r;};
/* Provide the ch operation used by this module and its client applications. */
static void ch(HANDLE*h){/* Apply this operation only while the related capability or state is available. */ if(h&&*h&&*h!=INVALID_HANDLE_VALUE){CloseHandle(*h);*h=NULL;}}
/* Provide the quote arg operation used by this module and its client applications. */
static UmiStatus quote_arg(char*out,size_t cap,size_t*u,const char*a){size_t i=0,slashes=0;int q=0;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!a)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(const char*p=a;*p;p++)/* Preserve the original failure result so the caller can respond to the correct cause. */ if(*p==' '||*p=='\t'||*p=='"'){q=1;break;}/* Preserve the original failure result so the caller can respond to the correct cause. */ if(*u&&*u+2>cap)return UMI_STATUS_CAPACITY_EXCEEDED;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(*u)out[(*u)++]=' ';/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!q){size_t n=strlen(a);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(*u+n+1>cap)return UMI_STATUS_CAPACITY_EXCEEDED;memcpy(out+*u,a,n);*u+=n;out[*u]=0;return UMI_STATUS_OK;}/* Preserve the original failure result so the caller can respond to the correct cause. */ if(*u+2>cap)return UMI_STATUS_CAPACITY_EXCEEDED;out[(*u)++]='"';/* Continue only while work remains available; the loop body advances the state on each pass. */ while(1){slashes=0;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(a[i]=='\\'){slashes++;i++;}/* Preserve the original failure result so the caller can respond to the correct cause. */ if(a[i]=='"'){size_t n=slashes*2+1;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(n--){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(*u+2>cap)return UMI_STATUS_CAPACITY_EXCEEDED;out[(*u)++]='\\';}out[(*u)++]='"';i++;continue;}/* Preserve the original failure result so the caller can respond to the correct cause. */ if(a[i]=='\0'){size_t n=slashes*2;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(n--){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(*u+2>cap)return UMI_STATUS_CAPACITY_EXCEEDED;out[(*u)++]='\\';}break;}/* Continue only while work remains available; the loop body advances the state on each pass. */ while(slashes--){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(*u+2>cap)return UMI_STATUS_CAPACITY_EXCEEDED;out[(*u)++]='\\';}/* Preserve the original failure result so the caller can respond to the correct cause. */ if(*u+2>cap)return UMI_STATUS_CAPACITY_EXCEEDED;out[(*u)++]=a[i++];}out[(*u)++]='"';out[*u]=0;return UMI_STATUS_OK;}
/*
 * Provide the language runtime process stream start operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_process_stream_start(const UmiLanguageRuntimeProcessStreamConfig*c,UmiLanguageRuntimeProcessStream**out){SECURITY_ATTRIBUTES sa={0};STARTUPINFOA si={0};HANDLE in_r=NULL,out_w=NULL;char cmd[8192]={0};size_t u=0,i;BOOL ok;UmiLanguageRuntimeProcessStream*s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||c->program==NULL||!*c->program||c->argument_count>UMI_LANGUAGE_RUNTIME_MAX_ARGUMENTS||(c->argument_count&&c->arguments==NULL)||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(quote_arg(cmd,sizeof(cmd),&u,c->program)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<c->argument_count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(quote_arg(cmd,sizeof(cmd),&u,c->arguments[i])!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s=calloc(1,sizeof(*s));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!s)return UMI_STATUS_OUT_OF_MEMORY;sa.nLength=sizeof(sa);sa.bInheritHandle=TRUE;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!CreatePipe(&in_r,&s->in_w,&sa,0)||!SetHandleInformation(s->in_w,HANDLE_FLAG_INHERIT,0)||!CreatePipe(&s->out_r,&out_w,&sa,0)||!SetHandleInformation(s->out_r,HANDLE_FLAG_INHERIT,0)){ch(&in_r);ch(&out_w);ch(&s->in_w);ch(&s->out_r);free(s);return UMI_STATUS_IO_ERROR;}si.cb=sizeof(si);si.dwFlags=STARTF_USESTDHANDLES;si.hStdInput=in_r;si.hStdOutput=out_w;si.hStdError=c->merge_stderr?out_w:GetStdHandle(STD_ERROR_HANDLE);ok=CreateProcessA(NULL,cmd,NULL,NULL,TRUE,CREATE_NO_WINDOW,NULL,(c->working_directory&&*c->working_directory)?c->working_directory:NULL,&si,&s->p);ch(&in_r);ch(&out_w);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!ok){ch(&s->in_w);ch(&s->out_r);free(s);return UMI_STATUS_IO_ERROR;}*out=s;return UMI_STATUS_OK;}
/*
 * Write language runtime process stream in its stable representation and report capacity
 * or input failures to the caller.
 */
UmiStatus umi_language_runtime_process_stream_write(UmiLanguageRuntimeProcessStream*s,const void*b,size_t n){const unsigned char*p=b;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||(b==NULL&&n))return UMI_STATUS_INVALID_ARGUMENT;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(n){DWORD w=0,r=(DWORD)(n>UINT32_MAX?UINT32_MAX:n);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!WriteFile(s->in_w,p,r,&w,NULL)||!w)return UMI_STATUS_IO_ERROR;p+=w;n-=w;}return UMI_STATUS_OK;}
/*
 * Read language runtime process stream into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_language_runtime_process_stream_read(UmiLanguageRuntimeProcessStream*s,void*out,size_t cap,uint32_t timeout_ms,size_t*n){ULONGLONG start;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||out==NULL||!cap||n==NULL)return UMI_STATUS_INVALID_ARGUMENT;*n=0;start=GetTickCount64();/* Visit each bounded item once so every record receives the same rule. */ for(;;){DWORD av=0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!PeekNamedPipe(s->out_r,NULL,0,NULL,&av,NULL))return UMI_STATUS_IO_ERROR;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(av){DWORD rd=0,req=(DWORD)((size_t)av<cap?(size_t)av:cap);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!ReadFile(s->out_r,out,req,&rd,NULL))return UMI_STATUS_IO_ERROR;*n=rd;return UMI_STATUS_OK;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(timeout_ms==0||GetTickCount64()-start>=timeout_ms)return UMI_STATUS_NOT_FOUND;Sleep(1);}}
/*
 * Provide the language runtime process stream is running operation used by this module and
 * its client applications.
 */
int umi_language_runtime_process_stream_is_running(UmiLanguageRuntimeProcessStream*s){DWORD code=0;return s&&s->p.hProcess&&GetExitCodeProcess(s->p.hProcess,&code)&&code==STILL_ACTIVE;}
/*
 * Provide the language runtime process stream stop operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_process_stream_stop(UmiLanguageRuntimeProcessStream*s,uint32_t timeout_ms){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!s)return UMI_STATUS_INVALID_ARGUMENT;ch(&s->in_w);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_language_runtime_process_stream_is_running(s))return UMI_STATUS_OK;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(WaitForSingleObject(s->p.hProcess,timeout_ms)==WAIT_OBJECT_0)return UMI_STATUS_OK;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!TerminateProcess(s->p.hProcess,1))return UMI_STATUS_IO_ERROR;WaitForSingleObject(s->p.hProcess,1000);return UMI_STATUS_OK;}
/*
 * Release or reset state held by language runtime process stream so the same storage can
 * be reused safely.
 */
void umi_language_runtime_process_stream_destroy(UmiLanguageRuntimeProcessStream*s){/* Apply this branch only when its contract condition is satisfied. */ if(!s)return;(void)umi_language_runtime_process_stream_stop(s,100);ch(&s->in_w);ch(&s->out_r);ch(&s->p.hThread);ch(&s->p.hProcess);free(s);}
#else
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
struct UmiLanguageRuntimeProcessStream{pid_t pid;int in_w,out_r;};
/* Provide the cf operation used by this module and its client applications. */
static void cf(int*f){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(f&&*f>=0){close(*f);*f=-1;}}
/*
 * Provide the language runtime process stream start operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_process_stream_start(const UmiLanguageRuntimeProcessStreamConfig*c,UmiLanguageRuntimeProcessStream**out){int in[2]={-1,-1},o[2]={-1,-1};pid_t p;size_t i;UmiLanguageRuntimeProcessStream*s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||c->program==NULL||!*c->program||c->argument_count>UMI_LANGUAGE_RUNTIME_MAX_ARGUMENTS||(c->argument_count&&c->arguments==NULL)||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(pipe(in)||pipe(o)){cf(&in[0]);cf(&in[1]);cf(&o[0]);cf(&o[1]);return UMI_STATUS_IO_ERROR;}p=fork();/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p<0){cf(&in[0]);cf(&in[1]);cf(&o[0]);cf(&o[1]);return UMI_STATUS_IO_ERROR;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==0){char*argv[UMI_LANGUAGE_RUNTIME_MAX_ARGUMENTS+2];dup2(in[0],STDIN_FILENO);dup2(o[1],STDOUT_FILENO);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c->merge_stderr)dup2(o[1],STDERR_FILENO);cf(&in[0]);cf(&in[1]);cf(&o[0]);cf(&o[1]);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c->working_directory&&*c->working_directory)chdir(c->working_directory);argv[0]=(char*)c->program;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<c->argument_count;i++)argv[i+1]=(char*)c->arguments[i];argv[c->argument_count+1]=NULL;execvp(c->program,argv);_exit(127);}s=calloc(1,sizeof(*s));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!s){kill(p,SIGKILL);waitpid(p,NULL,0);cf(&in[0]);cf(&in[1]);cf(&o[0]);cf(&o[1]);return UMI_STATUS_OUT_OF_MEMORY;}s->pid=p;s->in_w=in[1];s->out_r=o[0];cf(&in[0]);cf(&o[1]);*out=s;return UMI_STATUS_OK;}
/*
 * Write language runtime process stream in its stable representation and report capacity
 * or input failures to the caller.
 */
UmiStatus umi_language_runtime_process_stream_write(UmiLanguageRuntimeProcessStream*s,const void*b,size_t n){const unsigned char*p=b;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||(b==NULL&&n))return UMI_STATUS_INVALID_ARGUMENT;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(n){ssize_t w=write(s->in_w,p,n);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(w<0){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(errno==EINTR)continue;return UMI_STATUS_IO_ERROR;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(w==0)return UMI_STATUS_IO_ERROR;p+=(size_t)w;n-=(size_t)w;}return UMI_STATUS_OK;}
/*
 * Read language runtime process stream into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_language_runtime_process_stream_read(UmiLanguageRuntimeProcessStream*s,void*out,size_t cap,uint32_t timeout_ms,size_t*n){struct pollfd fd;int r;ssize_t c;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||out==NULL||!cap||n==NULL)return UMI_STATUS_INVALID_ARGUMENT;fd=(struct pollfd){s->out_r,POLLIN,0};r=poll(&fd,1,timeout_ms>(uint32_t)INT32_MAX?INT32_MAX:(int)timeout_ms);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==0){*n=0;return UMI_STATUS_NOT_FOUND;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r<0)return errno==EINTR?UMI_STATUS_NOT_FOUND:UMI_STATUS_IO_ERROR;c=read(s->out_r,out,cap);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c<=0){*n=0;return c==0?UMI_STATUS_NOT_FOUND:UMI_STATUS_IO_ERROR;}*n=(size_t)c;return UMI_STATUS_OK;}
/*
 * Provide the language runtime process stream is running operation used by this module and
 * its client applications.
 */
int umi_language_runtime_process_stream_is_running(UmiLanguageRuntimeProcessStream*s){int st;pid_t r;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!s||s->pid<=0)return 0;r=waitpid(s->pid,&st,WNOHANG);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(r==0)return 1;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(r==s->pid)s->pid=-1;return 0;}
/*
 * Provide the language runtime process stream stop operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_process_stream_stop(UmiLanguageRuntimeProcessStream*s,uint32_t timeout_ms){uint32_t w=0;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!s)return UMI_STATUS_INVALID_ARGUMENT;cf(&s->in_w);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_language_runtime_process_stream_is_running(s))return UMI_STATUS_OK;kill(s->pid,SIGTERM);/* Continue only while work remains available; the loop body advances the state on each pass. */ while(w<timeout_ms){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_language_runtime_process_stream_is_running(s))return UMI_STATUS_OK;poll(NULL,0,1);w++;}/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_language_runtime_process_stream_is_running(s)){kill(s->pid,SIGKILL);waitpid(s->pid,NULL,0);s->pid=-1;}return UMI_STATUS_OK;}
/*
 * Release or reset state held by language runtime process stream so the same storage can
 * be reused safely.
 */
void umi_language_runtime_process_stream_destroy(UmiLanguageRuntimeProcessStream*s){/* Apply this branch only when its contract condition is satisfied. */ if(!s)return;(void)umi_language_runtime_process_stream_stop(s,100);cf(&s->in_w);cf(&s->out_r);free(s);}
#endif
