/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_corpus_memory/test_corpus_memory.c
 *
 * PURPOSE:
 *   Measure corpus-owned allocation peaks and inject every allocation failure without loading model weights.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/corpus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static struct { void *pointer; size_t bytes; } tracked[8];
static size_t live_bytes, peak_bytes, calls, failure_at;
static void *record(void *pointer,size_t bytes)
{
    if(pointer==NULL)return NULL;
    for(size_t i=0;i<8U;++i)if(tracked[i].pointer==NULL){
        tracked[i].pointer=pointer;tracked[i].bytes=bytes;live_bytes+=bytes;
        if(live_bytes>peak_bytes)peak_bytes=live_bytes;
        return pointer;
    }
    abort();
}
static void *observe_malloc(size_t bytes)
{
    ++calls;if(calls==failure_at)return NULL;
    return record(malloc(bytes),bytes);
}
static void *observe_calloc(size_t count,size_t bytes)
{
    ++calls;if(calls==failure_at)return NULL;
    if(bytes!=0U && count>SIZE_MAX/bytes)return NULL;
    return record(calloc(count,bytes),count*bytes);
}
static void observe_free(void *pointer)
{
    if(pointer==NULL)return;
    for(size_t i=0;i<8U;++i)if(tracked[i].pointer==pointer){
        live_bytes-=tracked[i].bytes;tracked[i].pointer=NULL;tracked[i].bytes=0;
        free(pointer);return;
    }
    abort();
}
/* Instrument the production translation unit, not a second corpus algorithm.
 * stdio/runtime allocations are outside this explicit payload accounting. */
#define malloc observe_malloc
#define calloc observe_calloc
#define free observe_free
#include "../../src/ai/training/corpus.c"
#undef malloc
#undef calloc
#undef free
#define CHECK(x) do { if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;} } while(0)

static int fixture(const char *base,size_t bytes,char path[1024])
{
    FILE *file=NULL;
    for(unsigned int n=0;n<32U;++n){
        int used=snprintf(path,1024,"%s-%u.bin",base,n);
        if(used<0 || used>=1024)return 0;
        file=fopen(path,"wbx");if(file!=NULL)break;
    }
    if(file==NULL)return 0;
    unsigned char block[256];for(size_t i=0;i<sizeof(block);++i)block[i]=(unsigned char)i;
    for(size_t written=0;written<bytes;){
        size_t part=bytes-written;if(part>sizeof(block))part=sizeof(block);
        if(fwrite(block,1,part,file)!=part){(void)fclose(file);(void)remove(path);return 0;}
        written+=part;
    }
    if(fclose(file)!=0){(void)remove(path);return 0;}
    return 1;
}
static void reset_observation(size_t fail)
{
    if(live_bytes!=0)abort();
    peak_bytes=0;calls=0;failure_at=fail;
}
int main(int argc,char **argv)
{
    if(argc!=3)return 2;
    const int allocations=strcmp(argv[1],"allocations")==0;
    const int maximum=strcmp(argv[1],"maximum")==0;
    const int bounds=strcmp(argv[1],"bounds")==0;
    const int independence=strcmp(argv[1],"independence")==0;
    if(!allocations && !maximum && !bounds && !independence)return 2;
    size_t bytes=maximum?UMI_AI_CORPUS_MAX_BYTES:257U;char path[1024];
    CHECK(fixture(argv[2],bytes,path));
    reset_observation(0);
    UmiAiCorpus *corpus=NULL;size_t length=0;
    CHECK(UmiAiCorpusLoadFile(path,&corpus)==UMI_STATUS_OK);
    CHECK(UmiAiCorpusLength(corpus,&length)==UMI_STATUS_OK && length==bytes);
    uint32_t inputs[2]={0},targets[2]={0};
    CHECK(UmiAiCorpusWindow(corpus,0,2,inputs,targets,2)==UMI_STATUS_OK);
    CHECK(inputs[0]==4U && inputs[1]==5U && targets[0]==5U && targets[1]==6U);
    const size_t observed_peak=peak_bytes,observed_calls=calls;
    unsigned char hash[32],again[32];
    CHECK(UmiAiCorpusSha256(corpus,hash)==UMI_STATUS_OK);
    if(independence){
        /* The test deletes only the path it just created exclusively. The
         * admitted corpus must remain valid without the original file. */
        CHECK(remove(path)==0);
        CHECK(UmiAiCorpusSha256(corpus,again)==UMI_STATUS_OK);
        CHECK(memcmp(hash,again,sizeof(hash))==0);
        CHECK(UmiAiCorpusWindow(corpus,254U,2U,inputs,targets,2U)==UMI_STATUS_OK);
        CHECK(inputs[0]==258U && inputs[1]==259U && targets[0]==259U && targets[1]==4U);
    }
    UmiAiCorpusDestroy(corpus);corpus=NULL;
    CHECK(live_bytes==0);
    printf("Payload %zu; peak corpus-owned bytes %zu; allocation calls %zu\n",bytes,observed_peak,observed_calls);
    if(allocations){
        for(size_t fail=1;fail<=observed_calls;++fail){
            reset_observation(fail);
            UmiAiCorpus *sentinel=(UmiAiCorpus *)(void *)&length;corpus=sentinel;
            CHECK(UmiAiCorpusLoadFile(path,&corpus)==UMI_STATUS_OUT_OF_MEMORY);
            CHECK(corpus==sentinel && live_bytes==0);
        }
    }
    if(!independence)CHECK(remove(path)==0);
    if(bounds){
        const size_t sizes[]={0U,UMI_AI_CORPUS_MAX_BYTES+1U};
        for(size_t i=0;i<2;++i){
            CHECK(fixture(argv[2],sizes[i],path));reset_observation(0);corpus=NULL;
            UmiStatus status=UmiAiCorpusLoadFile(path,&corpus);
            CHECK(remove(path)==0);
            CHECK(status==(i==0?UMI_STATUS_INVALID_ARGUMENT:UMI_STATUS_CAPACITY_EXCEEDED));
            CHECK(corpus==NULL && live_bytes==0 && calls==0);
        }
    }
    /* One admitted payload plus a small owner is the resource contract. This
     * excludes stdio buffering and is not a process-wide RSS measurement. */
    CHECK(observed_peak<=bytes+64U);
    return 0;
}
