/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_comfy_queue_json.c
 *
 * PURPOSE:
 *   Verify queue pagination, escaped identifiers, exact numbers and unchanged output on refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_queue_file.h"
#include "umicom/language_runtime/json_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int Text(const UmiJsonTree *tree,int object,const char *key,const char *expected)
{
    int value=-1;char text[256];CHECK(UmiJsonTreeMember(tree,object,key,&value)==UMI_STATUS_OK);
    CHECK(UmiJsonTreeKind(tree,value)==UMI_LANGUAGE_RUNTIME_JSON_STRING);
    CHECK(UmiJsonTreeText(tree,value,text,sizeof text)==UMI_STATUS_OK && !strcmp(text,expected));return 0;
}
static int Number(const UmiJsonTree *tree,int object,const char *key,int64_t expected)
{
    int value=-1;int64_t n=-1;CHECK(UmiJsonTreeMember(tree,object,key,&value)==UMI_STATUS_OK);
    CHECK(UmiJsonTreeInteger(tree,value,&n)==UMI_STATUS_OK && n==expected);return 0;
}
static int Refusal(const UmiComfyQueue *queue,size_t offset,size_t limit,size_t capacity,UmiStatus expected)
{
    char output[64];memset(output,'Q',sizeof output);size_t length=77U;
    CHECK(UmiComfyQueueJson(queue,offset,limit,output,capacity,&length)==expected);
    CHECK(length==77U);for(size_t i=0U;i<sizeof output;++i)CHECK(output[i]=='Q');return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    const char *sample="{\"queue_running\":[[-0,\"run\\\"\\n東京\",{\"text\":\"PRIVATE_SENTINEL\"},{},[\"PRIVATE_OUTPUT\"]]],\"queue_pending\":[[9007199254740993,\"later\",{},{\"hidden\":\"PRIVATE_EXTRA\"},[]],[2.5E+2,\"earlier\",{},{},[]]]}";char *many=NULL;
    if(!strcmp(mode,"empty"))sample="{\"queue_running\":[],\"queue_pending\":[]}";
    if(!strcmp(mode,"maximum-page")){
        many=malloc(65536U);CHECK(many!=NULL);
        size_t used=(size_t)snprintf(many,65536U,"{\"queue_running\":[],\"queue_pending\":[");
        for(size_t i=0U;i<300U;++i){
            int n=snprintf(many+used,65536U-used,"%s[%zu,\"job-%zu\",{},{},[]]",i?",":"",i,i);
            CHECK(n>0 && (size_t)n<65536U-used);used+=(size_t)n;
        }
        strcpy(many+used,"]}");sample=many;
    }
    UmiProviderConnection connection={0};strcpy(connection.id,"queue-json-fixture");
    strcpy(connection.provider_id,"comfyui");strcpy(connection.label,"Queue fixture");
    strcpy(connection.endpoint,"http://127.0.0.1:8188");connection.enabled=true;
    connection.route=UMI_PROVIDER_CONNECTION_LOOPBACK;connection.timeout_ms=30000;
    UmiComfyQueue *queue=NULL;
    CHECK(UmiComfyQueueCreate(&connection,UINT64_MAX,sample,strlen(sample),NULL,&queue)==UMI_STATUS_OK);
    free(many); /* The queue owns copies; generated source no longer exists. */
    char *json=malloc(UMI_COMFY_QUEUE_JSON_CAPACITY);CHECK(json!=NULL);size_t length=0U;
    size_t offset=!strcmp(mode,"second-page")?1U:!strcmp(mode,"last-page")?2U:!strcmp(mode,"end-page")?3U:0U;
    size_t limit=!strcmp(mode,"first-page")||!strcmp(mode,"second-page")?1U:256U;
    CHECK(UmiComfyQueueJson(queue,offset,limit,json,UMI_COMFY_QUEUE_JSON_CAPACITY,&length)==UMI_STATUS_OK);
    CHECK(length==strlen(json));UmiJsonTree *tree=NULL;
    CHECK(UmiJsonTreeCreate(json,length,NULL,NULL,&tree)==UMI_STATUS_OK);
    int entries=-1;CHECK(UmiJsonTreeMember(tree,0,"entries",&entries)==UMI_STATUS_OK);
    int first=UmiJsonTreeFirst(tree,entries);
    CHECK(!Text(tree,0,"connection_revision","18446744073709551615"));
    CHECK(!Text(tree,0,"scope","queue-observations"));
    if(!strcmp(mode,"summary")){
        CHECK(!Number(tree,0,"running_count",1) && !Number(tree,0,"pending_count",2));
        CHECK(!Number(tree,0,"entry_count",3) && !Number(tree,0,"page_count",3));
    }else if(!strcmp(mode,"first-page") || !strcmp(mode,"second-page") || !strcmp(mode,"last-page")){
        CHECK(UmiJsonTreeCount(tree,entries)==1U && !Number(tree,0,"offset",(int64_t)offset));
        CHECK(!Number(tree,0,"remaining_count",(int64_t)(2U-offset)));
        CHECK(!Number(tree,0,"next_offset",(int64_t)(offset+1U)));
        CHECK(!Text(tree,first,"state",offset==0U?"running":"pending"));
        CHECK(!Text(tree,first,"prompt_id",offset==0U?"run\"\n東京":offset==1U?"later":"earlier"));
    }else if(!strcmp(mode,"empty") || !strcmp(mode,"end-page")){
        CHECK(UmiJsonTreeCount(tree,entries)==0U && !Number(tree,0,"remaining_count",0));
    }else if(!strcmp(mode,"beyond"))CHECK(!Refusal(queue,4U,1U,64U,UMI_STATUS_NOT_FOUND));
    else if(!strcmp(mode,"zero-limit"))CHECK(!Refusal(queue,0U,0U,64U,UMI_STATUS_INVALID_ARGUMENT));
    else if(!strcmp(mode,"large-limit"))CHECK(!Refusal(queue,0U,257U,64U,UMI_STATUS_INVALID_ARGUMENT));
    else if(!strcmp(mode,"null-owner"))CHECK(!Refusal(NULL,0U,1U,64U,UMI_STATUS_INVALID_ARGUMENT));
    else if(!strcmp(mode,"null-out")){
        size_t n=77U;CHECK(UmiComfyQueueJson(queue,0U,1U,NULL,64U,&n)==UMI_STATUS_INVALID_ARGUMENT && n==77U);
    }else if(!strcmp(mode,"null-length")){
        char out[64];memset(out,'Q',sizeof out);
        CHECK(UmiComfyQueueJson(queue,0U,1U,out,sizeof out,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        for(size_t i=0U;i<sizeof out;++i)CHECK(out[i]=='Q');
    }else if(!strcmp(mode,"exact-capacity") || !strcmp(mode,"small-capacity")){
        char *out=malloc(length+1U);CHECK(out!=NULL);memset(out,'Q',length+1U);size_t n=77U;
        if(!strcmp(mode,"exact-capacity")){
            CHECK(UmiComfyQueueJson(queue,0U,256U,out,length+1U,&n)==UMI_STATUS_OK);
            CHECK(n==length && !memcmp(out,json,length+1U));
        }else{
            CHECK(UmiComfyQueueJson(queue,0U,256U,out,length,&n)==UMI_STATUS_CAPACITY_EXCEEDED && n==77U);
            for(size_t i=0U;i<=length;++i)CHECK(out[i]=='Q');
        }free(out);
    }else if(!strcmp(mode,"zero-capacity"))CHECK(!Refusal(queue,0U,1U,0U,UMI_STATUS_CAPACITY_EXCEEDED));
    else if(!strcmp(mode,"escaping"))CHECK(!Text(tree,first,"prompt_id","run\"\n東京"));
    else if(!strcmp(mode,"number-string")){
        CHECK(!Text(tree,first,"number_json","-0"));first=UmiJsonTreeNext(tree,first);
        CHECK(!Text(tree,first,"number_json","9007199254740993"));first=UmiJsonTreeNext(tree,first);
        CHECK(!Text(tree,first,"number_json","2.5E+2"));
    }else if(!strcmp(mode,"digests")){
        UmiComfyQueueReview review;char hex[65];CHECK(UmiComfyQueueInspect(queue,&review)==UMI_STATUS_OK);
        UmiSha256Hex(review.connection_sha256,hex);CHECK(!Text(tree,0,"connection_sha256",hex));
        CHECK(!Text(tree,0,"response_sha256","5c0d999c4c077faceae99f390724a6bfba35e969ed2b50561913c8a9d4829012"));
    }else if(!strcmp(mode,"private-values")){
        CHECK(strstr(json,"PRIVATE_")==NULL);int absent=-1;
        CHECK(UmiJsonTreeMember(tree,first,"prompt",&absent)==UMI_STATUS_NOT_FOUND);
        CHECK(UmiJsonTreeMember(tree,first,"extra_data",&absent)==UMI_STATUS_NOT_FOUND);
    }else if(!strcmp(mode,"maximum-page")){
        CHECK(UmiJsonTreeCount(tree,entries)==256U);
        CHECK(!Number(tree,0,"entry_count",300) && !Number(tree,0,"remaining_count",44));
        CHECK(!Number(tree,0,"next_offset",256));
    }else CHECK(!strcmp(mode,"max-revision"));
    UmiJsonTreeDestroy(tree);UmiComfyQueueDestroy(queue);free(json);return 0;
}
