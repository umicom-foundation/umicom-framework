/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai/test_stream_collector.c
 *
 * PURPOSE:
 *   Exercise bounded legacy collector state, aliases and callback-owned chunk lifetime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/stream.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"%d: %s\n",__LINE__,#c);return 1;}}while(0)
typedef struct SinkState {
    char chunk[UMI_AI_TEXT_CAPACITY*4U];
    unsigned calls;
    int final;
    UmiAiStreamCollector *owner;
    int reset,nested,valid;
} SinkState;
static void Observe(const char *text,int final,void *data)
{
    SinkState *state=data;
    ++state->calls;state->final=final;
    size_t length=strlen(text);
    if(length>=sizeof state->chunk){state->valid=0;return;}
    memcpy(state->chunk,text,length+1U);
    if(state->reset){
        umi_ai_stream_init(state->owner,NULL,NULL);
        if(strcmp(text,state->chunk)!=0)state->valid=0;
    }
    if(state->nested){
        state->nested=0;
        /* Avoid a recursive observer, but retain the outer borrowed chunk. */
        state->owner->sink=NULL;
        if(umi_ai_stream_push(state->owner,"z",0)!=UMI_STATUS_OK ||
            strcmp(text,state->chunk)!=0)state->valid=0;
    }
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const char *mode=argv[1];
    UmiAiStreamCollector stream={0};SinkState sink={.owner=&stream,.valid=1};
    umi_ai_stream_init(&stream,Observe,&sink);
    if(strcmp(mode,"arguments")==0){
        CHECK(umi_ai_stream_push(NULL,"x",0)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_stream_push(&stream,NULL,0)==UMI_STATUS_INVALID_ARGUMENT);
        umi_ai_stream_init(NULL,NULL,NULL);CHECK(sink.calls==0U);return 0;
    }
    const char *input="abc";int final=0;UmiStatus expected=UMI_STATUS_OK;
    char large[sizeof stream.text];memset(large,'x',sizeof large);
    if(strcmp(mode,"empty")==0)input="";
    else if(strcmp(mode,"exact")==0){large[sizeof large-1U]='\0';input=large;}
    else if(strcmp(mode,"overflow")==0){
        CHECK(umi_ai_stream_push(&stream,"a",0)==UMI_STATUS_OK);
        large[sizeof large-1U]='\0';input=large;expected=UMI_STATUS_CAPACITY_EXCEEDED;
    }else if(strcmp(mode,"unterminated")==0){input=large;expected=UMI_STATUS_CAPACITY_EXCEEDED;}
    else if(strcmp(mode,"corrupt-size")==0){stream.length=SIZE_MAX;expected=UMI_STATUS_INVALID_STATE;}
    else if(strcmp(mode,"corrupt-terminator")==0){stream.text[0]='x';expected=UMI_STATUS_INVALID_STATE;}
    else if(strcmp(mode,"embedded-terminator")==0){stream.length=2U;stream.text[2]='\0';expected=UMI_STATUS_INVALID_STATE;}
    else if(strncmp(mode,"alias-",6U)==0){
        CHECK(umi_ai_stream_push(&stream,"abc",0)==UMI_STATUS_OK);
        input=stream.text+(strcmp(mode,"alias-slice")==0?1U:0U);
    }else if(strcmp(mode,"callback-reset")==0)sink.reset=1;
    else if(strcmp(mode,"callback-nested")==0)sink.nested=1;
    else if(strcmp(mode,"final-forwarding")==0)final=42;
    else if(strcmp(mode,"append")!=0)return 2;
    UmiAiStreamCollector before=stream;unsigned calls=sink.calls;
    CHECK(umi_ai_stream_push(&stream,input,final)==expected);
    CHECK(sink.valid);
    if(expected!=UMI_STATUS_OK){
        CHECK(memcmp(&before,&stream,sizeof stream)==0 && sink.calls==calls);
        return 0;
    }
    CHECK(sink.calls==calls+1U && sink.final==final);
    if(strcmp(mode,"exact")==0)CHECK(stream.length==sizeof stream.text-1U && strlen(sink.chunk)==stream.length);
    else if(strcmp(mode,"alias-self")==0)CHECK(strcmp(stream.text,"abcabc")==0 && strcmp(sink.chunk,"abc")==0);
    else if(strcmp(mode,"alias-slice")==0)CHECK(strcmp(stream.text,"abcbc")==0 && strcmp(sink.chunk,"bc")==0);
    else if(strcmp(mode,"callback-reset")==0)CHECK(stream.length==0U && strcmp(sink.chunk,"abc")==0);
    else if(strcmp(mode,"callback-nested")==0)CHECK(strcmp(stream.text,"abcz")==0 && strcmp(sink.chunk,"abc")==0);
    else CHECK(strcmp(stream.text,input)==0);
    return 0;
}
