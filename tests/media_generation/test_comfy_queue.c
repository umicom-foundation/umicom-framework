/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_comfy_queue.c
 *
 * PURPOSE:
 *   Verify queue wire grammar, identity uniqueness, copied lifetime and adverse resource boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;}}while(0)
typedef struct Fixture {const char *name,*json;UmiStatus expected;size_t count;} Fixture;
static const Fixture fixtures[]={
    {"complete","{\"queue_running\":[[7,\"z-running\",{},{},[]]],\"queue_pending\":[[20,\"b-pending\",{},{},[]],[3,\"a-pending\",{},{},[]]]}",UMI_STATUS_OK,3U},
    {"empty","{\"queue_running\":[],\"queue_pending\":[]}",UMI_STATUS_OK,0U},
    {"front","{\"queue_running\":[[-3,\"job-a\",{},{},[]]],\"queue_pending\":[]}",UMI_STATUS_OK,1U},
    {"fraction","{\"queue_running\":[[0.125,\"job-a\",{},{},[]]],\"queue_pending\":[]}",UMI_STATUS_OK,1U},
    {"exponent","{\"queue_running\":[[2.5e+2,\"job-a\",{},{},[]]],\"queue_pending\":[]}",UMI_STATUS_OK,1U},
    {"large-integer","{\"queue_running\":[[9223372036854775808,\"job-a\",{},{},[]]],\"queue_pending\":[]}",UMI_STATUS_OK,1U},
    {"large-exponent","{\"queue_running\":[[1e9999,\"job-a\",{},{},[]]],\"queue_pending\":[]}",UMI_STATUS_OK,1U},
    {"number-boundary","{\"queue_running\":[[11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111,\"job-a\",{},{},[]]],\"queue_pending\":[]}",UMI_STATUS_OK,1U},
    {"number-limit","{\"queue_running\":[[111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111,\"job-a\",{},{},[]]],\"queue_pending\":[]}",UMI_STATUS_CAPACITY_EXCEEDED,1U},
    {"id-boundary","{\"queue_running\":[[1,\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\",{},{},[]]],\"queue_pending\":[]}",UMI_STATUS_OK,1U},
    {"id-limit","{\"queue_running\":[[1,\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\",{},{},[]]],\"queue_pending\":[]}",UMI_STATUS_CAPACITY_EXCEEDED,1U},
    {"unicode","{\"queue_running\":[[1,\"café-進捗\",{},{},[]]],\"queue_pending\":[]}",UMI_STATUS_OK,1U},
    {"missing-running","{\"queue_pending\":[]}",UMI_STATUS_PARSE_ERROR,1U},
    {"missing-pending","{\"queue_running\":[]}",UMI_STATUS_PARSE_ERROR,1U},
    {"wrong-root","[]",UMI_STATUS_PARSE_ERROR,1U},
    {"wrong-list","{\"queue_running\":{},\"queue_pending\":[]}",UMI_STATUS_PARSE_ERROR,1U},
    {"row-type","{\"queue_running\":[{}],\"queue_pending\":[]}",UMI_STATUS_PARSE_ERROR,1U},
    {"row-short","{\"queue_running\":[[1,\"a\",{},{}]],\"queue_pending\":[]}",UMI_STATUS_PARSE_ERROR,1U},
    {"number-string","{\"queue_running\":[[\"1\",\"job-a\",{},{},[]]],\"queue_pending\":[]}",UMI_STATUS_PARSE_ERROR,1U},
    {"number-bool","{\"queue_running\":[[true,\"job-a\",{},{},[]]],\"queue_pending\":[]}",UMI_STATUS_PARSE_ERROR,1U},
    {"number-null","{\"queue_running\":[[null,\"job-a\",{},{},[]]],\"queue_pending\":[]}",UMI_STATUS_PARSE_ERROR,1U},
    {"empty-id","{\"queue_running\":[[1,\"\",{},{},[]]],\"queue_pending\":[]}",UMI_STATUS_PARSE_ERROR,1U},
    {"id-type","{\"queue_running\":[[1,7,{},{},[]]],\"queue_pending\":[]}",UMI_STATUS_PARSE_ERROR,1U},
    {"prompt-type","{\"queue_running\":[[1,\"a\",[],{},[]]],\"queue_pending\":[]}",UMI_STATUS_PARSE_ERROR,1U},
    {"extra-type","{\"queue_running\":[[1,\"a\",{},null,[]]],\"queue_pending\":[]}",UMI_STATUS_PARSE_ERROR,1U},
    {"outputs-type","{\"queue_running\":[[1,\"a\",{},{},{}]],\"queue_pending\":[]}",UMI_STATUS_PARSE_ERROR,1U},
    {"output-nonstring","{\"queue_running\":[[1,\"a\",{},{},[1]]],\"queue_pending\":[]}",UMI_STATUS_PARSE_ERROR,1U},
    {"empty-output","{\"queue_running\":[[1,\"a\",{},{},[\"\"]]],\"queue_pending\":[]}",UMI_STATUS_PARSE_ERROR,1U},
    {"trailing","{\"queue_running\":[],\"queue_pending\":[]}{}",UMI_STATUS_PARSE_ERROR,1U},
    {"decoded-nul","{\"queue_running\":[[1,\"a\\u0000b\",{},{},[]]],\"queue_pending\":[]}",UMI_STATUS_PARSE_ERROR,1U},
    {"unpaired","{\"queue_running\":[[1,\"\\ud800\",{},{},[]]],\"queue_pending\":[]}",UMI_STATUS_PARSE_ERROR,1U},
    {"extensions","{\"queue_running\":[[1,\"job-a\",{\"private\":\"secret-prompt\"},{\"token\":\"secret-extra\"},[\"out\"],{\"future\":true}]],\"queue_pending\":[],\"future\":{\"version\":1}}",UMI_STATUS_OK,1U},
    {"duplicate-id","{\"queue_running\":[[1,\"job-a\",{},{},[]],[1,\"job-a\",{},{},[]]],\"queue_pending\":[]}",UMI_STATUS_ALREADY_EXISTS,1U},
    {"escaped-duplicate","{\"queue_running\":[[1,\"a\",{},{},[]],[2,\"\\u0061\",{},{},[]]],\"queue_pending\":[]}",UMI_STATUS_ALREADY_EXISTS,1U},
    {"cross-duplicate","{\"queue_running\":[[1,\"job-a\",{},{},[]]],\"queue_pending\":[[1,\"job-a\",{},{},[]]]}",UMI_STATUS_ALREADY_EXISTS,1U},
    {"case-sensitive","{\"queue_running\":[[1,\"A\",{},{},[]],[2,\"a\",{},{},[]]],\"queue_pending\":[]}",UMI_STATUS_OK,2U},
    {"duplicate-key","{\"queue_running\":[],\"queue_running\":[],\"queue_pending\":[]}",UMI_STATUS_ALREADY_EXISTS,1U},
    {"nested-duplicate","{\"queue_running\":[[1,\"a\",{\"x\":1,\"x\":2},{},[]]],\"queue_pending\":[]}",UMI_STATUS_ALREADY_EXISTS,1U},
};
static UmiProviderConnection Connection(void)
{
    UmiProviderConnection c={0};strcpy(c.id,"queue-fixture");strcpy(c.provider_id,"comfyui");
    strcpy(c.label,"Queue fixture");strcpy(c.endpoint,"http://127.0.0.1:8188");
    c.enabled=true;c.route=UMI_PROVIDER_CONNECTION_LOOPBACK;c.timeout_ms=30000;return c;
}
static int Expect(const void *bytes,size_t length,UmiStatus expected,size_t count)
{
    UmiProviderConnection c=Connection();UmiComfyQueue *queue=NULL;
    UmiStatus status=UmiComfyQueueCreate(&c,1U,bytes,length,NULL,&queue);CHECK(status==expected);
    if(status!=UMI_STATUS_OK){CHECK(queue==NULL);return 0;}
    CHECK(UmiComfyQueueCount(queue)==count);UmiComfyQueueReview review={0};
    CHECK(UmiComfyQueueInspect(queue,&review)==UMI_STATUS_OK);
    CHECK(review.running_count+review.pending_count==count && review.response_bytes==length);
    CHECK(UmiComfyQueueCheckConnection(queue,&c,1U)==UMI_STATUS_OK);
    for(size_t i=0U;i<count;++i){UmiComfyQueueEntry item={0};size_t found=SIZE_MAX;
        CHECK(UmiComfyQueueAt(queue,i,&item)==UMI_STATUS_OK);
        CHECK(UmiComfyQueueFind(queue,item.prompt_id,&found)==UMI_STATUS_OK && found==i);
        CHECK(item.state==(i<review.running_count?UMI_COMFY_QUEUE_RUNNING:UMI_COMFY_QUEUE_PENDING));
    }
    UmiComfyQueueDestroy(queue);return 0;
}
static char *Many(size_t count)
{
    size_t capacity=count*80U+128U,used=0U;char *json=malloc(capacity);if(!json)return NULL;
    strcpy(json,"{\"queue_running\":[],\"queue_pending\":[");used=strlen(json);
    for(size_t i=0U;i<count;++i){int n=snprintf(json+used,capacity-used,"%s[%zu,\"job-%zu\",{},{},[]]",i?",":"",i,i);
        if(n<0 || (size_t)n>=capacity-used){free(json);return NULL;}used+=(size_t)n;
    }
    strcpy(json+used,"]}");return json;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    for(size_t i=0U;i<sizeof fixtures/sizeof fixtures[0];++i)if(strcmp(mode,fixtures[i].name)==0){
        int result=Expect(fixtures[i].json,strlen(fixtures[i].json),fixtures[i].expected,fixtures[i].count);
        if(result!=0 || fixtures[i].expected!=UMI_STATUS_OK)return result;
        /* Independent expected spellings catch accidental integer/float coercion. */
        const char *number=NULL;
        if(strcmp(mode,"front")==0)number="-3";
        if(strcmp(mode,"fraction")==0)number="0.125";
        if(strcmp(mode,"exponent")==0)number="2.5e+2";
        if(strcmp(mode,"large-integer")==0)number="9223372036854775808";
        if(strcmp(mode,"large-exponent")==0)number="1e9999";
        if(number){UmiProviderConnection c=Connection();UmiComfyQueue *queue=NULL;UmiComfyQueueEntry item;
            CHECK(UmiComfyQueueCreate(&c,1U,fixtures[i].json,strlen(fixtures[i].json),NULL,&queue)==UMI_STATUS_OK);
            CHECK(UmiComfyQueueAt(queue,0U,&item)==UMI_STATUS_OK && strcmp(item.number_json,number)==0);UmiComfyQueueDestroy(queue);
        }
        return 0;
    }
    UmiProviderConnection c=Connection();UmiComfyQueue *queue=NULL;const char *sample=fixtures[0].json;
    if(strcmp(mode,"invalid-utf8")==0){const unsigned char invalid[]={'{','"',0xff,'"',':','0','}'};return Expect(invalid,sizeof invalid,UMI_STATUS_PARSE_ERROR,0U);}
    if(strcmp(mode,"byte-limit")==0 || strcmp(mode,"exact-byte-limit")==0){
        size_t length=UMI_COMFY_QUEUE_MAX_BYTES+(strcmp(mode,"byte-limit")==0?1U:0U);char *text=malloc(length);CHECK(text!=NULL);
        const char *empty="{\"queue_running\":[],\"queue_pending\":[]}";memset(text,' ',length);memcpy(text,empty,strlen(empty));
        int result=Expect(text,length,length>UMI_COMFY_QUEUE_MAX_BYTES?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK,0U);free(text);return result;
    }
    if(strcmp(mode,"entry-limit")==0 || strcmp(mode,"maximum-entries")==0){
        size_t count=UMI_COMFY_QUEUE_LIMIT+(strcmp(mode,"entry-limit")==0?1U:0U);char *text=Many(count);CHECK(text!=NULL);
        int result=Expect(text,strlen(text),count>UMI_COMFY_QUEUE_LIMIT?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK,count);free(text);return result;
    }
    if(strcmp(mode,"depth-limit")==0){char text[256];size_t length=0U;for(size_t i=0;i<40U;++i)text[length++]='[';text[length++]='0';for(size_t i=0;i<40U;++i)text[length++]=']';return Expect(text,length,UMI_STATUS_CAPACITY_EXCEEDED,0U);}
    if(strcmp(mode,"node-limit")==0){size_t capacity=150000U,length=1U;char *text=malloc(capacity);CHECK(text!=NULL);text[0]='[';
        for(size_t i=0;i<65536U;++i){if(i)text[length++]=',';text[length++]='0';}text[length++]=']';int result=Expect(text,length,UMI_STATUS_CAPACITY_EXCEEDED,0U);free(text);return result;
    }
    if(strcmp(mode,"connection-scope")==0){c.enabled=false;CHECK(UmiComfyQueueCreate(&c,1U,sample,strlen(sample),NULL,&queue)==UMI_STATUS_PERMISSION_DENIED);CHECK(queue==NULL);return 0;}
    if(strcmp(mode,"cancel")==0){UmiCancellationToken *token=NULL;CHECK(umi_cancellation_token_create(&token)==UMI_STATUS_OK);umi_cancellation_token_request(token);
        CHECK(UmiComfyQueueCreate(&c,1U,sample,strlen(sample),token,&queue)==UMI_STATUS_CANCELLED);CHECK(queue==NULL);umi_cancellation_token_destroy(token);return 0;
    }
    if(strcmp(mode,"nulls")==0){CHECK(UmiComfyQueueCreate(NULL,1U,sample,strlen(sample),NULL,&queue)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfyQueueCreate(&c,1U,NULL,1U,NULL,&queue)==UMI_STATUS_INVALID_ARGUMENT);CHECK(UmiComfyQueueCreate(&c,1U,sample,0U,NULL,&queue)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfyQueueCreate(&c,1U,sample,strlen(sample),NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);CHECK(queue==NULL && UmiComfyQueueCount(NULL)==0U);UmiComfyQueueDestroy(NULL);return 0;
    }
    if(strcmp(mode,"revision")==0){CHECK(UmiComfyQueueCreate(&c,0U,sample,strlen(sample),NULL,&queue)==UMI_STATUS_INVALID_ARGUMENT);CHECK(queue==NULL);}
    CHECK(UmiComfyQueueCreate(&c,1U,sample,strlen(sample),NULL,&queue)==UMI_STATUS_OK);
    if(strcmp(mode,"occupied")==0){UmiComfyQueue *before=queue;CHECK(UmiComfyQueueCreate(&c,1U,sample,strlen(sample),NULL,&queue)==UMI_STATUS_INVALID_ARGUMENT);CHECK(queue==before);}
    else if(strcmp(mode,"revision")==0){CHECK(UmiComfyQueueCheckConnection(queue,&c,2U)==UMI_STATUS_BUSY);strcpy(c.label,"Edited label");CHECK(UmiComfyQueueCheckConnection(queue,&c,1U)==UMI_STATUS_BUSY);}
    else if(strcmp(mode,"identity")==0){UmiComfyQueueReview review;char digest[UMI_SHA256_HEX_CAPACITY];CHECK(UmiComfyQueueInspect(queue,&review)==UMI_STATUS_OK);UmiSha256Hex(review.response_sha256,digest);
        CHECK(strcmp(digest,"d877bb24b53cbbc216ed4270d87fd190f5ed50ef0f1859d42429bf4d59de5266")==0);CHECK(review.connection_revision==1U && review.running_count==1U && review.pending_count==2U);
        UmiComfyQueueEntry item;CHECK(UmiComfyQueueAt(queue,1U,&item)==UMI_STATUS_OK && strcmp(item.prompt_id,"b-pending")==0 && strcmp(item.number_json,"20")==0);
        CHECK(UmiComfyQueueAt(queue,2U,&item)==UMI_STATUS_OK && strcmp(item.prompt_id,"a-pending")==0 && strcmp(item.number_json,"3")==0);
    }
    else if(strcmp(mode,"ownership")==0){UmiComfyQueueDestroy(queue);queue=NULL;size_t length=strlen(sample);char *copy=malloc(length);CHECK(copy!=NULL);memcpy(copy,sample,length);
        CHECK(UmiComfyQueueCreate(&c,1U,copy,length,NULL,&queue)==UMI_STATUS_OK);memset(copy,'X',length);free(copy);memset(&c,0,sizeof c);
        UmiComfyQueueEntry item;CHECK(UmiComfyQueueAt(queue,0U,&item)==UMI_STATUS_OK);UmiComfyQueueDestroy(queue);queue=NULL;
        CHECK(strcmp(item.prompt_id,"z-running")==0 && strcmp(item.number_json,"7")==0 && item.state==UMI_COMFY_QUEUE_RUNNING);
    }
    else if(strcmp(mode,"find-bounds")==0){size_t found=91U;char long_id[130];memset(long_id,'x',129U);long_id[129]='\0';const char invalid[]={ (char)0xff,0 };
        CHECK(UmiComfyQueueFind(queue,"missing",&found)==UMI_STATUS_NOT_FOUND && found==91U);
        CHECK(UmiComfyQueueFind(queue,"",&found)==UMI_STATUS_INVALID_ARGUMENT && found==91U);
        CHECK(UmiComfyQueueFind(queue,long_id,&found)==UMI_STATUS_CAPACITY_EXCEEDED && found==91U);
        CHECK(UmiComfyQueueFind(queue,invalid,&found)==UMI_STATUS_INVALID_ARGUMENT && found==91U);
        CHECK(UmiComfyQueueFind(queue,"Z-running",&found)==UMI_STATUS_NOT_FOUND && found==91U);
        CHECK(UmiComfyQueueFind(queue,NULL,&found)==UMI_STATUS_INVALID_ARGUMENT && found==91U);
    }
    else if(strcmp(mode,"output-preserved")==0){UmiComfyQueueEntry item,before;memset(&item,0x5a,sizeof item);memcpy(&before,&item,sizeof item);
        CHECK(UmiComfyQueueAt(queue,SIZE_MAX,&item)==UMI_STATUS_NOT_FOUND && memcmp(&item,&before,sizeof item)==0);
        CHECK(UmiComfyQueueAt(NULL,0U,&item)==UMI_STATUS_INVALID_ARGUMENT && memcmp(&item,&before,sizeof item)==0);
        UmiComfyQueueReview review,prior;memset(&review,0x5a,sizeof review);memcpy(&prior,&review,sizeof review);
        CHECK(UmiComfyQueueInspect(NULL,&review)==UMI_STATUS_INVALID_ARGUMENT && memcmp(&review,&prior,sizeof review)==0);
        CHECK(UmiComfyQueueInspect(queue,NULL)==UMI_STATUS_INVALID_ARGUMENT);CHECK(UmiComfyQueueAt(queue,0U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    }
    else{UmiComfyQueueDestroy(queue);fprintf(stderr,"Unknown test mode: %s\n",mode);return 2;}
    UmiComfyQueueDestroy(queue);return 0;
}
