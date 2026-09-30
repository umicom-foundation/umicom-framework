/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/os_image/process.c
 * PURPOSE:
 *   Use the canonical process/cancellation runner, with bounded raw capture. Git, make and
 *   QEMU paths are explicit trust decisions. This is not a sandbox.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Use the canonical process/cancellation runner, with bounded raw capture.
 * Git, make and QEMU paths are explicit trust decisions. This is not a sandbox. */
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "internal.h"
#include <stdatomic.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <pthread.h>
#include <time.h>
#endif
typedef struct Capture {
    OiText *text;
    UmiCancellationToken *stop;
    size_t maximum;
}
Capture;
static void CaptureBytes(const char *data,size_t size,void *context) {
    Capture *c=context;
    if(c->text->status!=UMI_STATUS_OK)return;
    if(size>c->maximum-c->text->size) {
        c->text->status=UMI_STATUS_CAPACITY_EXCEEDED;
        umi_cancellation_token_request(c->stop);
        return;
    }
    OiAppend(c->text,data,size);
    if(c->text->status!=UMI_STATUS_OK)umi_cancellation_token_request(c->stop);
}
/* A bounded watcher combines external Stop with capture-overflow cancellation.
 * It owns no GUI object and is joined before borrowed request memory is released. */
typedef struct Watch {
    const UmiCancellationToken *external;
    UmiCancellationToken *stop;
    atomic_int done;
}
Watch;
#ifdef _WIN32
static DWORD WINAPI WatchRun(LPVOID context)
#else
static void *WatchRun(void *context)
#endif
{
    Watch *w=context;
    while(!atomic_load(&w->done)) {
        if(umi_cancellation_token_is_requested(w->external))umi_cancellation_token_request(w->stop);
#ifdef _WIN32
        Sleep(10);
#else
        struct timespec interval= {
            0,10000000L
        }
        ;
        (void)nanosleep(&interval,NULL);
#endif
    }
#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}
UmiStatus OiCaptureLimited(const char *program,const char *const *args,size_t count,const char *cwd, unsigned timeout,const UmiCancellationToken *cancel,size_t maximum,OiText *capture,UmiProcessResult *result) {
    if(!capture||!result||!maximum||maximum>UMI_OS_IMAGE_MAX_BUILD_LOG||capture->size>maximum)return UMI_STATUS_INVALID_ARGUMENT;
    memset(result,0,sizeof *result);
    result->exit_code=-1;
    UmiStatus s=OiNativeProgram(program);
    if(s!=UMI_STATUS_OK)return s;
    UmiCancellationToken *stop=NULL;
    s=umi_cancellation_token_create(&stop);
    if(s!=UMI_STATUS_OK)return s;
    if(umi_cancellation_token_is_requested(cancel))umi_cancellation_token_request(stop);
    Watch watch;
    watch.external=cancel;
    watch.stop=stop;
    atomic_init(&watch.done,0);
#ifdef _WIN32
    HANDLE thread=NULL;
    if(cancel)thread=CreateThread(NULL,0,WatchRun,&watch,0,NULL);
    if(cancel&&!thread) {
        umi_cancellation_token_destroy(stop);
        return UMI_STATUS_IO_ERROR;
    }
#else
    pthread_t thread;
    if(cancel&&pthread_create(&thread,NULL,WatchRun,&watch)!=0) {
        umi_cancellation_token_destroy(stop);
        return UMI_STATUS_IO_ERROR;
    }
#endif
    const UmiEnvironmentVariable environment[]= {
        {
            "LC_ALL","C"
        }
        , {
            "LANG","C"
        }
        , {
            "TZ","UTC"
        }
    }
    ;
    UmiProcessRequest request= {
        0
    }
    ;
    request.program=program;
    request.arguments=args;
    request.argument_count=count;
    request.working_directory=cwd;
    request.capture_stdout=1;
    request.capture_stderr=1;
    request.timeout_ms=timeout;
    request.poll_interval_ms=10;
    request.cancellation=stop;
    request.environment=environment;
    request.environment_count=sizeof environment/sizeof environment[0];
    request.window_mode=UMI_PROCESS_WINDOW_HIDDEN;
    Capture observer= {
        capture,stop,maximum
    }
    ;
    s=UmiProcessExecuteWithLifetime(&request,UMI_PROCESS_LIFETIME_TREE,NULL,CaptureBytes,&observer,result);
    atomic_store(&watch.done,1);
#ifdef _WIN32
    if(thread) {
        if(WaitForSingleObject(thread,INFINITE)!=WAIT_OBJECT_0)s=UMI_STATUS_IO_ERROR;
        (void)CloseHandle(thread);
    }
#else
    if(cancel&&pthread_join(thread,NULL)!=0)s=UMI_STATUS_IO_ERROR;
#endif
    if(capture->status!=UMI_STATUS_OK)s=capture->status;
    umi_cancellation_token_destroy(stop);
    return s;
}
UmiStatus OiCapture(const char *program,const char *const *args,size_t count, const char *cwd,unsigned timeout,const UmiCancellationToken *cancel, OiText *capture,UmiProcessResult *result) {
    return OiCaptureLimited(program,args,count,cwd,timeout,cancel,UMI_OS_IMAGE_MAX_LOG,capture,result);
}
UmiStatus OiBuildroot(const OiPlan *p) {
    const char *args[]= {
        "-C",p->buildroot,"rev-parse","HEAD"
    }
    ;
    OiText t;
    OiTextInit(&t);
    UmiProcessResult *r=calloc(1,sizeof *r);
    if(!r)return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus s=OiCapture(p->git,args,4,p->buildroot,10000,NULL,&t,r);
    if(s==UMI_STATUS_OK&&(t.size!=41U||memcmp(t.data,p->commit,40)||t.data[40]!='\n'))s=UMI_STATUS_INVALID_STATE;
    OiTextClear(&t);
    if(s==UMI_STATUS_OK) {
        const char *clean[]= {
            "-C",p->buildroot,"status","--porcelain=v1","--untracked-files=all"
        }
        ;
        s=OiCapture(p->git,clean,5,p->buildroot,10000,NULL,&t,r);
        if(s==UMI_STATUS_OK&&t.size)s=UMI_STATUS_INVALID_STATE;
        OiTextClear(&t);
    }
    free(r);
    return s;
}
