/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Thin native tool host. UTF-8 conversion belongs only at the Windows boundary. */
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "umicom/os_image/image.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
int wmain(int argc,wchar_t **wide) {
    char **argv=calloc((size_t)argc+1U,sizeof *argv);
    if(!argv)return 1;
    int result=1;
    for(int i=0;i<argc;++i) {
        int size=WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,wide[i],-1,NULL,0,NULL,NULL);
        if(size<=0)goto end;
        argv[i]=malloc((size_t)size);
        if(!argv[i])goto end;
        if(!WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,wide[i],-1,argv[i],size,NULL,NULL))goto end;
    }
    result=UmiOsImageMain(argc,argv);
    end:for(int i=0;i<argc;++i)free(argv[i]);
    free(argv);
    return result;
}
#else
#include <signal.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>
/* Only this standalone host owns process-global signals. The handler performs
 * no allocation, locking or Framework call. A normal thread requests Stop. */
static atomic_int stopped;
static_assert(ATOMIC_INT_LOCK_FREE == 2, "The standalone signal adapter requires lock-free atomic int");
typedef struct StopWatch {
    UmiCancellationToken *token;
    atomic_int done;
}
StopWatch;
static void SignalStop(int number) {
    atomic_store_explicit(&stopped,number,memory_order_relaxed);
}
static void *WatchStop(void *context) {
    StopWatch *watch=context;
    while(!atomic_load(&watch->done)) {
        if(atomic_load_explicit(&stopped,memory_order_relaxed))umi_cancellation_token_request(watch->token);
        struct timespec interval= {
            0,10000000L
        }
        ;
        (void)nanosleep(&interval,NULL);
    }
    return NULL;
}
int main(int argc,char **argv) {
    UmiCancellationToken *token=NULL;
    if(umi_cancellation_token_create(&token)!=UMI_STATUS_OK)return 1;
    struct sigaction action= {
        0
    }
    ,oldInt,oldTerm;
    action.sa_handler=SignalStop;
    sigemptyset(&action.sa_mask);
    if(sigaction(SIGINT,&action,&oldInt)) {
        umi_cancellation_token_destroy(token);
        return 1;
    }
    if(sigaction(SIGTERM,&action,&oldTerm)) {
        (void)sigaction(SIGINT,&oldInt,NULL);
        umi_cancellation_token_destroy(token);
        return 1;
    }
    StopWatch watch;
    watch.token=token;
    atomic_init(&watch.done,0);
    pthread_t thread;
    int result=1;
    if(!pthread_create(&thread,NULL,WatchStop,&watch)) {
        result=UmiOsImageMainWithCancellation(argc,argv,token);
        atomic_store(&watch.done,1);
        if(pthread_join(thread,NULL))result=1;
    }
    (void)sigaction(SIGINT,&oldInt,NULL);
    (void)sigaction(SIGTERM,&oldTerm,NULL);
    umi_cancellation_token_destroy(token);
    int requested=atomic_load_explicit(&stopped,memory_order_relaxed);
    return requested?128+requested:result;
}
#endif
