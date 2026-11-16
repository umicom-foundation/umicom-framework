/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai/test_model_discovery_query.c
 *
 * PURPOSE:
 *   Verify native model-browser filtering preserves full identities, unknown metadata and independent snapshot ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/model_discovery.h"
#include <stdio.h>
#include <string.h>
#define CHECK(e) do { if(!(e)) { fprintf(stderr,"%d: %s\n",__LINE__,#e); return 1; } } while(0)
static const char fixture[]=
"{\"models\":["
"{\"model\":\"zeta\",\"name\":\"Zulu\",\"size\":40,\"size_vram\":32,\"context_length\":4096,\"details\":{\"family\":\"llama\",\"format\":\"gguf\",\"quantization_level\":\"Q4\"}},"
"{\"model\":\"beta\",\"name\":\"Shared\",\"size\":0,\"size_vram\":0,\"context_length\":0,\"details\":{\"family\":\"llama\",\"format\":\"gguf\",\"quantization_level\":\"Q4\"}},"
"{\"model\":\"alpha\",\"name\":\"Shared\",\"size\":40,\"context_length\":8192,\"details\":{\"family\":\"qwen\",\"format\":\"gguf\",\"quantization_level\":\"Q8\"}},"
"{\"model\":\"gamma\",\"name\":\"Gamma\",\"details\":{\"family\":\"llama\"}},"
"{\"model\":\"delta\",\"name\":\"Caf\u00e9\",\"size\":10,\"context_length\":4096,\"details\":{\"family\":\"other\",\"format\":\"bin\"}}]}";
static int check_ids(const UmiAiDiscoverySnapshot *snapshot,const char *expected)
{
    char actual[128]={0};
    for(size_t i=0;i<UmiAiDiscoveryCount(snapshot);++i) {
        UmiAiDiscoveredModel model;CHECK(UmiAiDiscoveryAt(snapshot,i,&model)==UMI_STATUS_OK);
        if(i)strcat(actual,",");CHECK(strlen(actual)+strlen(model.id)<sizeof actual);strcat(actual,model.id);
    }
    CHECK(strcmp(actual,expected)==0);return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    const char *body=fixture;UmiAiDiscoveryFormat format=UMI_AI_DISCOVERY_OLLAMA_LOADED;
    if(strcmp(mode,"empty")==0)body="{\"models\":[]}";
    if(strcmp(mode,"openai")==0){body="{\"object\":\"list\",\"data\":[{\"object\":\"model\",\"id\":\"remote\"}]}";format=UMI_AI_DISCOVERY_OPENAI_LIST;}
    UmiAiDiscoverySnapshot *source=NULL,*selected=NULL;
    CHECK(UmiAiDiscoveryDecode(format,body,strlen(body),NULL,&source)==UMI_STATUS_OK);
    UmiAiDiscoveryFilter filter={0};UmiStatus expected=UMI_STATUS_OK;
    const char *ids="zeta,beta,alpha,gamma,delta";UmiCancellationToken *cancel=NULL;
    if(strcmp(mode,"source")==0 || strcmp(mode,"ownership")==0 || strcmp(mode,"source-unchanged")==0) {}
    else if(strcmp(mode,"id-order")==0 || strcmp(mode,"lookup-after-sort")==0){filter.order=UMI_AI_DISCOVERY_ORDER_ID;ids="alpha,beta,delta,gamma,zeta";}
    else if(strcmp(mode,"name-order")==0){filter.order=UMI_AI_DISCOVERY_ORDER_NAME;ids="delta,gamma,alpha,beta,zeta";}
    else if(strcmp(mode,"size-order")==0){filter.order=UMI_AI_DISCOVERY_ORDER_SIZE_ASC;ids="beta,delta,alpha,zeta,gamma";}
    else if(strcmp(mode,"context-order")==0){filter.order=UMI_AI_DISCOVERY_ORDER_CONTEXT_DESC;ids="alpha,delta,zeta,beta,gamma";}
    else if(strcmp(mode,"search-name")==0){strcpy(filter.search,"sHaReD");ids="beta,alpha";}
    else if(strcmp(mode,"search-id")==0){strcpy(filter.search,"ET");ids="zeta,beta";}
    else if(strcmp(mode,"search-family")==0){strcpy(filter.search,"lLaMa");ids="zeta,beta,gamma";}
    else if(strcmp(mode,"search-unicode")==0){strcpy(filter.search,"Caf\u00e9");ids="delta";}
    else if(strcmp(mode,"no-unicode-fold")==0){strcpy(filter.search,"CAF\u00c9");ids="";}
    else if(strcmp(mode,"family")==0){strcpy(filter.family,"llama");ids="zeta,beta,gamma";}
    else if(strcmp(mode,"facet-case")==0){strcpy(filter.family,"Llama");ids="";}
    else if(strcmp(mode,"format")==0){strcpy(filter.format,"gguf");ids="zeta,beta,alpha";}
    else if(strcmp(mode,"quantization")==0){strcpy(filter.quantization,"Q4");ids="zeta,beta";}
    else if(strcmp(mode,"combined")==0){strcpy(filter.family,"llama");strcpy(filter.format,"gguf");strcpy(filter.quantization,"Q4");filter.minimum_context_tokens=1;filter.size_limit_enabled=1;filter.maximum_size_bytes=40;ids="zeta";}
    else if(strcmp(mode,"known")==0){filter.required_known=UMI_AI_DISCOVERY_VRAM|UMI_AI_DISCOVERY_CONTEXT;ids="zeta,beta";}
    else if(strcmp(mode,"size-zero")==0){filter.size_limit_enabled=1;ids="beta";}
    else if(strcmp(mode,"size-range")==0){filter.size_limit_enabled=1;filter.maximum_size_bytes=10;ids="beta,delta";}
    else if(strcmp(mode,"context-min")==0){filter.minimum_context_tokens=4097;ids="alpha";}
    else if(strcmp(mode,"context-too-high")==0){filter.minimum_context_tokens=UINT64_MAX;ids="";}
    else if(strcmp(mode,"no-match")==0){strcpy(filter.search,"missing");ids="";}
    else if(strcmp(mode,"empty")==0){ids="";}
    else if(strcmp(mode,"openai")==0){filter.required_known=UMI_AI_DISCOVERY_SIZE;ids="";}
    else if(strcmp(mode,"unterminated-search")==0){memset(filter.search,'x',sizeof filter.search);expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"unterminated-family")==0){memset(filter.family,'x',sizeof filter.family);expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"invalid-utf8")==0){filter.search[0]=(char)0xc0;filter.search[1]=(char)0xaf;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"unknown-known")==0){filter.required_known=0x80000000U;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"invalid-order")==0){filter.order=(UmiAiDiscoveryOrder)99;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"invalid-enable")==0){filter.size_limit_enabled=2;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"disabled-limit")==0){filter.maximum_size_bytes=10;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"cancel")==0){CHECK(umi_cancellation_token_create(&cancel)==UMI_STATUS_OK);umi_cancellation_token_request(cancel);expected=UMI_STATUS_CANCELLED;}
    else if(strcmp(mode,"occupied")==0){selected=source;CHECK(UmiAiDiscoverySelect(source,&filter,NULL,&selected)==UMI_STATUS_INVALID_ARGUMENT && selected==source);UmiAiDiscoveryDestroy(source);return 0;}
    else if(strcmp(mode,"nulls")==0){CHECK(UmiAiDiscoverySelect(NULL,&filter,NULL,&selected)==UMI_STATUS_INVALID_ARGUMENT && selected==NULL);CHECK(UmiAiDiscoverySelect(source,NULL,NULL,&selected)==UMI_STATUS_INVALID_ARGUMENT);CHECK(UmiAiDiscoverySelect(source,&filter,NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);UmiAiDiscoveryDestroy(source);return 0;}
    else CHECK(0);
    CHECK(UmiAiDiscoverySelect(source,&filter,cancel,&selected)==expected);
    if(expected==UMI_STATUS_OK){
        CHECK(UmiAiDiscoveryFormatOf(selected)==format);
        if(strcmp(mode,"ownership")==0){UmiAiDiscoveryDestroy(source);source=NULL;memset(&filter,0xa5,sizeof filter);}
        CHECK(check_ids(selected,ids)==0);
        if(strcmp(mode,"lookup-after-sort")==0){UmiAiDiscoveredModel model;CHECK(UmiAiDiscoveryFind(selected,"zeta",&model)==UMI_STATUS_OK && model.size_bytes==40);}
        if(strcmp(mode,"source-unchanged")==0)CHECK(check_ids(source,"zeta,beta,alpha,gamma,delta")==0);
        if(strcmp(mode,"size-zero")==0){UmiAiDiscoveredModel model;CHECK(UmiAiDiscoveryAt(selected,0,&model)==UMI_STATUS_OK && (model.known & UMI_AI_DISCOVERY_SIZE) && model.size_bytes==0);}
    }else CHECK(selected==NULL);
    umi_cancellation_token_destroy(cancel);UmiAiDiscoveryDestroy(selected);UmiAiDiscoveryDestroy(source);return 0;
}
