/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Linker-wrapped Linux failure tests. No production fault switches are added. */
#define _POSIX_C_SOURCE 200809L
#include "umicom/platform/threading.h"
#include <errno.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x); exit(1); } } while(0)
void *__real_calloc(size_t, size_t); void __real_free(void *);
int __real_pthread_create(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *);
int __real_pthread_detach(pthread_t);
int __real_nanosleep(const struct timespec *, struct timespec *);
static atomic_int track, freed, entered, go, leaving;
static _Atomic(void *) tracked;
static int failAllocation, failCreate, failDetach, fakeSleep, sleepCalls;
void *__wrap_calloc(size_t count, size_t size)
{
    if (failAllocation) { failAllocation = 0; return NULL; }
    void *p = __real_calloc(count, size);
    if (atomic_load(&track)) atomic_store(&tracked, p);
    return p;
}
void __wrap_free(void *p)
{
    if (p != NULL && p == atomic_load(&tracked)) atomic_fetch_add(&freed, 1);
    __real_free(p);
}
int __wrap_pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                         void *(*entry)(void *), void *context)
{
    if (failCreate) return EAGAIN;
    return __real_pthread_create(thread, attr, entry, context);
}
int __wrap_pthread_detach(pthread_t thread)
{
    if (failDetach) { failDetach = 0; return EINVAL; }
    return __real_pthread_detach(thread);
}
int __wrap_nanosleep(const struct timespec *request, struct timespec *remaining)
{
    if (!fakeSleep) return __real_nanosleep(request, remaining);
    ++sleepCalls;
    if (fakeSleep == 2) { errno = EINVAL; return -1; }
    if (sleepCalls == 1) {
        CHECK(request->tv_sec == 0 && request->tv_nsec == 100000000L);
        CHECK(remaining != NULL); remaining->tv_sec = 0; remaining->tv_nsec = 60000000L;
        errno = EINTR; return -1;
    }
    if (sleepCalls == 2) {
        CHECK(request->tv_nsec == 60000000L);
        remaining->tv_sec = 0; remaining->tv_nsec = 10000000L; errno = EINTR; return -1;
    }
    CHECK(sleepCalls == 3 && request->tv_nsec == 10000000L); return 0;
}
static void Wait(atomic_int *flag)
{
    for (unsigned i=0;i<10000U;++i) { if (atomic_load(flag)) return; umi_thread_sleep_ms(1U); }
    CHECK(0 /* worker timeout */);
}
static int Worker(void *unused)
{
    (void)unused; atomic_store(&entered, 1); Wait(&go); atomic_store(&leaving, 1); return 61;
}
static void Start(UmiThread **thread)
{
    atomic_store(&track,1); CHECK(umi_thread_start(Worker,NULL,thread)==UMI_STATUS_OK);
    atomic_store(&track,0); Wait(&entered);
}
int main(int argc, char **argv)
{
    CHECK(argc==2); UmiThread *thread=NULL; int result=0;
    if (!strcmp(argv[1],"allocation")) {
        failAllocation=1;
        CHECK(umi_thread_start(Worker,NULL,&thread)==UMI_STATUS_OUT_OF_MEMORY);
        CHECK(thread==NULL && !atomic_load(&entered));
    } else if (!strcmp(argv[1],"native_create")) {
        failCreate=1; atomic_store(&track,1);
        CHECK(umi_thread_start(Worker,NULL,&thread)==UMI_STATUS_UNAVAILABLE);
        CHECK(thread==NULL && atomic_load(&freed)==1 && !atomic_load(&entered));
    } else if (!strcmp(argv[1],"release_failure")) {
        Start(&thread); UmiThread *original=thread; failDetach=1;
        CHECK(UmiThreadRelease(&thread)==UMI_STATUS_INTERNAL_ERROR);
        CHECK(thread==original && !atomic_load(&freed));
        atomic_store(&go,1); CHECK(umi_thread_join(thread,&result)==UMI_STATUS_OK && result==61);
        CHECK(UmiThreadRelease(&thread)==UMI_STATUS_OK && thread==NULL && atomic_load(&freed)==1);
    } else if (!strcmp(argv[1],"release_exactly_once")) {
        Start(&thread); CHECK(UmiThreadRelease(&thread)==UMI_STATUS_OK);
        CHECK(thread==NULL && atomic_load(&freed)==0);
        atomic_store(&go,1); Wait(&freed); CHECK(atomic_load(&freed)==1); Wait(&leaving);
    } else if (!strcmp(argv[1],"join_exactly_once")) {
        Start(&thread); atomic_store(&go,1);
        CHECK(umi_thread_join(thread,&result)==UMI_STATUS_OK && result==61);
        CHECK(!atomic_load(&freed));
        CHECK(UmiThreadRelease(&thread)==UMI_STATUS_OK && thread==NULL && atomic_load(&freed)==1);
    } else if (!strcmp(argv[1],"sleep_remainder")) {
        fakeSleep=1; umi_thread_sleep_ms(100U); CHECK(sleepCalls==3);
    } else if (!strcmp(argv[1],"sleep_error")) {
        fakeSleep=2; umi_thread_sleep_ms(100U); CHECK(sleepCalls==1);
    } else CHECK(0 /* unknown test */);
    return 0;
}
