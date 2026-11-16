/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/ollama_pull/main.c
 *
 * PURPOSE:
 *   Teach streamed model-status observations using only installed Framework public APIs and synthetic input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/ollama_pull.h"
#include "umicom/ai/ollama_pull_file.h"
#include "umicom/platform/native_arguments.h"
#include <stdio.h>
#include <string.h>

typedef struct Progress {size_t events;} Progress;
static UmiStatus Observe(const UmiOllamaPullEvent *event,void *context)
{
    Progress *progress=context;++progress->events;
    /* Display a stable category, not untrusted provider text as a format string.
     * Byte counters concern the current event, not a sum across model layers. */
    const char *kind=event->kind==UMI_OLLAMA_PULL_SUCCESS?"reported success":
        event->kind==UMI_OLLAMA_PULL_ERROR?"reported error":"progress";
    printf("event %zu: %s\n",progress->events,kind);return UMI_STATUS_OK;
}
/* The original no-argument sample remains callable; the entry now uses Framework UTF-8 argument decoding so the installed SDK can inspect chosen files too. The former implementation is retained for owner review. */
#if 0
int main(void)
#endif
static int ProgressSample(void)
{
    const char transcript[]="{\"status\":\"pulling layer\",\"digest\":\"synthetic-layer\",\"total\":100,\"completed\":25}\n{\"status\":\"success\"}\n";
    UmiOllamaPullStream *stream=NULL;Progress progress={0};
    UmiStatus status=UmiOllamaPullStreamCreate(Observe,&progress,&stream);
    /* Transport chunks need not align with lines, JSON tokens or UTF-8 code
     * points. This small example deliberately feeds seven bytes at a time. */
    for(size_t offset=0U;status==UMI_STATUS_OK && offset<sizeof transcript-1U;){
        size_t remaining=sizeof transcript-1U-offset,chunk=remaining<7U?remaining:7U;
        status=UmiOllamaPullStreamFeed(stream,transcript+offset,chunk,NULL);offset+=chunk;
    }
    if(status==UMI_STATUS_OK)status=UmiOllamaPullStreamFinish(stream,NULL);
    UmiOllamaPullSnapshot view={0};
    if(status==UMI_STATUS_OK)status=UmiOllamaPullStreamRead(stream,&view);
    UmiStatus release=UmiOllamaPullStreamRelease(&stream);
    if(status!=UMI_STATUS_OK || release!=UMI_STATUS_OK || view.events!=2U || !view.input_finished)return 1;
    puts("Synthetic stream complete; no model downloaded or verified.");
    /* A truncated response is refused even when its earlier progress was valid. */
    status=UmiOllamaPullStreamCreate(NULL,NULL,&stream);
    const char incomplete[]="{\"status\":\"pulling layer\"}\n";
    if(status==UMI_STATUS_OK)status=UmiOllamaPullStreamFeed(stream,incomplete,strlen(incomplete),NULL);
    if(status==UMI_STATUS_OK)status=UmiOllamaPullStreamFinish(stream,NULL);
    release=UmiOllamaPullStreamRelease(&stream);
    if(status!=UMI_STATUS_PARSE_ERROR || release!=UMI_STATUS_OK)return 1;
    puts("Missing terminal record refused.");return 0;
}

static int ProgressCommand(int argc,char **argv)
{
    if(argc==1)return ProgressSample();
    if(argc>=2 && strcmp(argv[1],"models")==0)return UmiOllamaPullFileCommand(argc-2,argv+2);
    fputs("Use no arguments for the synthetic sample, or models inspect-pull ABSOLUTE_NDJSON.\n",stderr);return 2;
}
int main(int argc,char **argv)
{
    int result=1;UmiStatus status=UmiNativeArgumentsDispatch(argc,argv,ProgressCommand,&result);
    return status==UMI_STATUS_OK?result:1;
}
