/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai/test_ollama_pull_event.c
 *
 * PURPOSE:
 *   Exercise complete pull-event admission, copied metadata, numeric precision and atomic refusal.
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
static int Expected(const void *bytes,size_t length,UmiStatus expected,const UmiCancellationToken *cancel)
{
    UmiOllamaPullEvent event,before;memset(&event,0xa5,sizeof event);memcpy(&before,&event,sizeof before);
    UmiStatus status=UmiOllamaPullEventDecode(bytes,length,cancel,&event);
    CHECK(status==expected);
    if(status!=UMI_STATUS_OK)CHECK(memcmp(&event,&before,sizeof event)==0);
    return 0;
}
typedef struct Fixture{const char *name,*json;UmiStatus expected;} Fixture;
static const Fixture fixtures[]={
    {"status","{\"status\":\"pulling layer\"}",UMI_STATUS_OK},
    {"success","{\"status\":\"success\"}",UMI_STATUS_OK},
    {"error","{\"error\":\"network ended\"}",UMI_STATUS_OK},
    {"error-precedence","{\"status\":\"success\",\"error\":\"failed\"}",UMI_STATUS_OK},
    {"progress","{\"status\":\"pulling\",\"digest\":\"opaque:layer\",\"total\":9007199254740993,\"completed\":7}",UMI_STATUS_OK},
    {"known-zero","{\"status\":\"pulling\",\"total\":0,\"completed\":0}",UMI_STATUS_OK},
    {"completed-only","{\"status\":\"pulling\",\"completed\":3}",UMI_STATUS_OK},
    {"max-integer","{\"status\":\"pulling\",\"total\":9223372036854775807}",UMI_STATUS_OK},
    {"unknown","{\"status\":\"future phase\",\"extension\":{\"enabled\":true}}",UMI_STATUS_OK},
    {"unicode","{\"status\":\"café\"}",UMI_STATUS_OK},
    {"case-sensitive","{\"status\":\"SUCCESS\"}",UMI_STATUS_OK},
    {"empty-object","{}",UMI_STATUS_PARSE_ERROR},
    {"wrong-root","[]",UMI_STATUS_PARSE_ERROR},
    {"missing-status","{\"total\":5}",UMI_STATUS_PARSE_ERROR},
    {"empty-status","{\"status\":\"\"}",UMI_STATUS_PARSE_ERROR},
    {"empty-error","{\"error\":\"\"}",UMI_STATUS_PARSE_ERROR},
    {"empty-digest","{\"status\":\"pulling\",\"digest\":\"\"}",UMI_STATUS_PARSE_ERROR},
    {"null-status","{\"status\":null}",UMI_STATUS_PARSE_ERROR},
    {"wrong-error","{\"error\":true}",UMI_STATUS_PARSE_ERROR},
    {"wrong-digest","{\"status\":\"pulling\",\"digest\":7}",UMI_STATUS_PARSE_ERROR},
    {"negative","{\"status\":\"pulling\",\"total\":-1}",UMI_STATUS_PARSE_ERROR},
    {"fraction","{\"status\":\"pulling\",\"total\":1.5}",UMI_STATUS_PARSE_ERROR},
    {"exponent","{\"status\":\"pulling\",\"completed\":1e2}",UMI_STATUS_PARSE_ERROR},
    {"overflow","{\"status\":\"pulling\",\"total\":9223372036854775808}",UMI_STATUS_CAPACITY_EXCEEDED},
    {"counter-null","{\"status\":\"pulling\",\"total\":null}",UMI_STATUS_PARSE_ERROR},
    {"beyond-total","{\"status\":\"pulling\",\"total\":3,\"completed\":4}",UMI_STATUS_PARSE_ERROR},
    {"duplicate","{\"status\":\"a\",\"status\":\"success\"}",UMI_STATUS_ALREADY_EXISTS},
    {"escaped-duplicate","{\"status\":\"a\",\"sta\\u0074us\":\"b\"}",UMI_STATUS_ALREADY_EXISTS},
    {"nested-duplicate","{\"status\":\"a\",\"other\":{\"x\":1,\"x\":2}}",UMI_STATUS_ALREADY_EXISTS},
    {"trailing","{\"status\":\"success\"}x",UMI_STATUS_PARSE_ERROR},
    {"decoded-nul","{\"status\":\"a\\u0000b\"}",UMI_STATUS_PARSE_ERROR},
    {"unpaired","{\"status\":\"\\ud800\"}",UMI_STATUS_PARSE_ERROR},
};
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    for(size_t i=0U;i<sizeof fixtures/sizeof fixtures[0];++i){
        if(strcmp(mode,fixtures[i].name)!=0)continue;
        CHECK(Expected(fixtures[i].json,strlen(fixtures[i].json),fixtures[i].expected,NULL)==0);
        if(fixtures[i].expected!=UMI_STATUS_OK)return 0;
        UmiOllamaPullEvent event={0};CHECK(UmiOllamaPullEventDecode(fixtures[i].json,strlen(fixtures[i].json),NULL,&event)==UMI_STATUS_OK);
        if(strcmp(mode,"success")==0)CHECK(event.kind==UMI_OLLAMA_PULL_SUCCESS);
        else if(strcmp(mode,"error")==0 || strcmp(mode,"error-precedence")==0)CHECK(event.kind==UMI_OLLAMA_PULL_ERROR);
        else CHECK(event.kind==UMI_OLLAMA_PULL_STATUS);
        if(strcmp(mode,"progress")==0){CHECK(event.total==UINT64_C(9007199254740993));CHECK(event.completed==7U);CHECK(event.known==3U);CHECK(strcmp(event.digest,"opaque:layer")==0);}
        if(strcmp(mode,"known-zero")==0){CHECK(event.known==3U);CHECK(event.total==0U && event.completed==0U);}
        if(strcmp(mode,"completed-only")==0)CHECK(event.known==UMI_OLLAMA_PULL_COMPLETED_KNOWN && event.completed==3U);
        if(strcmp(mode,"max-integer")==0)CHECK(event.total==INT64_MAX);
        if(strcmp(mode,"status")==0)CHECK(event.known==0U && event.digest[0]=='\0');
        if(strcmp(mode,"unicode")==0)CHECK(strcmp(event.status,"caf\xc3\xa9")==0);
        return 0;
    }
    char buffer[UMI_OLLAMA_PULL_LINE_BYTES+2U];const char *success="{\"status\":\"success\"}";
    if(strcmp(mode,"text-limit")==0 || strcmp(mode,"error-limit")==0 || strcmp(mode,"digest-limit")==0){
        const char *key=strcmp(mode,"text-limit")==0?"status":strcmp(mode,"error-limit")==0?"error":"digest";
        size_t limit=strcmp(key,"status")==0?256U:strcmp(key,"error")==0?1024U:128U;
        int written=snprintf(buffer,sizeof buffer,"{\"status\":\"pulling\",\"%s\":\"",key);
        /* Status needs a single key; duplicate keys must not hide the text bound. */
        if(strcmp(key,"status")==0)written=snprintf(buffer,sizeof buffer,"{\"status\":\"");
        CHECK(written>0);size_t n=(size_t)written;memset(buffer+n,'x',limit);memcpy(buffer+n+limit,"\"}",3U);
        CHECK(Expected(buffer,strlen(buffer),UMI_STATUS_OK,NULL)==0);
        buffer[n+limit]='x';memcpy(buffer+n+limit+1U,"\"}",3U);
        return Expected(buffer,strlen(buffer),UMI_STATUS_CAPACITY_EXCEEDED,NULL);
    }
    if(strcmp(mode,"byte-limit")==0 || strcmp(mode,"exact-byte-limit")==0){
        memset(buffer,' ',sizeof buffer);memcpy(buffer,success,strlen(success));
        return Expected(buffer,UMI_OLLAMA_PULL_LINE_BYTES+(strcmp(mode,"byte-limit")==0?1U:0U),strcmp(mode,"byte-limit")==0?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK,NULL);
    }
    if(strcmp(mode,"depth")==0){size_t n=0U;for(size_t i=0;i<20U;++i)buffer[n++]='[';buffer[n++]='0';for(size_t i=0;i<20U;++i)buffer[n++]=']';return Expected(buffer,n,UMI_STATUS_CAPACITY_EXCEEDED,NULL);}
    if(strcmp(mode,"nodes")==0){size_t n=(size_t)snprintf(buffer,sizeof buffer,"{\"status\":\"a\",\"extra\":[0");for(size_t i=0;i<512U;++i){buffer[n++]=',';buffer[n++]='0';}buffer[n++]=']';buffer[n++]='}';return Expected(buffer,n,UMI_STATUS_CAPACITY_EXCEEDED,NULL);}
    if(strcmp(mode,"embedded-nul")==0){memcpy(buffer,success,strlen(success));buffer[5]='\0';return Expected(buffer,strlen(success),UMI_STATUS_PARSE_ERROR,NULL);}
    if(strcmp(mode,"invalid-utf8")==0){const unsigned char bad[]={ '{','"','s','t','a','t','u','s','"',':','"',0xff,'"','}' };return Expected(bad,sizeof bad,UMI_STATUS_PARSE_ERROR,NULL);}
    if(strcmp(mode,"cancel")==0){UmiCancellationToken *token=NULL;CHECK(umi_cancellation_token_create(&token)==UMI_STATUS_OK);umi_cancellation_token_request(token);int result=Expected(success,strlen(success),UMI_STATUS_CANCELLED,token);umi_cancellation_token_destroy(token);return result;}
    if(strcmp(mode,"nulls")==0){CHECK(Expected(NULL,1U,UMI_STATUS_INVALID_ARGUMENT,NULL)==0);CHECK(Expected(success,0U,UMI_STATUS_INVALID_ARGUMENT,NULL)==0);CHECK(UmiOllamaPullEventDecode(success,strlen(success),NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);return 0;}
    if(strcmp(mode,"ownership")==0){strcpy(buffer,success);UmiOllamaPullEvent event={0};CHECK(UmiOllamaPullEventDecode(buffer,strlen(buffer),NULL,&event)==UMI_STATUS_OK);memset(buffer,'x',strlen(buffer));CHECK(strcmp(event.status,"success")==0);return 0;}
    return 2;
}
