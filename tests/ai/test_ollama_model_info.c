/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai/test_ollama_model_info.c
 *
 * PURPOSE:
 *   Verify unknown metadata, capability declarations, identity bounds and owned model-detail refusal semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/ollama_model_info.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;}}while(0)
typedef struct Fixture {const char *mode,*json;UmiStatus status;} Fixture;
static const Fixture fixtures[]={
    {"complete","{\"modified_at\":\"2026-01-01T00:00:00Z\",\"details\":{\"parent_model\":\"base:tag\",\"format\":\"gguf\",\"family\":\"example\",\"parameter_size\":\"8B\",\"quantization_level\":\"Q4_0\"},\"model_info\":{\"general.architecture\":\"example\",\"general.parameter_count\":9007199254740993,\"example.context_length\":8192,\"example.embedding_length\":64},\"capabilities\":[\"completion\",\"vision\",\"future-能力\"],\"template\":\"PRIVATE_TEMPLATE\",\"system\":\"PRIVATE_SYSTEM\",\"license\":\"PRIVATE_LICENSE\"}",UMI_STATUS_OK},
    {"absent","{}",UMI_STATUS_OK},
    {"empty-capabilities","{\"capabilities\":[]}",UMI_STATUS_OK},
    {"known-zero","{\"model_info\":{\"general.architecture\":\"a\",\"general.parameter_count\":0,\"a.context_length\":0,\"a.embedding_length\":0}}",UMI_STATUS_OK},
    {"exact-integers","{\"model_info\":{\"general.architecture\":\"a\",\"general.parameter_count\":9223372036854775807,\"a.context_length\":9223372036854775807,\"a.embedding_length\":9223372036854775807}}",UMI_STATUS_OK},
    {"missing-architecture","{\"details\":{\"family\":\"llama\"},\"model_info\":{\"llama.context_length\":8192}}",UMI_STATUS_OK},
    {"foreign-architecture","{\"model_info\":{\"general.architecture\":\"a\",\"b.context_length\":12}}",UMI_STATUS_OK},
    {"unknown-capability","{\"capabilities\":[\"future-能力\"]}",UMI_STATUS_OK},
    {"case-sensitive","{\"capabilities\":[\"tools\",\"Tools\"]}",UMI_STATUS_OK},
    {"wrong-root","[]",UMI_STATUS_PARSE_ERROR},
    {"details-type","{\"details\":[]}",UMI_STATUS_PARSE_ERROR},
    {"model-info-type","{\"model_info\":null}",UMI_STATUS_PARSE_ERROR},
    {"capabilities-type","{\"capabilities\":null}",UMI_STATUS_PARSE_ERROR},
    {"capability-type","{\"capabilities\":[1]}",UMI_STATUS_PARSE_ERROR},
    {"empty-capability","{\"capabilities\":[\"\"]}",UMI_STATUS_PARSE_ERROR},
    {"duplicate-capability","{\"capabilities\":[\"tools\",\"tools\"]}",UMI_STATUS_ALREADY_EXISTS},
    {"escaped-duplicate","{\"capabilities\":[\"tools\",\"\\u0074ools\"]}",UMI_STATUS_ALREADY_EXISTS},
    {"duplicate-key","{\"details\":{},\"\\u0064etails\":{}}",UMI_STATUS_ALREADY_EXISTS},
    {"nested-duplicate","{\"unknown\":{\"a\":1,\"a\":2}}",UMI_STATUS_ALREADY_EXISTS},
    {"numeric-negative","{\"model_info\":{\"general.parameter_count\":-1}}",UMI_STATUS_PARSE_ERROR},
    {"numeric-fraction","{\"model_info\":{\"general.parameter_count\":1.5}}",UMI_STATUS_PARSE_ERROR},
    {"numeric-overflow","{\"model_info\":{\"general.parameter_count\":9223372036854775808}}",UMI_STATUS_CAPACITY_EXCEEDED},
    {"architecture-null","{\"model_info\":{\"general.architecture\":null}}",UMI_STATUS_PARSE_ERROR},
    {"detail-null","{\"details\":{\"format\":null}}",UMI_STATUS_PARSE_ERROR},
    {"provider-error","{\"error\":\"PRIVATE_PROVIDER_ERROR\"}",UMI_STATUS_INVALID_STATE},
    {"empty-error","{\"error\":\"\"}",UMI_STATUS_PARSE_ERROR},
    {"wrong-error","{\"error\":1}",UMI_STATUS_PARSE_ERROR},
    {"trailing","{}{}",UMI_STATUS_PARSE_ERROR}
};
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1],*model="example:model",*json="{\"modified_at\":\"2026-01-01T00:00:00Z\",\"details\":{\"parent_model\":\"base:tag\",\"format\":\"gguf\",\"family\":\"example\",\"parameter_size\":\"8B\",\"quantization_level\":\"Q4_0\"},\"model_info\":{\"general.architecture\":\"example\",\"general.parameter_count\":9007199254740993,\"example.context_length\":8192,\"example.embedding_length\":64},\"capabilities\":[\"completion\",\"vision\",\"future-能力\"],\"template\":\"PRIVATE_TEMPLATE\",\"system\":\"PRIVATE_SYSTEM\",\"license\":\"PRIVATE_LICENSE\"}";
    UmiStatus expected=UMI_STATUS_OK;int recognized=0;
    for(size_t i=0U;i<sizeof fixtures/sizeof fixtures[0];++i){
        if(!strcmp(mode,fixtures[i].mode)){json=fixtures[i].json;expected=fixtures[i].status;recognized=1;break;}
    }
    char *large=NULL;char model_storage[514];size_t length=strlen(json);
    UmiCancellationToken *cancel=NULL;UmiOllamaModelInfo *info=NULL;
    if(!strcmp(mode,"embedded-nul")){
        static const unsigned char bytes[]={'{','}',0,' '};json=(const char *)bytes;length=sizeof bytes;expected=UMI_STATUS_PARSE_ERROR;
    }else if(!strcmp(mode,"invalid-utf8")){
        static const unsigned char bytes[]={'{','"',0xff,'"',':','0','}'};json=(const char *)bytes;length=sizeof bytes;expected=UMI_STATUS_PARSE_ERROR;
    }else if(!strcmp(mode,"capability-max") || !strcmp(mode,"capability-overflow")){
        char name[130];size_t count=!strcmp(mode,"capability-max")?128U:129U;memset(name,'x',count);name[count]='\0';
        large=malloc(256U);CHECK(large!=NULL);int n=snprintf(large,256U,"{\"capabilities\":[\"%s\"]}",name);
        CHECK(n>0 && n<256);json=large;length=(size_t)n;
        if(count>128U)expected=UMI_STATUS_CAPACITY_EXCEEDED;
    }else if(!strcmp(mode,"capability-count-max") || !strcmp(mode,"capability-count-overflow")){
        size_t count=!strcmp(mode,"capability-count-max")?64U:65U;
        large=malloc(4096U);CHECK(large!=NULL);size_t used=(size_t)snprintf(large,4096U,"{\"capabilities\":[");
        for(size_t i=0U;i<count;++i){
            int n=snprintf(large+used,4096U-used,"%s\"cap-%zu\"",i?",":"",i);
            CHECK(n>0 && (size_t)n<4096U-used);used+=(size_t)n;
        }strcpy(large+used,"]}");json=large;length=strlen(json);
        if(count>64U)expected=UMI_STATUS_CAPACITY_EXCEEDED;
    }else if(!strcmp(mode,"model-max") || !strcmp(mode,"model-overflow")){
        size_t count=!strcmp(mode,"model-max")?512U:513U;
        memset(model_storage,'m',count);model_storage[count]='\0';model=model_storage;
        if(count>512U)expected=UMI_STATUS_CAPACITY_EXCEEDED;
    }else if(!strcmp(mode,"model-utf8")){
        model_storage[0]=(char)0xff;model_storage[1]='\0';model=model_storage;expected=UMI_STATUS_INVALID_ARGUMENT;
    }else if(!strcmp(mode,"model-empty")){model="";expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"byte-limit")){length=UMI_OLLAMA_MODEL_INFO_MAX_BYTES+1U;expected=UMI_STATUS_CAPACITY_EXCEEDED;}
    else if(!strcmp(mode,"exact-byte-limit")){
        large=malloc(UMI_OLLAMA_MODEL_INFO_MAX_BYTES);CHECK(large!=NULL);
        memcpy(large,json,length);memset(large+length,' ',UMI_OLLAMA_MODEL_INFO_MAX_BYTES-length);
        json=large;length=UMI_OLLAMA_MODEL_INFO_MAX_BYTES;
    }else if(!strcmp(mode,"depth")){
        large=malloc(256U);CHECK(large!=NULL);size_t used=0U;
        memcpy(large,"{\"unknown\":",11U);used=11U;
        for(size_t i=0U;i<40U;++i){large[used++]='[';}
        large[used++]='0';
        for(size_t i=0U;i<40U;++i){large[used++]=']';}
        large[used++]='}';
        json=large;length=used;expected=UMI_STATUS_CAPACITY_EXCEEDED;
    }else if(!strcmp(mode,"node-limit")){
        large=malloc(140000U);CHECK(large!=NULL);size_t used=12U;memcpy(large,"{\"unknown\":[",used);
        for(size_t i=0U;i<65536U;++i){if(i)large[used++]=',';large[used++]='0';}
        large[used++]=']';large[used++]='}';json=large;length=used;expected=UMI_STATUS_CAPACITY_EXCEEDED;
    }else if(!strcmp(mode,"ownership")){
        large=malloc(length+1U);CHECK(large!=NULL);memcpy(large,json,length+1U);json=large;
    }else if(!strcmp(mode,"occupied")){
        CHECK(UmiOllamaModelInfoCreate(model,json,length,NULL,&info)==UMI_STATUS_OK);
        expected=UMI_STATUS_INVALID_ARGUMENT;
    }else if(!strcmp(mode,"cancel")){
        CHECK(umi_cancellation_token_create(&cancel)==UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);expected=UMI_STATUS_CANCELLED;
    }else if(!recognized)CHECK(!strcmp(mode,"read-refusal") || !strcmp(mode,"capability-bounds") || !strcmp(mode,"query-refusal") || !strcmp(mode,"nulls"));
    UmiOllamaModelInfo *before=info;
    CHECK(UmiOllamaModelInfoCreate(model,json,length,cancel,&info)==expected);
    if(large!=NULL){memset(large,0,(!strcmp(mode,"byte-limit")?0U:length));free(large);}
    umi_cancellation_token_destroy(cancel);
    if(expected!=UMI_STATUS_OK){CHECK(info==before);UmiOllamaModelInfoDestroy(info);return 0;}
    UmiOllamaModelInfoSummary summary;CHECK(UmiOllamaModelInfoInspect(info,&summary)==UMI_STATUS_OK);
    CHECK(!strcmp(summary.requested_model,model) && summary.response_bytes==length);
    if(!strcmp(mode,"complete") || !strcmp(mode,"ownership")){
        CHECK(summary.known==7U && summary.parameter_count==UINT64_C(9007199254740993));
        CHECK(summary.context_length==8192U && summary.embedding_length==64U);
        CHECK(!strcmp(summary.parent_model,"base:tag") && !strcmp(summary.format,"gguf"));
        CHECK(!strcmp(summary.family,"example") && !strcmp(summary.parameter_size,"8B") && !strcmp(summary.quantization,"Q4_0"));
        CHECK(summary.capabilities_observed && summary.capability_count==3U);
        char name[129],hex[65];CHECK(UmiOllamaModelInfoCapabilityAt(info,2U,name,sizeof name)==UMI_STATUS_OK && !strcmp(name,"future-能力"));
        UmiSha256Hex(summary.response_sha256,hex);CHECK(!strcmp(hex,"5b993a9c136d29d879ac727a41d2b27a68d62220e9415daa32ea9eab3c8293b5"));
        bool declared=false;CHECK(UmiOllamaModelInfoDeclares(info,"vision",&declared)==UMI_STATUS_OK && declared);
        CHECK(UmiOllamaModelInfoDeclares(info,"image-generation",&declared)==UMI_STATUS_OK && !declared);
    }else if(!strcmp(mode,"absent") || !strcmp(mode,"missing-architecture") || !strcmp(mode,"foreign-architecture")){
        CHECK(summary.known==0U && !summary.capabilities_observed);
        bool declared=true;CHECK(UmiOllamaModelInfoDeclares(info,"completion",&declared)==UMI_STATUS_NOT_FOUND && declared);
    }else if(!strcmp(mode,"empty-capabilities")){
        CHECK(summary.capabilities_observed && summary.capability_count==0U);
        bool declared=true;CHECK(UmiOllamaModelInfoDeclares(info,"completion",&declared)==UMI_STATUS_OK && !declared);
    }else if(!strcmp(mode,"known-zero"))CHECK(summary.known==7U && summary.parameter_count==0U && summary.context_length==0U && summary.embedding_length==0U);
    else if(!strcmp(mode,"exact-integers"))CHECK(summary.known==7U && summary.parameter_count==INT64_MAX && summary.context_length==INT64_MAX && summary.embedding_length==INT64_MAX);
    else if(!strcmp(mode,"unknown-capability")){bool declared=false;CHECK(UmiOllamaModelInfoDeclares(info,"future-能力",&declared)==UMI_STATUS_OK && declared);}
    else if(!strcmp(mode,"case-sensitive")){bool declared=true;CHECK(summary.capability_count==2U && UmiOllamaModelInfoDeclares(info,"TOOLS",&declared)==UMI_STATUS_OK && !declared);}
    else if(!strcmp(mode,"capability-count-max"))CHECK(summary.capability_count==64U);
    else if(!strcmp(mode,"capability-max")){
        char name[129];CHECK(UmiOllamaModelInfoCapabilityAt(info,0U,name,sizeof name)==UMI_STATUS_OK && strlen(name)==128U);
    }else if(!strcmp(mode,"read-refusal")){
        UmiOllamaModelInfoSummary out;unsigned char saved[sizeof out];memset(&out,0xa5,sizeof out);memcpy(saved,&out,sizeof out);
        CHECK(UmiOllamaModelInfoInspect(NULL,&out)==UMI_STATUS_INVALID_ARGUMENT && !memcmp(saved,&out,sizeof out));
        CHECK(UmiOllamaModelInfoInspect(info,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    }else if(!strcmp(mode,"capability-bounds")){
        char out[129];memset(out,'Q',sizeof out);
        CHECK(UmiOllamaModelInfoCapabilityAt(info,3U,out,sizeof out)==UMI_STATUS_NOT_FOUND);
        CHECK(UmiOllamaModelInfoCapabilityAt(info,0U,out,10U)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiOllamaModelInfoCapabilityAt(info,0U,out,0U)==UMI_STATUS_CAPACITY_EXCEEDED);
        for(size_t i=0U;i<sizeof out;++i)CHECK(out[i]=='Q');
        CHECK(UmiOllamaModelInfoCapabilityAt(info,0U,out,11U)==UMI_STATUS_OK && !strcmp(out,"completion"));
    }else if(!strcmp(mode,"query-refusal")){
        char name[130];memset(name,'x',129U);name[129]='\0';bool declared=true;
        CHECK(UmiOllamaModelInfoDeclares(info,name,&declared)==UMI_STATUS_CAPACITY_EXCEEDED && declared);
        CHECK(UmiOllamaModelInfoDeclares(info,"",&declared)==UMI_STATUS_INVALID_ARGUMENT && declared);
        name[0]=(char)0xff;name[1]='\0';
        CHECK(UmiOllamaModelInfoDeclares(info,name,&declared)==UMI_STATUS_INVALID_ARGUMENT && declared);
        CHECK(UmiOllamaModelInfoDeclares(info,NULL,&declared)==UMI_STATUS_INVALID_ARGUMENT && declared);
        CHECK(UmiOllamaModelInfoDeclares(info,"vision",NULL)==UMI_STATUS_INVALID_ARGUMENT);
    }else if(!strcmp(mode,"nulls")){
        UmiOllamaModelInfo *out=NULL;bool declared=true;
        CHECK(UmiOllamaModelInfoCreate(NULL,"{}",2U,NULL,&out)==UMI_STATUS_INVALID_ARGUMENT && out==NULL);
        CHECK(UmiOllamaModelInfoCreate(model,NULL,2U,NULL,&out)==UMI_STATUS_INVALID_ARGUMENT && out==NULL);
        CHECK(UmiOllamaModelInfoCreate(model,"{}",0U,NULL,&out)==UMI_STATUS_INVALID_ARGUMENT && out==NULL);
        CHECK(UmiOllamaModelInfoCreate(model,"{}",2U,NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiOllamaModelInfoDeclares(NULL,"vision",&declared)==UMI_STATUS_INVALID_ARGUMENT && declared);
        CHECK(UmiOllamaModelInfoCapabilityAt(info,0U,NULL,10U)==UMI_STATUS_INVALID_ARGUMENT);
        UmiOllamaModelInfoDestroy(NULL);
    }else CHECK(!strcmp(mode,"model-max") || !strcmp(mode,"exact-byte-limit"));
    UmiOllamaModelInfoDestroy(info);return 0;
}
