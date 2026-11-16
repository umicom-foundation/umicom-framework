/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_comfy_nodes.c
 *
 * PURPOSE:
 *   Verify owned ComfyUI class metadata, schema spans, bounds and saved connection identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_nodes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if(!(x)){fprintf(stderr,"%d: %s\n",__LINE__,#x);return 1;} } while(0)
static const char sample[]="{\"ExampleNode\":{\"name\":\"ExampleNode\",\"display_name\":\"Example node\",\"category\":\"testing\",\"input\":{\"required\":{\"text\":[\"STRING\",{\"default\":\"hello\"}]},\"optional\":{\"seed\":[\"INT\",{\"min\":0}]}},\"output\":[\"IMAGE\",\"MASK\"],\"output_node\":true,\"deprecated\":false,\"experimental\":true,\"api_node\":false}}";
static UmiProviderConnection Connection(void)
{
    UmiProviderConnection c={0};strcpy(c.id,"comfy-node-fixture");strcpy(c.provider_id,"comfyui");
    strcpy(c.label,"Native node fixture");strcpy(c.endpoint,"http://127.0.0.1:8188");
    c.route=UMI_PROVIDER_CONNECTION_LOOPBACK;c.timeout_ms=30000;c.enabled=true;return c;
}
static char *Many(const char *kind,size_t count)
{
    size_t capacity=count*64U+128U,used=0U;char *json=malloc(capacity);if(!json)return NULL;
    const char *prefix=strcmp(kind,"classes")==0?"{":strcmp(kind,"inputs")==0?"{\"N\":{\"input\":{\"required\":{":"{\"N\":{\"input\":{},\"output\":[";
    strcpy(json,prefix);used=strlen(json);
    for(size_t i=0U;i<count;++i){
        int n=strcmp(kind,"classes")==0?snprintf(json+used,capacity-used,"%s\"N%zu\":{\"input\":{},\"output\":[]}",i?",":"",i):
            strcmp(kind,"inputs")==0?snprintf(json+used,capacity-used,"%s\"p%zu\":[\"STRING\"]",i?",":"",i):
            snprintf(json+used,capacity-used,"%s\"TYPE\"",i?",":"");
        if(n<0 || (size_t)n>=capacity-used){free(json);return NULL;}used+=(size_t)n;
    }
    strcpy(json+used,strcmp(kind,"classes")==0?"}":strcmp(kind,"inputs")==0?"}},\"output\":[]}}":"]}}");return json;
}
static const struct {const char *mode,*json;UmiStatus expected;} fixtures[]={
    {"empty","{}",UMI_STATUS_OK},
    {"minimal","{\"N\":{\"input\":{},\"output\":[]}}",UMI_STATUS_OK},
/* The opaque-schema fixture must be valid JSON before testing schema preservation. Static fixture parsing found an extra closing brace; retain the initial fixture for review. The former implementation is retained for owner review. */
#if 0
    {"opaque","{\"N\":{\"input\":{\"required\":{\"x\":{\"custom\":true}}},\"hidden\":{\"context\":\"PROMPT\"}},\"output\":[\"*\"]}}",UMI_STATUS_OK},
#endif
    {"opaque","{\"N\":{\"input\":{\"required\":{\"x\":{\"custom\":true}},\"hidden\":{\"context\":\"PROMPT\"}},\"output\":[\"*\"]}}",UMI_STATUS_OK},
    {"unicode","{\"示例\":{\"name\":\"示例\",\"input\":{\"required\":{\"文本\":[\"STRING\"]}},\"output\":[\"图像\"],\"display_name\":\"示例\",\"category\":\"自定义\"}}",UMI_STATUS_OK},
    {"root","[]",UMI_STATUS_PARSE_ERROR},
    {"node-kind","{\"N\":null}",UMI_STATUS_PARSE_ERROR},
    {"empty-class","{\"\":{\"input\":{},\"output\":[]}}",UMI_STATUS_PARSE_ERROR},
    {"name-mismatch","{\"N\":{\"name\":\"Other\",\"input\":{},\"output\":[]}}",UMI_STATUS_PARSE_ERROR},
    {"name-kind","{\"N\":{\"name\":5,\"input\":{},\"output\":[]}}",UMI_STATUS_PARSE_ERROR},
    {"missing-input","{\"N\":{\"output\":[]}}",UMI_STATUS_PARSE_ERROR},
    {"input-kind","{\"N\":{\"input\":[],\"output\":[]}}",UMI_STATUS_PARSE_ERROR},
    {"required-kind","{\"N\":{\"input\":{\"required\":[]},\"output\":[]}}",UMI_STATUS_PARSE_ERROR},
    {"optional-kind","{\"N\":{\"input\":{\"optional\":null},\"output\":[]}}",UMI_STATUS_PARSE_ERROR},
    {"empty-input","{\"N\":{\"input\":{\"required\":{\"\":[]}},\"output\":[]}}",UMI_STATUS_PARSE_ERROR},
    {"cross-group","{\"N\":{\"input\":{\"required\":{\"x\":[]},\"optional\":{\"x\":[]}},\"output\":[]}}",UMI_STATUS_ALREADY_EXISTS},
    {"missing-output","{\"N\":{\"input\":{}}}",UMI_STATUS_PARSE_ERROR},
    {"output-kind","{\"N\":{\"input\":{},\"output\":{}}}",UMI_STATUS_PARSE_ERROR},
    {"output-type","{\"N\":{\"input\":{},\"output\":[1]}}",UMI_STATUS_PARSE_ERROR},
    {"empty-output","{\"N\":{\"input\":{},\"output\":[\"\"]}}",UMI_STATUS_PARSE_ERROR},
    {"display-kind","{\"N\":{\"input\":{},\"output\":[],\"display_name\":null}}",UMI_STATUS_PARSE_ERROR},
    {"category-kind","{\"N\":{\"input\":{},\"output\":[],\"category\":false}}",UMI_STATUS_PARSE_ERROR},
    {"flag-kind","{\"N\":{\"input\":{},\"output\":[],\"output_node\":1}}",UMI_STATUS_PARSE_ERROR},
    {"duplicate","{\"N\":{},\"N\":{}}",UMI_STATUS_ALREADY_EXISTS},
    {"escaped-duplicate","{\"N\":{},\"\\u004e\":{}}",UMI_STATUS_ALREADY_EXISTS},
    {"nested-duplicate","{\"N\":{\"input\":{},\"output\":[],\"future\":{\"x\":1,\"x\":2}}}",UMI_STATUS_ALREADY_EXISTS},
    {"malformed","{\"N\":",UMI_STATUS_PARSE_ERROR},
    {"trailing","{} false",UMI_STATUS_PARSE_ERROR},
};
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1],*json=sample;char *owned=NULL;size_t length=0;
    UmiStatus expected=UMI_STATUS_OK;UmiProviderConnection c=Connection();uint64_t revision=7U;
    UmiComfyNodeCatalogue *catalogue=NULL;UmiCancellationToken *cancel=NULL;
    int fixture=0;
    for(size_t i=0;i<sizeof fixtures/sizeof fixtures[0];++i)if(strcmp(mode,fixtures[i].mode)==0){json=fixtures[i].json;expected=fixtures[i].expected;fixture=1;break;}
    if(!fixture){
        if(!strcmp(mode,"source-order"))json="{\"Zed\":{\"input\":{},\"output\":[]},\"Alpha\":{\"input\":{},\"output\":[]}}";
        else if(!strcmp(mode,"flags-unknown"))json="{\"N\":{\"input\":{},\"output\":[],\"output_node\":false}}";
        else if(!strcmp(mode,"max-classes") || !strcmp(mode,"too-many-classes")){owned=Many("classes",!strcmp(mode,"max-classes")?4096U:4097U);CHECK(owned);json=owned;if(!strcmp(mode,"too-many-classes"))expected=UMI_STATUS_CAPACITY_EXCEEDED;}
        else if(!strcmp(mode,"max-inputs") || !strcmp(mode,"too-many-inputs")){owned=Many("inputs",!strcmp(mode,"max-inputs")?256U:257U);CHECK(owned);json=owned;if(!strcmp(mode,"too-many-inputs"))expected=UMI_STATUS_CAPACITY_EXCEEDED;}
        else if(!strcmp(mode,"max-outputs") || !strcmp(mode,"too-many-outputs")){owned=Many("outputs",!strcmp(mode,"max-outputs")?64U:65U);CHECK(owned);json=owned;if(!strcmp(mode,"too-many-outputs"))expected=UMI_STATUS_CAPACITY_EXCEEDED;}
        else if(!strcmp(mode,"max-class") || !strcmp(mode,"long-class") || !strcmp(mode,"max-metadata") || !strcmp(mode,"long-metadata") || !strcmp(mode,"long-input") || !strcmp(mode,"long-output")){
            char text[258];size_t n=strstr(mode,"metadata")?(!strcmp(mode,"max-metadata")?256U:257U):(!strcmp(mode,"max-class")?128U:129U);
            memset(text,'x',n);text[n]=0;owned=malloc(1024);CHECK(owned);
            if(strstr(mode,"metadata"))snprintf(owned,1024,"{\"N\":{\"input\":{},\"output\":[],\"display_name\":\"%s\",\"category\":\"%s\"}}",text,text);
            else if(!strcmp(mode,"long-input"))snprintf(owned,1024,"{\"N\":{\"input\":{\"required\":{\"%s\":[]}},\"output\":[]}}",text);
            else if(!strcmp(mode,"long-output"))snprintf(owned,1024,"{\"N\":{\"input\":{},\"output\":[\"%s\"]}}",text);
            else snprintf(owned,1024,"{\"%s\":{\"input\":{},\"output\":[]}}",text);
            json=owned;if(strstr(mode,"long"))expected=UMI_STATUS_CAPACITY_EXCEEDED;
        } else if(!strcmp(mode,"max-bytes") || !strcmp(mode,"too-many-bytes")){
            length=UMI_COMFY_NODES_MAX_BYTES+(!strcmp(mode,"too-many-bytes")?1U:0U);
            owned=malloc(length+1);CHECK(owned);memset(owned,' ',length);owned[0]='{';owned[1]='}';owned[length]=0;json=owned;
            if(!strcmp(mode,"too-many-bytes"))expected=UMI_STATUS_CAPACITY_EXCEEDED;
        } else if(!strcmp(mode,"cancel")){CHECK(umi_cancellation_token_create(&cancel)==UMI_STATUS_OK);umi_cancellation_token_request(cancel);expected=UMI_STATUS_CANCELLED;}
        else if(!strcmp(mode,"zero-revision")){revision=0;expected=UMI_STATUS_INVALID_ARGUMENT;}
        else if(!strcmp(mode,"disabled")){c.enabled=false;expected=UMI_STATUS_PERMISSION_DENIED;}
        else if(!strcmp(mode,"provider")){strcpy(c.provider_id,"other");expected=UMI_STATUS_PERMISSION_DENIED;}
        else if(!strcmp(mode,"remote")){c.route=UMI_PROVIDER_CONNECTION_HTTPS;strcpy(c.endpoint,"https://example.invalid");expected=UMI_STATUS_PERMISSION_DENIED;}
        else if(!strcmp(mode,"path")){strcat(c.endpoint,"/object_info");expected=UMI_STATUS_PERMISSION_DENIED;}
        else if(!strcmp(mode,"secret")){strcpy(c.secret_reference,"vault:fixture");expected=UMI_STATUS_INVALID_ARGUMENT;}
        else if(!strcmp(mode,"ownership")){owned=malloc(sizeof sample);CHECK(owned);memcpy(owned,sample,sizeof sample);json=owned;}
        else CHECK(!strcmp(mode,"metadata") || !strcmp(mode,"schema-span") || !strcmp(mode,"input-span") || !strcmp(mode,"output-types") || !strcmp(mode,"occupied") || !strcmp(mode,"same-connection") || !strcmp(mode,"changed-connection") || !strcmp(mode,"changed-revision") || !strcmp(mode,"accessor-refusal") || !strcmp(mode,"find-refusal") || !strcmp(mode,"nulls"));
    }
    if(length==0)length=strlen(json);
    CHECK(UmiComfyNodeCatalogueCreate(&c,revision,json,length,cancel,&catalogue)==expected);
    umi_cancellation_token_destroy(cancel);
    if(expected!=UMI_STATUS_OK){CHECK(catalogue==NULL);free(owned);return 0;}
    if(!strcmp(mode,"empty") || !strcmp(mode,"max-bytes"))CHECK(UmiComfyNodeCatalogueCount(catalogue)==0);
    else {
        UmiComfyNodeInfo info;CHECK(UmiComfyNodeCatalogueAt(catalogue,0,&info)==UMI_STATUS_OK);
        if(!strcmp(mode,"metadata") || !strcmp(mode,"ownership")){
            if(owned)memset(owned,'X',length);
            CHECK(!strcmp(info.class_type,"ExampleNode") && !strcmp(info.display_name,"Example node") && !strcmp(info.category,"testing"));
            CHECK(info.known_flags==15U && info.enabled_flags==(UMI_COMFY_NODE_OUTPUT_NODE|UMI_COMFY_NODE_EXPERIMENTAL));
            CHECK(info.required_count==1 && info.optional_count==1 && info.output_count==2);
            const char *span=NULL;size_t count=0;CHECK(UmiComfyNodeCatalogueSchema(catalogue,0,&span,&count)==UMI_STATUS_OK && count>0 && span[0]=='{');
        } else if(!strcmp(mode,"source-order")){
            size_t index=99;CHECK(!strcmp(info.class_type,"Zed") && UmiComfyNodeCatalogueCount(catalogue)==2);
            CHECK(UmiComfyNodeCatalogueFind(catalogue,"Alpha",&index)==UMI_STATUS_OK && index==1);
        } else if(!strcmp(mode,"flags-unknown") || !strcmp(mode,"minimal")){
            CHECK(info.enabled_flags==0 && info.known_flags==(!strcmp(mode,"minimal")?0U:UMI_COMFY_NODE_OUTPUT_NODE));
        } else if(!strcmp(mode,"input-span")){
            UmiComfyNodeInput input;CHECK(UmiComfyNodeCatalogueInputAt(catalogue,0,UMI_COMFY_INPUT_REQUIRED,0,&input)==UMI_STATUS_OK);
            CHECK(!strcmp(input.name,"text") && input.schema_bytes==strlen("[\"STRING\",{\"default\":\"hello\"}]") && !memcmp(input.schema_json,"[\"STRING\",{\"default\":\"hello\"}]",input.schema_bytes));
            CHECK(UmiComfyNodeCatalogueInputAt(catalogue,0,UMI_COMFY_INPUT_OPTIONAL,0,&input)==UMI_STATUS_OK && !strcmp(input.name,"seed"));
        } else if(!strcmp(mode,"output-types")){
            char type[129];CHECK(UmiComfyNodeCatalogueOutputAt(catalogue,0,0,type,sizeof type)==UMI_STATUS_OK && !strcmp(type,"IMAGE"));
            CHECK(UmiComfyNodeCatalogueOutputAt(catalogue,0,1,type,sizeof type)==UMI_STATUS_OK && !strcmp(type,"MASK"));
        } else if(!strcmp(mode,"schema-span")){
            const char *span=NULL;size_t count=0;CHECK(UmiComfyNodeCatalogueSchema(catalogue,0,&span,&count)==UMI_STATUS_OK);
            const char *begin=strchr(sample,':')+1;CHECK(count==strlen(begin)-1 && !memcmp(span,begin,count));
        } else if(!strcmp(mode,"max-classes"))CHECK(UmiComfyNodeCatalogueCount(catalogue)==4096);
        else if(!strcmp(mode,"max-inputs"))CHECK(info.required_count==256);
        else if(!strcmp(mode,"max-outputs"))CHECK(info.output_count==64);
        else if(!strcmp(mode,"max-class"))CHECK(strlen(info.class_type)==128);
        else if(!strcmp(mode,"max-metadata"))CHECK(strlen(info.display_name)==256 && strlen(info.category)==256);
        else if(!strcmp(mode,"unicode"))CHECK(!strcmp(info.class_type,"示例"));
    }
    if(!strcmp(mode,"occupied")){UmiComfyNodeCatalogue *before=catalogue;CHECK(UmiComfyNodeCatalogueCreate(&c,revision,sample,sizeof sample-1,NULL,&catalogue)==UMI_STATUS_INVALID_ARGUMENT && catalogue==before);}
    if(!strcmp(mode,"same-connection") || !strcmp(mode,"changed-connection") || !strcmp(mode,"changed-revision")){
        if(!strcmp(mode,"changed-connection"))strcpy(c.label,"Changed label");
        CHECK(UmiComfyNodeCatalogueCheckConnection(catalogue,&c,revision+(!strcmp(mode,"changed-revision")?1U:0U))==(!strcmp(mode,"same-connection")?UMI_STATUS_OK:UMI_STATUS_BUSY));
    }
    if(!strcmp(mode,"accessor-refusal")){
        UmiComfyNodeInfo info,before;memset(&info,0xa5,sizeof info);before=info;
        CHECK(UmiComfyNodeCatalogueAt(catalogue,1,&info)==UMI_STATUS_NOT_FOUND && !memcmp(&info,&before,sizeof info));
        UmiComfyNodeInput input,prior;memset(&input,0xa5,sizeof input);prior=input;
        CHECK(UmiComfyNodeCatalogueInputAt(catalogue,0,UMI_COMFY_INPUT_REQUIRED,1,&input)==UMI_STATUS_NOT_FOUND && !memcmp(&input,&prior,sizeof input));
        CHECK(UmiComfyNodeCatalogueInputAt(catalogue,0,(UmiComfyInputGroup)0,0,&input)==UMI_STATUS_INVALID_ARGUMENT && !memcmp(&input,&prior,sizeof input));
        char type[2]={'X','Y'};CHECK(UmiComfyNodeCatalogueOutputAt(catalogue,0,0,type,sizeof type)==UMI_STATUS_CAPACITY_EXCEEDED && type[0]=='X' && type[1]=='Y');
        const char *span=sample;size_t count=99;CHECK(UmiComfyNodeCatalogueSchema(catalogue,1,&span,&count)==UMI_STATUS_NOT_FOUND && span==sample && count==99);
    }
    if(!strcmp(mode,"find-refusal")){
        size_t index=99;char long_name[130];memset(long_name,'x',129);long_name[129]=0;
        CHECK(UmiComfyNodeCatalogueFind(catalogue,"Absent",&index)==UMI_STATUS_NOT_FOUND && index==99);
        CHECK(UmiComfyNodeCatalogueFind(catalogue,long_name,&index)==UMI_STATUS_CAPACITY_EXCEEDED && index==99);
        CHECK(UmiComfyNodeCatalogueFind(catalogue,"",&index)==UMI_STATUS_INVALID_ARGUMENT && index==99);
    }
    if(!strcmp(mode,"nulls")){
        UmiComfyNodeCatalogue *none=NULL;UmiComfyNodeInfo info;size_t index=0;
        CHECK(UmiComfyNodeCatalogueCreate(NULL,7,sample,sizeof sample-1,NULL,&none)==UMI_STATUS_INVALID_ARGUMENT && none==NULL);
        CHECK(UmiComfyNodeCatalogueCreate(&c,7,sample,sizeof sample-1,NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfyNodeCatalogueAt(NULL,0,&info)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfyNodeCatalogueAt(catalogue,0,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfyNodeCatalogueFind(catalogue,NULL,&index)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfyNodeCatalogueFind(catalogue,"ExampleNode",NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfyNodeCatalogueCheckConnection(NULL,&c,7)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfyNodeCatalogueCount(NULL)==0);UmiComfyNodeCatalogueDestroy(NULL);
    }
    UmiComfyNodeCatalogueDestroy(catalogue);free(owned);return 0;
}
