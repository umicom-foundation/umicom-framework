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
static void ch(HANDLE*h){if(h&&*h&&*h!=INVALID_HANDLE_VALUE){CloseHandle(*h);*h=NULL;}}
static UmiStatus quote_arg(char*out,size_t cap,size_t*u,const char*a){size_t i=0,slashes=0;int q=0;if(!a)return UMI_STATUS_INVALID_ARGUMENT;for(const char*p=a;*p;p++)if(*p==' '||*p=='\t'||*p=='"'){q=1;break;}if(*u&&*u+2>cap)return UMI_STATUS_CAPACITY_EXCEEDED;if(*u)out[(*u)++]=' ';if(!q){size_t n=strlen(a);if(*u+n+1>cap)return UMI_STATUS_CAPACITY_EXCEEDED;memcpy(out+*u,a,n);*u+=n;out[*u]=0;return UMI_STATUS_OK;}if(*u+2>cap)return UMI_STATUS_CAPACITY_EXCEEDED;out[(*u)++]='"';while(1){slashes=0;while(a[i]=='\\'){slashes++;i++;}if(a[i]=='"'){size_t n=slashes*2+1;while(n--){if(*u+2>cap)return UMI_STATUS_CAPACITY_EXCEEDED;out[(*u)++]='\\';}out[(*u)++]='"';i++;continue;}if(a[i]=='\0'){size_t n=slashes*2;while(n--){if(*u+2>cap)return UMI_STATUS_CAPACITY_EXCEEDED;out[(*u)++]='\\';}break;}while(slashes--){if(*u+2>cap)return UMI_STATUS_CAPACITY_EXCEEDED;out[(*u)++]='\\';}if(*u+2>cap)return UMI_STATUS_CAPACITY_EXCEEDED;out[(*u)++]=a[i++];}out[(*u)++]='"';out[*u]=0;return UMI_STATUS_OK;}
UmiStatus umi_language_runtime_process_stream_start(const UmiLanguageRuntimeProcessStreamConfig*c,UmiLanguageRuntimeProcessStream**out){SECURITY_ATTRIBUTES sa={0};STARTUPINFOA si={0};HANDLE in_r=NULL,out_w=NULL;char cmd[8192]={0};size_t u=0,i;BOOL ok;UmiLanguageRuntimeProcessStream*s;if(c==NULL||c->program==NULL||!*c->program||c->argument_count>UMI_LANGUAGE_RUNTIME_MAX_ARGUMENTS||(c->argument_count&&c->arguments==NULL)||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;if(quote_arg(cmd,sizeof(cmd),&u,c->program)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;for(i=0;i<c->argument_count;i++)if(quote_arg(cmd,sizeof(cmd),&u,c->arguments[i])!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s=calloc(1,sizeof(*s));if(!s)return UMI_STATUS_OUT_OF_MEMORY;sa.nLength=sizeof(sa);sa.bInheritHandle=TRUE;if(!CreatePipe(&in_r,&s->in_w,&sa,0)||!SetHandleInformation(s->in_w,HANDLE_FLAG_INHERIT,0)||!CreatePipe(&s->out_r,&out_w,&sa,0)||!SetHandleInformation(s->out_r,HANDLE_FLAG_INHERIT,0)){ch(&in_r);ch(&out_w);ch(&s->in_w);ch(&s->out_r);free(s);return UMI_STATUS_IO_ERROR;}si.cb=sizeof(si);si.dwFlags=STARTF_USESTDHANDLES;si.hStdInput=in_r;si.hStdOutput=out_w;si.hStdError=c->merge_stderr?out_w:GetStdHandle(STD_ERROR_HANDLE);ok=CreateProcessA(NULL,cmd,NULL,NULL,TRUE,CREATE_NO_WINDOW,NULL,(c->working_directory&&*c->working_directory)?c->working_directory:NULL,&si,&s->p);ch(&in_r);ch(&out_w);if(!ok){ch(&s->in_w);ch(&s->out_r);free(s);return UMI_STATUS_IO_ERROR;}*out=s;return UMI_STATUS_OK;}
UmiStatus umi_language_runtime_process_stream_write(UmiLanguageRuntimeProcessStream*s,const void*b,size_t n){const unsigned char*p=b;if(s==NULL||(b==NULL&&n))return UMI_STATUS_INVALID_ARGUMENT;while(n){DWORD w=0,r=(DWORD)(n>UINT32_MAX?UINT32_MAX:n);if(!WriteFile(s->in_w,p,r,&w,NULL)||!w)return UMI_STATUS_IO_ERROR;p+=w;n-=w;}return UMI_STATUS_OK;}
UmiStatus umi_language_runtime_process_stream_read(UmiLanguageRuntimeProcessStream*s,void*out,size_t cap,uint32_t timeout_ms,size_t*n){ULONGLONG start;if(s==NULL||out==NULL||!cap||n==NULL)return UMI_STATUS_INVALID_ARGUMENT;*n=0;start=GetTickCount64();for(;;){DWORD av=0;if(!PeekNamedPipe(s->out_r,NULL,0,NULL,&av,NULL))return UMI_STATUS_IO_ERROR;if(av){DWORD rd=0,req=(DWORD)((size_t)av<cap?(size_t)av:cap);if(!ReadFile(s->out_r,out,req,&rd,NULL))return UMI_STATUS_IO_ERROR;*n=rd;return UMI_STATUS_OK;}if(timeout_ms==0||GetTickCount64()-start>=timeout_ms)return UMI_STATUS_NOT_FOUND;Sleep(1);}}
int umi_language_runtime_process_stream_is_running(UmiLanguageRuntimeProcessStream*s){DWORD code=0;return s&&s->p.hProcess&&GetExitCodeProcess(s->p.hProcess,&code)&&code==STILL_ACTIVE;}
UmiStatus umi_language_runtime_process_stream_stop(UmiLanguageRuntimeProcessStream*s,uint32_t timeout_ms){if(!s)return UMI_STATUS_INVALID_ARGUMENT;ch(&s->in_w);if(!umi_language_runtime_process_stream_is_running(s))return UMI_STATUS_OK;if(WaitForSingleObject(s->p.hProcess,timeout_ms)==WAIT_OBJECT_0)return UMI_STATUS_OK;if(!TerminateProcess(s->p.hProcess,1))return UMI_STATUS_IO_ERROR;WaitForSingleObject(s->p.hProcess,1000);return UMI_STATUS_OK;}
void umi_language_runtime_process_stream_destroy(UmiLanguageRuntimeProcessStream*s){if(!s)return;(void)umi_language_runtime_process_stream_stop(s,100);ch(&s->in_w);ch(&s->out_r);ch(&s->p.hThread);ch(&s->p.hProcess);free(s);}
#else
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
struct UmiLanguageRuntimeProcessStream{pid_t pid;int in_w,out_r;};
static void cf(int*f){if(f&&*f>=0){close(*f);*f=-1;}}
UmiStatus umi_language_runtime_process_stream_start(const UmiLanguageRuntimeProcessStreamConfig*c,UmiLanguageRuntimeProcessStream**out){int in[2]={-1,-1},o[2]={-1,-1};pid_t p;size_t i;UmiLanguageRuntimeProcessStream*s;if(c==NULL||c->program==NULL||!*c->program||c->argument_count>UMI_LANGUAGE_RUNTIME_MAX_ARGUMENTS||(c->argument_count&&c->arguments==NULL)||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;if(pipe(in)||pipe(o)){cf(&in[0]);cf(&in[1]);cf(&o[0]);cf(&o[1]);return UMI_STATUS_IO_ERROR;}p=fork();if(p<0){cf(&in[0]);cf(&in[1]);cf(&o[0]);cf(&o[1]);return UMI_STATUS_IO_ERROR;}if(p==0){char*argv[UMI_LANGUAGE_RUNTIME_MAX_ARGUMENTS+2];dup2(in[0],STDIN_FILENO);dup2(o[1],STDOUT_FILENO);if(c->merge_stderr)dup2(o[1],STDERR_FILENO);cf(&in[0]);cf(&in[1]);cf(&o[0]);cf(&o[1]);if(c->working_directory&&*c->working_directory)chdir(c->working_directory);argv[0]=(char*)c->program;for(i=0;i<c->argument_count;i++)argv[i+1]=(char*)c->arguments[i];argv[c->argument_count+1]=NULL;execvp(c->program,argv);_exit(127);}s=calloc(1,sizeof(*s));if(!s){kill(p,SIGKILL);waitpid(p,NULL,0);cf(&in[0]);cf(&in[1]);cf(&o[0]);cf(&o[1]);return UMI_STATUS_OUT_OF_MEMORY;}s->pid=p;s->in_w=in[1];s->out_r=o[0];cf(&in[0]);cf(&o[1]);*out=s;return UMI_STATUS_OK;}
UmiStatus umi_language_runtime_process_stream_write(UmiLanguageRuntimeProcessStream*s,const void*b,size_t n){const unsigned char*p=b;if(s==NULL||(b==NULL&&n))return UMI_STATUS_INVALID_ARGUMENT;while(n){ssize_t w=write(s->in_w,p,n);if(w<0){if(errno==EINTR)continue;return UMI_STATUS_IO_ERROR;}if(w==0)return UMI_STATUS_IO_ERROR;p+=(size_t)w;n-=(size_t)w;}return UMI_STATUS_OK;}
UmiStatus umi_language_runtime_process_stream_read(UmiLanguageRuntimeProcessStream*s,void*out,size_t cap,uint32_t timeout_ms,size_t*n){struct pollfd fd;int r;ssize_t c;if(s==NULL||out==NULL||!cap||n==NULL)return UMI_STATUS_INVALID_ARGUMENT;fd=(struct pollfd){s->out_r,POLLIN,0};r=poll(&fd,1,timeout_ms>(uint32_t)INT32_MAX?INT32_MAX:(int)timeout_ms);if(r==0){*n=0;return UMI_STATUS_NOT_FOUND;}if(r<0)return errno==EINTR?UMI_STATUS_NOT_FOUND:UMI_STATUS_IO_ERROR;c=read(s->out_r,out,cap);if(c<=0){*n=0;return c==0?UMI_STATUS_NOT_FOUND:UMI_STATUS_IO_ERROR;}*n=(size_t)c;return UMI_STATUS_OK;}
int umi_language_runtime_process_stream_is_running(UmiLanguageRuntimeProcessStream*s){int st;pid_t r;if(!s||s->pid<=0)return 0;r=waitpid(s->pid,&st,WNOHANG);if(r==0)return 1;if(r==s->pid)s->pid=-1;return 0;}
UmiStatus umi_language_runtime_process_stream_stop(UmiLanguageRuntimeProcessStream*s,uint32_t timeout_ms){uint32_t w=0;if(!s)return UMI_STATUS_INVALID_ARGUMENT;cf(&s->in_w);if(!umi_language_runtime_process_stream_is_running(s))return UMI_STATUS_OK;kill(s->pid,SIGTERM);while(w<timeout_ms){if(!umi_language_runtime_process_stream_is_running(s))return UMI_STATUS_OK;poll(NULL,0,1);w++;}if(umi_language_runtime_process_stream_is_running(s)){kill(s->pid,SIGKILL);waitpid(s->pid,NULL,0);s->pid=-1;}return UMI_STATUS_OK;}
void umi_language_runtime_process_stream_destroy(UmiLanguageRuntimeProcessStream*s){if(!s)return;(void)umi_language_runtime_process_stream_stop(s,100);cf(&s->in_w);cf(&s->out_r);free(s);}
#endif
