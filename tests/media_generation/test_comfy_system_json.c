/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_comfy_system_json.c
 *
 * PURPOSE:
 *   Check unknown and exact counter semantics, escaping, bounded device output and atomic refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/



#include "umicom/media_generation/comfy_system_file.h"
#include "umicom/language_runtime/json_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int Text(const UmiJsonTree *tree,int object,const char *key,const char *expected)
{
    int value=-1;char text[4096];CHECK(UmiJsonTreeMember(tree,object,key,&value)==UMI_STATUS_OK);
    CHECK(UmiJsonTreeKind(tree,value)==UMI_LANGUAGE_RUNTIME_JSON_STRING);
    CHECK(UmiJsonTreeText(tree,value,text,sizeof text)==UMI_STATUS_OK && !strcmp(text,expected));return 0;
}
static int Number(const UmiJsonTree *tree,int object,const char *key,int64_t expected)
{
    int value=-1;int64_t n=-1;CHECK(UmiJsonTreeMember(tree,object,key,&value)==UMI_STATUS_OK);
    CHECK(UmiJsonTreeInteger(tree,value,&n)==UMI_STATUS_OK && n==expected);return 0;
}
static int Boolean(const UmiJsonTree *tree,int object,const char *key,int expected)
{
    int value=-1,n=-1;CHECK(UmiJsonTreeMember(tree,object,key,&value)==UMI_STATUS_OK);
    CHECK(UmiJsonTreeBoolean(tree,value,&n)==UMI_STATUS_OK && n==expected);return 0;
}
static int Null(const UmiJsonTree *tree,int object,const char *key)
{
    int value=-1;const char *bytes=NULL;size_t length=0U;
    CHECK(UmiJsonTreeMember(tree,object,key,&value)==UMI_STATUS_OK);
    CHECK(UmiJsonTreeKind(tree,value)==UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE);
    CHECK(UmiJsonTreeSourceSpan(tree,value,&bytes,&length)==UMI_STATUS_OK);
    CHECK(length==4U && !memcmp(bytes,"null",4U));return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    const char *sample="{\"system\":{\"os\":\"test\\\"\\n東京\",\"python_version\":\"synthetic\","
        "\"pytorch_version\":\"test\",\"comfyui_version\":\"test\",\"embedded_python\":false,"
        "\"ram_total\":9223372036854775807,\"ram_free\":9007199254740993,"
        "\"argv\":[\"PRIVATE_ARG\"]},\"devices\":[{\"name\":\"GPU\\\"\\n東京\",\"type\":\"future\","
        "\"index\":9007199254740993,\"vram_total\":9223372036854775807,\"vram_free\":0,"
        "\"torch_vram_total\":9007199254740993,\"torch_vram_free\":1,\"extra\":\"PRIVATE_DEVICE\"}],"
        "\"private\":\"PRIVATE_EXTENSION\"}";
    if(!strcmp(mode,"unknown"))sample="{\"system\":{},\"devices\":[{\"name\":\"CPU\",\"type\":\"cpu\"}]}";
    if(!strcmp(mode,"null-fields"))sample="{\"system\":{\"ram_free\":null,\"os\":null,\"embedded_python\":null},"
        "\"devices\":[{\"name\":\"CPU\",\"type\":\"cpu\",\"index\":null,\"vram_free\":null}]}";
    if(!strcmp(mode,"known-zero"))sample="{\"system\":{\"os\":\"\",\"embedded_python\":false,\"ram_total\":0,\"ram_free\":0},"
        "\"devices\":[{\"name\":\"GPU\",\"type\":\"cuda\",\"index\":0,\"vram_total\":0,\"vram_free\":0}]}";
    if(!strcmp(mode,"inconsistent"))sample="{\"system\":{\"ram_total\":1,\"ram_free\":2},\"devices\":["
        "{\"name\":\"GPU\",\"type\":\"cuda\",\"vram_total\":2,\"vram_free\":3,\"torch_vram_total\":4,\"torch_vram_free\":5}]}";
    if(!strcmp(mode,"free-without-total"))sample="{\"system\":{\"ram_free\":2},\"devices\":["
        "{\"name\":\"GPU\",\"type\":\"cuda\",\"vram_free\":3,\"torch_vram_free\":5}]}";
    char *many=NULL;
    if(!strcmp(mode,"maximum-escaped-devices")){
        many=malloc(300000U);CHECK(many!=NULL);size_t used=0U;
        const char *prefix="{\"system\":{},\"devices\":[";used=strlen(prefix);memcpy(many,prefix,used);
        for(size_t row=0U;row<UMI_COMFY_SYSTEM_DEVICE_LIMIT;++row){
            if(row)many[used++]=',';
            const char *name="{\"name\":\"";memcpy(many+used,name,strlen(name));used+=strlen(name);
            for(size_t j=0U;j<512U;++j){memcpy(many+used,"\\u0001",6U);used+=6U;}
            const char *type="\",\"type\":\"";memcpy(many+used,type,strlen(type));used+=strlen(type);
            for(size_t j=0U;j<64U;++j){memcpy(many+used,"\\u0002",6U);used+=6U;}
            memcpy(many+used,"\"}",2U);used+=2U;
        }
        memcpy(many+used,"]}",3U);sample=many;
    }
    UmiProviderConnection connection={0};strcpy(connection.id,"system-json-fixture");
    strcpy(connection.provider_id,"comfyui");strcpy(connection.label,"System fixture");
    strcpy(connection.endpoint,"http://127.0.0.1:8188");connection.enabled=true;
    connection.route=UMI_PROVIDER_CONNECTION_LOOPBACK;connection.timeout_ms=30000;
    unsigned char digest[UMI_SHA256_BYTES];UmiSha256 hash;UmiSha256Init(&hash);
    CHECK(UmiSha256Update(&hash,sample,strlen(sample))==UMI_STATUS_OK);
    CHECK(UmiSha256Final(&hash,digest)==UMI_STATUS_OK);
    UmiComfySystemStats *stats=NULL;
    CHECK(UmiComfySystemStatsCreate(&connection,UINT64_MAX,sample,strlen(sample),NULL,&stats)==UMI_STATUS_OK);
    free(many);
    char *json=malloc(UMI_COMFY_SYSTEM_JSON_CAPACITY);CHECK(json!=NULL);size_t length=0U;
    CHECK(UmiComfySystemStatsJson(stats,json,UMI_COMFY_SYSTEM_JSON_CAPACITY,&length)==UMI_STATUS_OK);
    CHECK(length==strlen(json));UmiJsonTree *tree=NULL;
    CHECK(UmiJsonTreeCreate(json,length,NULL,NULL,&tree)==UMI_STATUS_OK);
    CHECK(!Text(tree,0,"scope","system-observations"));
    CHECK(!Text(tree,0,"connection_revision","18446744073709551615"));
    int devices=-1;CHECK(UmiJsonTreeMember(tree,0,"devices",&devices)==UMI_STATUS_OK);
    int first=UmiJsonTreeFirst(tree,devices);CHECK(first>=0);
    if(!strcmp(mode,"summary")){
        CHECK(!Number(tree,0,"device_count",1) && !Number(tree,0,"known",127));
        CHECK(!Number(tree,first,"known",31) && !Number(tree,first,"response_index",0));
        CHECK(!Boolean(tree,0,"embedded_python",0) && !Boolean(tree,0,"ram_inconsistent",0));
    }else if(!strcmp(mode,"unknown") || !strcmp(mode,"null-fields")){
        CHECK(!Number(tree,0,"known",0) && !Number(tree,first,"known",0));
        CHECK(!Null(tree,0,"os") && !Null(tree,0,"python_version") && !Null(tree,0,"pytorch_version"));
        CHECK(!Null(tree,0,"comfyui_version") && !Null(tree,0,"embedded_python"));
        CHECK(!Null(tree,0,"ram_total") && !Null(tree,0,"ram_free") && !Null(tree,first,"index"));
        CHECK(!Null(tree,first,"vram_total") && !Null(tree,first,"vram_free"));
        CHECK(!Null(tree,first,"torch_vram_total") && !Null(tree,first,"torch_vram_free"));
    }else if(!strcmp(mode,"known-zero")){
        CHECK(!Text(tree,0,"os","") && !Boolean(tree,0,"embedded_python",0));
        CHECK(!Text(tree,0,"ram_total","0") && !Text(tree,0,"ram_free","0"));
        CHECK(!Text(tree,first,"index","0") && !Text(tree,first,"vram_free","0"));
    }else if(!strcmp(mode,"inconsistent") || !strcmp(mode,"free-without-total")){
        int inconsistent=!strcmp(mode,"inconsistent");
        CHECK(!Boolean(tree,0,"ram_inconsistent",inconsistent));
        CHECK(!Boolean(tree,first,"vram_inconsistent",inconsistent));
        CHECK(!Boolean(tree,first,"torch_vram_inconsistent",inconsistent));
        CHECK(!Text(tree,0,"ram_free","2") && !Text(tree,first,"vram_free","3"));
        CHECK(!Text(tree,first,"torch_vram_free","5"));
        if(!inconsistent)CHECK(!Null(tree,0,"ram_total") && !Null(tree,first,"vram_total"));
    }else if(!strcmp(mode,"exact-counters")){
        CHECK(!Text(tree,0,"ram_total","9223372036854775807") && !Text(tree,0,"ram_free","9007199254740993"));
        CHECK(!Text(tree,first,"index","9007199254740993") && !Text(tree,first,"vram_total","9223372036854775807"));
        CHECK(!Text(tree,first,"torch_vram_total","9007199254740993") && !Text(tree,first,"torch_vram_free","1"));
    }else if(!strcmp(mode,"escaping")){
        CHECK(!Text(tree,0,"os","test\"\n東京") && !Text(tree,first,"name","GPU\"\n東京"));
    }else if(!strcmp(mode,"private-values")){
        CHECK(strstr(json,"PRIVATE_")==NULL && strstr(json,"127.0.0.1")==NULL);
        int absent=-1;CHECK(UmiJsonTreeMember(tree,0,"argv",&absent)==UMI_STATUS_NOT_FOUND);
    }else if(!strcmp(mode,"digests")){
        char hex[UMI_SHA256_HEX_CAPACITY];UmiSha256Hex(digest,hex);
        CHECK(!Text(tree,0,"response_sha256",hex));
        UmiComfySystemSummary summary;CHECK(UmiComfySystemStatsInspect(stats,&summary)==UMI_STATUS_OK);
        UmiSha256Hex(summary.connection_sha256,hex);CHECK(!Text(tree,0,"connection_sha256",hex));
    }else if(!strcmp(mode,"maximum-escaped-devices")){
        CHECK(UmiJsonTreeCount(tree,devices)==UMI_COMFY_SYSTEM_DEVICE_LIMIT);
        CHECK(length>200000U && length<UMI_COMFY_SYSTEM_JSON_CAPACITY);
        char name[513];memset(name,1,512U);name[512]='\0';
        char type[65];memset(type,2,64U);type[64]='\0';
        for(int row=first;row>=0;row=UmiJsonTreeNext(tree,row)){
            CHECK(!Text(tree,row,"name",name) && !Text(tree,row,"type",type));
        }
    }else{
        char *output=malloc(length+1U);CHECK(output!=NULL);memset(output,'Q',length+1U);size_t n=77U;
        size_t capacity=length+1U;UmiStatus expected=UMI_STATUS_OK;
        const UmiComfySystemStats *selected=stats;char *destination=output;size_t *count=&n;
        if(!strcmp(mode,"short-capacity")){capacity=length;expected=UMI_STATUS_CAPACITY_EXCEEDED;}
        else if(!strcmp(mode,"zero-capacity")){capacity=0U;expected=UMI_STATUS_CAPACITY_EXCEEDED;}
        else if(!strcmp(mode,"null-owner")){selected=NULL;expected=UMI_STATUS_INVALID_ARGUMENT;}
        else if(!strcmp(mode,"null-output")){destination=NULL;expected=UMI_STATUS_INVALID_ARGUMENT;}
        else if(!strcmp(mode,"null-length")){count=NULL;expected=UMI_STATUS_INVALID_ARGUMENT;}
        else CHECK(!strcmp(mode,"exact-capacity"));
        CHECK(UmiComfySystemStatsJson(selected,destination,capacity,count)==expected);
        if(expected==UMI_STATUS_OK)CHECK(n==length && !memcmp(output,json,length+1U));
        else {CHECK(n==77U);for(size_t i=0U;i<=length;++i)CHECK(output[i]=='Q');}
        free(output);
    }
    UmiJsonTreeDestroy(tree);UmiComfySystemStatsDestroy(stats);free(json);return 0;
}
