/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include "fixture.h"
#include <stdint.h>
void *__real_calloc(size_t,size_t);
void *__real_malloc(size_t);
static int failCalloc,failMalloc;
void *__wrap_calloc(size_t n,size_t size){if(failCalloc){failCalloc=0;return NULL;}return __real_calloc(n,size);}
void *__wrap_malloc(size_t n){if(failMalloc){failMalloc=0;return NULL;}return __real_malloc(n);}
int main(void)
{
    UmiIbkrConnectionOptions options=UmiIbkrConnectionOptionsDefault();UmiIbkrConnection *c=(void *)(uintptr_t)1;
    failCalloc=1;CHECK(UmiIbkrConnectionCreate(&options,&c)==UMI_STATUS_OUT_OF_MEMORY&&c==NULL);
    Fixture *f=New();CHECK(f!=NULL);CHECK(Connect(f)==0);
    CHECK(PositionFeed(f,"DU123","1","1")==0);failMalloc=1;
    CHECK(UmiIbkrConnectionPump(f->c,10)==UMI_STATUS_OUT_OF_MEMORY);
    CHECK(f->closes==1U&&f->c->snapshot.stale);Delete(f);return 0;
}
