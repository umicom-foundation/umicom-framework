/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai/test_ollama_pull_stream.c
 *
 * PURPOSE:
 *   Verify framing, prefix observation, terminal evidence, cancellation and callback lifetime under adverse input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/ollama_pull.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static const char progress[]="{\"status\":\"pulling caf\xc3\xa9\",\"total\":8,\"completed\":4}\n";
static const char success[]="{\"status\":\"success\"}\n";
static const char error[]="{\"error\":\"failed\"}\n";
typedef struct Observer {
    UmiOllamaPullStream *stream;
    UmiCancellationToken *cancel;
    size_t count;
    int mode, bad;
    UmiOllamaPullEvent copy;
} Observer;
static UmiStatus Observe(const UmiOllamaPullEvent *event,void *context)
{
    Observer *owner=context;owner->copy=*event;++owner->count;
    UmiOllamaPullSnapshot view;
    if(UmiOllamaPullStreamRead(owner->stream,&view)!=UMI_STATUS_OK || view.events!=owner->count ||
       !view.has_event || view.last.kind!=event->kind)owner->bad=1;
    if(owner->mode==1)return UMI_STATUS_UNAVAILABLE;
    if(owner->mode==2){
        if(UmiOllamaPullStreamFeed(owner->stream,NULL,0U,NULL)!=UMI_STATUS_BUSY ||
            UmiOllamaPullStreamFinish(owner->stream,NULL)!=UMI_STATUS_BUSY ||
            UmiOllamaPullStreamRelease(&owner->stream)!=UMI_STATUS_BUSY || owner->stream==NULL)owner->bad=1;
    }
    if(owner->mode==3)umi_cancellation_token_request(owner->cancel);
    return UMI_STATUS_OK;
}
static int Exercise(const char *mode)
{
    Observer owner={0};CHECK(UmiOllamaPullStreamCreate(Observe,&owner,&owner.stream)==UMI_STATUS_OK);
    UmiStatus status=UMI_STATUS_OK,expected=UMI_STATUS_OK;UmiOllamaPullSnapshot view;
    char input[512];snprintf(input,sizeof input,"%s%s",progress,success);
    if(strcmp(mode,"every-split")==0){
        CHECK(UmiOllamaPullStreamRelease(&owner.stream)==UMI_STATUS_OK);
        for(size_t split=0U;split<=strlen(input);++split){
            owner.count=0U;CHECK(UmiOllamaPullStreamCreate(Observe,&owner,&owner.stream)==UMI_STATUS_OK);
            CHECK(UmiOllamaPullStreamFeed(owner.stream,input,split,NULL)==UMI_STATUS_OK);
            CHECK(UmiOllamaPullStreamFeed(owner.stream,input+split,strlen(input)-split,NULL)==UMI_STATUS_OK);
            CHECK(UmiOllamaPullStreamFinish(owner.stream,NULL)==UMI_STATUS_OK);CHECK(owner.count==2U && !owner.bad);
            CHECK(UmiOllamaPullStreamRelease(&owner.stream)==UMI_STATUS_OK);
        }return 0;
    }
    if(strcmp(mode,"whole")==0 || strcmp(mode,"callback-read")==0 || strcmp(mode,"reentrant")==0){
        owner.mode=strcmp(mode,"reentrant")==0?2:0;
        CHECK(UmiOllamaPullStreamFeed(owner.stream,input,strlen(input),NULL)==UMI_STATUS_OK);
        CHECK(UmiOllamaPullStreamRead(owner.stream,&view)==UMI_STATUS_OK);CHECK(view.terminal_seen && !view.input_finished);
        CHECK(UmiOllamaPullStreamFinish(owner.stream,NULL)==UMI_STATUS_OK);CHECK(owner.count==2U);
    }else if(strcmp(mode,"bytewise")==0){for(size_t i=0U;i<strlen(input);++i)CHECK(UmiOllamaPullStreamFeed(owner.stream,input+i,1U,NULL)==UMI_STATUS_OK);CHECK(UmiOllamaPullStreamFinish(owner.stream,NULL)==UMI_STATUS_OK);CHECK(owner.count==2U);
    }else if(strcmp(mode,"crlf")==0 || strcmp(mode,"blank-lines")==0){const char *text="\r\n\n{\"status\":\"success\"}\r\n\r\n";CHECK(UmiOllamaPullStreamFeed(owner.stream,text,strlen(text),NULL)==UMI_STATUS_OK);CHECK(UmiOllamaPullStreamFinish(owner.stream,NULL)==UMI_STATUS_OK);CHECK(owner.count==1U);
    }else if(strcmp(mode,"no-final-lf")==0){CHECK(UmiOllamaPullStreamFeed(owner.stream,success,strlen(success)-1U,NULL)==UMI_STATUS_OK);CHECK(owner.count==0U);CHECK(UmiOllamaPullStreamFinish(owner.stream,NULL)==UMI_STATUS_OK);CHECK(owner.count==1U);
    }else if(strcmp(mode,"missing-terminal")==0 || strcmp(mode,"empty")==0){if(strcmp(mode,"missing-terminal")==0)CHECK(UmiOllamaPullStreamFeed(owner.stream,progress,strlen(progress),NULL)==UMI_STATUS_OK);status=UmiOllamaPullStreamFinish(owner.stream,NULL);expected=UMI_STATUS_PARSE_ERROR;
    }else if(strcmp(mode,"provider-error")==0){CHECK(UmiOllamaPullStreamFeed(owner.stream,error,strlen(error),NULL)==UMI_STATUS_OK);status=UmiOllamaPullStreamFinish(owner.stream,NULL);expected=UMI_STATUS_IO_ERROR;CHECK(owner.count==1U && owner.copy.kind==UMI_OLLAMA_PULL_ERROR);
    }else if(strcmp(mode,"after-success")==0 || strcmp(mode,"after-error")==0){const char *first=strcmp(mode,"after-success")==0?success:error;CHECK(UmiOllamaPullStreamFeed(owner.stream,first,strlen(first),NULL)==UMI_STATUS_OK);status=UmiOllamaPullStreamFeed(owner.stream,progress,strlen(progress),NULL);expected=UMI_STATUS_PARSE_ERROR;CHECK(owner.count==1U);
    }else if(strcmp(mode,"late-malformed")==0 || strcmp(mode,"failure-latched")==0){CHECK(UmiOllamaPullStreamFeed(owner.stream,progress,strlen(progress),NULL)==UMI_STATUS_OK);status=UmiOllamaPullStreamFeed(owner.stream,"bad\n",4U,NULL);expected=UMI_STATUS_PARSE_ERROR;CHECK(owner.count==1U);CHECK(UmiOllamaPullStreamFeed(owner.stream,success,strlen(success),NULL)==expected);CHECK(UmiOllamaPullStreamFinish(owner.stream,NULL)==expected);
    }else if(strcmp(mode,"truncated")==0){CHECK(UmiOllamaPullStreamFeed(owner.stream,"{",1U,NULL)==UMI_STATUS_OK);status=UmiOllamaPullStreamFinish(owner.stream,NULL);expected=UMI_STATUS_PARSE_ERROR;
    }else if(strcmp(mode,"whitespace-line")==0){status=UmiOllamaPullStreamFeed(owner.stream," \n",2U,NULL);expected=UMI_STATUS_PARSE_ERROR;
    }else if(strcmp(mode,"callback-failure")==0){owner.mode=1;status=UmiOllamaPullStreamFeed(owner.stream,input,strlen(input),NULL);expected=UMI_STATUS_UNAVAILABLE;CHECK(owner.count==1U);
    }else if(strcmp(mode,"cancel-before")==0 || strcmp(mode,"cancel-callback")==0 || strcmp(mode,"cancel-finish")==0){
        CHECK(umi_cancellation_token_create(&owner.cancel)==UMI_STATUS_OK);
        if(strcmp(mode,"cancel-callback")==0){owner.mode=3;status=UmiOllamaPullStreamFeed(owner.stream,input,strlen(input),owner.cancel);CHECK(owner.count==1U);}
        else{if(strcmp(mode,"cancel-finish")==0)CHECK(UmiOllamaPullStreamFeed(owner.stream,success,strlen(success),NULL)==UMI_STATUS_OK);umi_cancellation_token_request(owner.cancel);status=strcmp(mode,"cancel-before")==0?UmiOllamaPullStreamFeed(owner.stream,input,strlen(input),owner.cancel):UmiOllamaPullStreamFinish(owner.stream,owner.cancel);}
        expected=UMI_STATUS_CANCELLED;umi_cancellation_token_destroy(owner.cancel);owner.cancel=NULL;
    }else if(strcmp(mode,"ownership")==0){CHECK(UmiOllamaPullStreamFeed(owner.stream,input,strlen(input),NULL)==UMI_STATUS_OK);memset(input,'x',strlen(input));CHECK(UmiOllamaPullStreamFinish(owner.stream,NULL)==UMI_STATUS_OK);CHECK(strcmp(owner.copy.status,"success")==0);
    }else if(strcmp(mode,"nulls")==0){
        CHECK(UmiOllamaPullStreamFeed(NULL,NULL,0U,NULL)==UMI_STATUS_INVALID_ARGUMENT);CHECK(UmiOllamaPullStreamFeed(owner.stream,NULL,1U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiOllamaPullStreamFeed(owner.stream,NULL,0U,NULL)==UMI_STATUS_OK);CHECK(UmiOllamaPullStreamFinish(NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiOllamaPullStreamRead(NULL,&view)==UMI_STATUS_INVALID_ARGUMENT);CHECK(UmiOllamaPullStreamRead(owner.stream,NULL)==UMI_STATUS_INVALID_ARGUMENT);CHECK(UmiOllamaPullStreamRelease(NULL)==UMI_STATUS_INVALID_ARGUMENT);CHECK(UmiOllamaPullStreamCreate(NULL,NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiOllamaPullStreamRead(owner.stream,&view)==UMI_STATUS_OK);CHECK(view.events==0U && view.bytes_admitted==0U && view.failure==UMI_STATUS_OK);
    }else if(strcmp(mode,"occupied")==0){UmiOllamaPullStream *before=owner.stream;CHECK(UmiOllamaPullStreamCreate(NULL,NULL,&owner.stream)==UMI_STATUS_INVALID_ARGUMENT);CHECK(owner.stream==before);
    }else if(strcmp(mode,"release-empty")==0){CHECK(UmiOllamaPullStreamRelease(&owner.stream)==UMI_STATUS_OK);CHECK(UmiOllamaPullStreamRelease(&owner.stream)==UMI_STATUS_OK);return 0;
    }else if(strcmp(mode,"finish-twice")==0 || strcmp(mode,"feed-after-finish")==0){CHECK(UmiOllamaPullStreamFeed(owner.stream,success,strlen(success),NULL)==UMI_STATUS_OK);CHECK(UmiOllamaPullStreamFinish(owner.stream,NULL)==UMI_STATUS_OK);CHECK(UmiOllamaPullStreamFinish(owner.stream,NULL)==UMI_STATUS_OK);CHECK(UmiOllamaPullStreamFeed(owner.stream,NULL,0U,NULL)==UMI_STATUS_INVALID_STATE);CHECK(owner.count==1U);
    }else if(strcmp(mode,"line-limit")==0 || strcmp(mode,"exact-line")==0){
        char *line=malloc(UMI_OLLAMA_PULL_LINE_BYTES+1U);CHECK(line!=NULL);memset(line,' ',UMI_OLLAMA_PULL_LINE_BYTES+1U);
        if(strcmp(mode,"exact-line")==0){memcpy(line,success,strlen(success)-1U);line[UMI_OLLAMA_PULL_LINE_BYTES]='\n';CHECK(UmiOllamaPullStreamFeed(owner.stream,line,UMI_OLLAMA_PULL_LINE_BYTES+1U,NULL)==UMI_STATUS_OK);CHECK(UmiOllamaPullStreamFinish(owner.stream,NULL)==UMI_STATUS_OK);}
        else{status=UmiOllamaPullStreamFeed(owner.stream,line,UMI_OLLAMA_PULL_LINE_BYTES+1U,NULL);expected=UMI_STATUS_CAPACITY_EXCEEDED;}free(line);
    }else if(strcmp(mode,"stream-limit")==0){char blanks[4096];memset(blanks,'\n',sizeof blanks);for(size_t i=0U;i<UMI_OLLAMA_PULL_STREAM_BYTES/sizeof blanks;++i)CHECK(UmiOllamaPullStreamFeed(owner.stream,blanks,sizeof blanks,NULL)==UMI_STATUS_OK);status=UmiOllamaPullStreamFeed(owner.stream,"\n",1U,NULL);expected=UMI_STATUS_CAPACITY_EXCEEDED;CHECK(owner.count==0U);
    }else if(strcmp(mode,"event-limit")==0 || strcmp(mode,"max-events")==0){
        const char *tick="{\"status\":\"pulling\"}\n";size_t count=UMI_OLLAMA_PULL_MAX_EVENTS-(strcmp(mode,"max-events")==0?1U:0U);
        for(size_t i=0U;i<count;++i)CHECK(UmiOllamaPullStreamFeed(owner.stream,tick,strlen(tick),NULL)==UMI_STATUS_OK);
        status=UmiOllamaPullStreamFeed(owner.stream,success,strlen(success),NULL);expected=strcmp(mode,"max-events")==0?UMI_STATUS_OK:UMI_STATUS_CAPACITY_EXCEEDED;
        if(expected==UMI_STATUS_OK)CHECK(UmiOllamaPullStreamFinish(owner.stream,NULL)==UMI_STATUS_OK);CHECK(owner.count==UMI_OLLAMA_PULL_MAX_EVENTS);
    }else{CHECK(UmiOllamaPullStreamRelease(&owner.stream)==UMI_STATUS_OK);return 2;}
    CHECK(status==expected);CHECK(!owner.bad);
    CHECK(UmiOllamaPullStreamRead(owner.stream,&view)==UMI_STATUS_OK);
    if(expected!=UMI_STATUS_OK){CHECK(view.failure==expected);CHECK(view.pending_bytes==0U);}
    CHECK(UmiOllamaPullStreamRelease(&owner.stream)==UMI_STATUS_OK);CHECK(owner.stream==NULL);return 0;
}
int main(int argc,char **argv){CHECK(argc==2);return Exercise(argv[1]);}
