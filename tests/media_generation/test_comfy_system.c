/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_comfy_system.c
 *
 * PURPOSE:
 *   Verify exact system counters, unknown metadata, copied lifetimes, connection binding and bounded failure behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;}}while(0)
typedef struct Fixture {const char *name,*json;UmiStatus status;size_t count;uint32_t system_known,device_known;} Fixture;
static const Fixture fixtures[]={
    {"complete","{\"system\":{\"os\":\"win32\",\"python_version\":\"3.x fixture\",\"pytorch_version\":\"fixture-torch\",\"comfyui_version\":\"fixture-comfy\",\"embedded_python\":false,\"ram_total\":16384,\"ram_free\":8192},\"devices\":[{\"name\":\"Synthetic GPU 東京\",\"type\":\"cuda\",\"index\":2,\"vram_total\":8192,\"vram_free\":4096,\"torch_vram_total\":4096,\"torch_vram_free\":2048}]}",UMI_STATUS_OK,1U,127U,31U},
    {"empty","{\"system\":{},\"devices\":[]}",UMI_STATUS_OK,0U,0U,0U},
    {"unknown-fields","{\"system\":{\"argv\":[\"private-argument\"],\"comfy_package_versions\":{\"extension\":\"opaque\"},\"deploy_environment\":{\"secret\":\"opaque\"}},\"devices\":[{\"name\":\"Synthetic CPU\",\"type\":\"future-backend\",\"index\":null}]}",UMI_STATUS_OK,1U,0U,0U},
    {"null-fields","{\"system\":{\"os\":null,\"python_version\":null,\"pytorch_version\":null,\"comfyui_version\":null,\"embedded_python\":null,\"ram_total\":null,\"ram_free\":null},\"devices\":[{\"name\":\"CPU\",\"type\":\"cpu\",\"index\":null,\"vram_total\":null,\"vram_free\":null,\"torch_vram_total\":null,\"torch_vram_free\":null}]}",UMI_STATUS_OK,1U,0U,0U},
    {"known-zero","{\"system\":{\"os\":\"win32\",\"python_version\":\"3.x fixture\",\"pytorch_version\":\"fixture-torch\",\"comfyui_version\":\"fixture-comfy\",\"embedded_python\":false,\"ram_total\":0,\"ram_free\":0},\"devices\":[{\"name\":\"Synthetic GPU 東京\",\"type\":\"cuda\",\"index\":0,\"vram_total\":0,\"vram_free\":0,\"torch_vram_total\":0,\"torch_vram_free\":0}]}",UMI_STATUS_OK,1U,127U,31U},
    {"embedded-true","{\"system\":{\"os\":\"win32\",\"python_version\":\"3.x fixture\",\"pytorch_version\":\"fixture-torch\",\"comfyui_version\":\"fixture-comfy\",\"embedded_python\":true,\"ram_total\":16384,\"ram_free\":8192},\"devices\":[{\"name\":\"Synthetic GPU 東京\",\"type\":\"cuda\",\"index\":2,\"vram_total\":8192,\"vram_free\":4096,\"torch_vram_total\":4096,\"torch_vram_free\":2048}]}",UMI_STATUS_OK,1U,127U,31U},
    {"inconsistent","{\"system\":{\"os\":\"win32\",\"python_version\":\"3.x fixture\",\"pytorch_version\":\"fixture-torch\",\"comfyui_version\":\"fixture-comfy\",\"embedded_python\":false,\"ram_total\":16384,\"ram_free\":99999},\"devices\":[{\"name\":\"Synthetic GPU 東京\",\"type\":\"cuda\",\"index\":2,\"vram_total\":8192,\"vram_free\":99999,\"torch_vram_total\":4096,\"torch_vram_free\":99999}]}",UMI_STATUS_OK,1U,127U,31U},
    {"free-without-total","{\"system\":{\"ram_free\":100},\"devices\":[{\"name\":\"CPU\",\"type\":\"cpu\",\"vram_free\":100,\"torch_vram_free\":100}]}",UMI_STATUS_OK,1U,64U,20U},
    {"repeated-devices","{\"system\":{\"os\":\"win32\",\"python_version\":\"3.x fixture\",\"pytorch_version\":\"fixture-torch\",\"comfyui_version\":\"fixture-comfy\",\"embedded_python\":false,\"ram_total\":16384,\"ram_free\":8192},\"devices\":[{\"name\":\"Synthetic GPU 東京\",\"type\":\"cuda\",\"index\":2,\"vram_total\":8192,\"vram_free\":4096,\"torch_vram_total\":4096,\"torch_vram_free\":2048},{\"name\":\"Synthetic GPU 東京\",\"type\":\"cuda\",\"index\":2,\"vram_total\":8192,\"vram_free\":4096,\"torch_vram_total\":4096,\"torch_vram_free\":2048}]}",UMI_STATUS_OK,2U,127U,31U},
    {"missing-system","{\"devices\":[]}",UMI_STATUS_PARSE_ERROR,0U,0U,0U},
    {"missing-devices","{\"system\":{}}",UMI_STATUS_PARSE_ERROR,0U,0U,0U},
    {"wrong-system","{\"system\":[],\"devices\":[]}",UMI_STATUS_PARSE_ERROR,0U,0U,0U},
    {"wrong-devices","{\"system\":{},\"devices\":{}}",UMI_STATUS_PARSE_ERROR,0U,0U,0U},
    {"device-type","{\"system\":{},\"devices\":[null]}",UMI_STATUS_PARSE_ERROR,0U,0U,0U},
    {"missing-name","{\"system\":{},\"devices\":[{\"type\":\"cuda\"}]}",UMI_STATUS_PARSE_ERROR,0U,0U,0U},
    {"missing-type","{\"system\":{},\"devices\":[{\"name\":\"GPU\"}]}",UMI_STATUS_PARSE_ERROR,0U,0U,0U},
    {"empty-name","{\"system\":{},\"devices\":[{\"name\":\"\",\"type\":\"cpu\"}]}",UMI_STATUS_PARSE_ERROR,0U,0U,0U},
    {"empty-type","{\"system\":{},\"devices\":[{\"name\":\"CPU\",\"type\":\"\"}]}",UMI_STATUS_PARSE_ERROR,0U,0U,0U},
    {"name-type","{\"system\":{},\"devices\":[{\"name\":1,\"type\":\"cpu\"}]}",UMI_STATUS_PARSE_ERROR,0U,0U,0U},
    {"version-type","{\"system\":{\"python_version\":1},\"devices\":[]}",UMI_STATUS_PARSE_ERROR,0U,0U,0U},
    {"embedded-type","{\"system\":{\"embedded_python\":1},\"devices\":[]}",UMI_STATUS_PARSE_ERROR,0U,0U,0U},
/* Shared JSON ownership reports duplicate decoded keys as ALREADY_EXISTS. Preserve the initial incompatible expectation for review and assert the actual public contract. The former implementation is retained for owner review. */
#if 0
    {"duplicate-key","{\"system\":{},\"devices\":[],\"system\":{}}",UMI_STATUS_PARSE_ERROR,0U,0U,0U},
#endif
    {"duplicate-key","{\"system\":{},\"devices\":[],\"system\":{}}",UMI_STATUS_ALREADY_EXISTS,0U,0U,0U},
/* Shared JSON ownership reports duplicate decoded keys as ALREADY_EXISTS. Preserve the initial incompatible expectation for review and assert the actual public contract. The former implementation is retained for owner review. */
#if 0
    {"escaped-duplicate","{\"system\":{},\"devices\":[],\"syst\\u0065m\":{}}",UMI_STATUS_PARSE_ERROR,0U,0U,0U},
#endif
    {"escaped-duplicate","{\"system\":{},\"devices\":[],\"syst\\u0065m\":{}}",UMI_STATUS_ALREADY_EXISTS,0U,0U,0U},
/* Shared JSON ownership reports duplicate decoded keys as ALREADY_EXISTS. Preserve the initial incompatible expectation for review and assert the actual public contract. The former implementation is retained for owner review. */
#if 0
    {"opaque-duplicate","{\"system\":{\"argv\":{\"x\":1,\"x\":2}},\"devices\":[]}",UMI_STATUS_PARSE_ERROR,0U,0U,0U},
#endif
    {"opaque-duplicate","{\"system\":{\"argv\":{\"x\":1,\"x\":2}},\"devices\":[]}",UMI_STATUS_ALREADY_EXISTS,0U,0U,0U},
    {"decoded-nul","{\"system\":{\"os\":\"a\\u0000b\"},\"devices\":[]}",UMI_STATUS_PARSE_ERROR,0U,0U,0U},
    {"unpaired","{\"system\":{\"os\":\"\\ud800\"},\"devices\":[]}",UMI_STATUS_PARSE_ERROR,0U,0U,0U},
    {"trailing","{\"system\":{},\"devices\":[]} true",UMI_STATUS_PARSE_ERROR,0U,0U,0U},
    {"wrong-root","[]",UMI_STATUS_PARSE_ERROR,0U,0U,0U},
    {"exact-integer","{\"system\":{\"ram_total\":9007199254740993},\"devices\":[{\"name\":\"CPU\",\"type\":\"cpu\",\"index\":9007199254740993}]}",UMI_STATUS_OK,1U,32U,1U},
    {"max-integer","{\"system\":{\"ram_total\":9223372036854775807},\"devices\":[{\"name\":\"CPU\",\"type\":\"cpu\",\"index\":9223372036854775807}]}",UMI_STATUS_OK,1U,32U,1U},
    {"minus-zero","{\"system\":{\"ram_total\":-0},\"devices\":[{\"name\":\"CPU\",\"type\":\"cpu\",\"index\":-0}]}",UMI_STATUS_OK,1U,32U,1U},
    {"os-boundary","{\"system\":{\"os\":\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\",\"python_version\":\"3.x fixture\",\"pytorch_version\":\"fixture-torch\",\"comfyui_version\":\"fixture-comfy\",\"embedded_python\":false,\"ram_total\":16384,\"ram_free\":8192},\"devices\":[{\"name\":\"Synthetic GPU 東京\",\"type\":\"cuda\",\"index\":2,\"vram_total\":8192,\"vram_free\":4096,\"torch_vram_total\":4096,\"torch_vram_free\":2048}]}",UMI_STATUS_OK,1U,127U,31U},
    {"os-limit","{\"system\":{\"os\":\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\",\"python_version\":\"3.x fixture\",\"pytorch_version\":\"fixture-torch\",\"comfyui_version\":\"fixture-comfy\",\"embedded_python\":false,\"ram_total\":16384,\"ram_free\":8192},\"devices\":[{\"name\":\"Synthetic GPU 東京\",\"type\":\"cuda\",\"index\":2,\"vram_total\":8192,\"vram_free\":4096,\"torch_vram_total\":4096,\"torch_vram_free\":2048}]}",UMI_STATUS_CAPACITY_EXCEEDED,1U,127U,31U},
    {"python-boundary","{\"system\":{\"os\":\"win32\",\"python_version\":\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\",\"pytorch_version\":\"fixture-torch\",\"comfyui_version\":\"fixture-comfy\",\"embedded_python\":false,\"ram_total\":16384,\"ram_free\":8192},\"devices\":[{\"name\":\"Synthetic GPU 東京\",\"type\":\"cuda\",\"index\":2,\"vram_total\":8192,\"vram_free\":4096,\"torch_vram_total\":4096,\"torch_vram_free\":2048}]}",UMI_STATUS_OK,1U,127U,31U},
    {"python-limit","{\"system\":{\"os\":\"win32\",\"python_version\":\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\",\"pytorch_version\":\"fixture-torch\",\"comfyui_version\":\"fixture-comfy\",\"embedded_python\":false,\"ram_total\":16384,\"ram_free\":8192},\"devices\":[{\"name\":\"Synthetic GPU 東京\",\"type\":\"cuda\",\"index\":2,\"vram_total\":8192,\"vram_free\":4096,\"torch_vram_total\":4096,\"torch_vram_free\":2048}]}",UMI_STATUS_CAPACITY_EXCEEDED,1U,127U,31U},
    {"torch-boundary","{\"system\":{\"os\":\"win32\",\"python_version\":\"3.x fixture\",\"pytorch_version\":\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\",\"comfyui_version\":\"fixture-comfy\",\"embedded_python\":false,\"ram_total\":16384,\"ram_free\":8192},\"devices\":[{\"name\":\"Synthetic GPU 東京\",\"type\":\"cuda\",\"index\":2,\"vram_total\":8192,\"vram_free\":4096,\"torch_vram_total\":4096,\"torch_vram_free\":2048}]}",UMI_STATUS_OK,1U,127U,31U},
    {"torch-limit","{\"system\":{\"os\":\"win32\",\"python_version\":\"3.x fixture\",\"pytorch_version\":\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\",\"comfyui_version\":\"fixture-comfy\",\"embedded_python\":false,\"ram_total\":16384,\"ram_free\":8192},\"devices\":[{\"name\":\"Synthetic GPU 東京\",\"type\":\"cuda\",\"index\":2,\"vram_total\":8192,\"vram_free\":4096,\"torch_vram_total\":4096,\"torch_vram_free\":2048}]}",UMI_STATUS_CAPACITY_EXCEEDED,1U,127U,31U},
    {"version-boundary","{\"system\":{\"os\":\"win32\",\"python_version\":\"3.x fixture\",\"pytorch_version\":\"fixture-torch\",\"comfyui_version\":\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\",\"embedded_python\":false,\"ram_total\":16384,\"ram_free\":8192},\"devices\":[{\"name\":\"Synthetic GPU 東京\",\"type\":\"cuda\",\"index\":2,\"vram_total\":8192,\"vram_free\":4096,\"torch_vram_total\":4096,\"torch_vram_free\":2048}]}",UMI_STATUS_OK,1U,127U,31U},
    {"version-limit","{\"system\":{\"os\":\"win32\",\"python_version\":\"3.x fixture\",\"pytorch_version\":\"fixture-torch\",\"comfyui_version\":\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\",\"embedded_python\":false,\"ram_total\":16384,\"ram_free\":8192},\"devices\":[{\"name\":\"Synthetic GPU 東京\",\"type\":\"cuda\",\"index\":2,\"vram_total\":8192,\"vram_free\":4096,\"torch_vram_total\":4096,\"torch_vram_free\":2048}]}",UMI_STATUS_CAPACITY_EXCEEDED,1U,127U,31U},
    {"name-boundary","{\"system\":{\"os\":\"win32\",\"python_version\":\"3.x fixture\",\"pytorch_version\":\"fixture-torch\",\"comfyui_version\":\"fixture-comfy\",\"embedded_python\":false,\"ram_total\":16384,\"ram_free\":8192},\"devices\":[{\"name\":\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\",\"type\":\"cuda\",\"index\":2,\"vram_total\":8192,\"vram_free\":4096,\"torch_vram_total\":4096,\"torch_vram_free\":2048}]}",UMI_STATUS_OK,1U,127U,31U},
    {"name-limit","{\"system\":{\"os\":\"win32\",\"python_version\":\"3.x fixture\",\"pytorch_version\":\"fixture-torch\",\"comfyui_version\":\"fixture-comfy\",\"embedded_python\":false,\"ram_total\":16384,\"ram_free\":8192},\"devices\":[{\"name\":\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\",\"type\":\"cuda\",\"index\":2,\"vram_total\":8192,\"vram_free\":4096,\"torch_vram_total\":4096,\"torch_vram_free\":2048}]}",UMI_STATUS_CAPACITY_EXCEEDED,1U,127U,31U},
    {"type-boundary","{\"system\":{\"os\":\"win32\",\"python_version\":\"3.x fixture\",\"pytorch_version\":\"fixture-torch\",\"comfyui_version\":\"fixture-comfy\",\"embedded_python\":false,\"ram_total\":16384,\"ram_free\":8192},\"devices\":[{\"name\":\"Synthetic GPU 東京\",\"type\":\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\",\"index\":2,\"vram_total\":8192,\"vram_free\":4096,\"torch_vram_total\":4096,\"torch_vram_free\":2048}]}",UMI_STATUS_OK,1U,127U,31U},
    {"type-limit","{\"system\":{\"os\":\"win32\",\"python_version\":\"3.x fixture\",\"pytorch_version\":\"fixture-torch\",\"comfyui_version\":\"fixture-comfy\",\"embedded_python\":false,\"ram_total\":16384,\"ram_free\":8192},\"devices\":[{\"name\":\"Synthetic GPU 東京\",\"type\":\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\",\"index\":2,\"vram_total\":8192,\"vram_free\":4096,\"torch_vram_total\":4096,\"torch_vram_free\":2048}]}",UMI_STATUS_CAPACITY_EXCEEDED,1U,127U,31U}
};
static UmiProviderConnection Connection(void)
{
    UmiProviderConnection c={0};
    strcpy(c.id,"stats-fixture");strcpy(c.provider_id,"comfyui");
    strcpy(c.label,"System fixture");strcpy(c.endpoint,"http://127.0.0.1:8188");
    c.enabled=true;c.route=UMI_PROVIDER_CONNECTION_LOOPBACK;c.timeout_ms=30000;return c;
}
static int Expect(const void *bytes,size_t length,UmiStatus expected)
{
    UmiProviderConnection c=Connection();UmiComfySystemStats *owner=NULL;
    CHECK(UmiComfySystemStatsCreate(&c,1U,bytes,length,NULL,&owner)==expected);
    if(expected!=UMI_STATUS_OK)CHECK(owner==NULL);
    UmiComfySystemStatsDestroy(owner);return 0;
}
static char *Many(size_t count)
{
    size_t capacity=count*64U+64U,used=0U;char *text=malloc(capacity);if(text==NULL)return NULL;
    strcpy(text,"{\"system\":{},\"devices\":[");used=strlen(text);
    for(size_t i=0U;i<count;++i){
        int n=snprintf(text+used,capacity-used,"%s{\"name\":\"device-%zu\",\"type\":\"future\"}",i?",":"",i);
        if(n<0 || (size_t)n>=capacity-used){free(text);return NULL;}used+=(size_t)n;
    }
    strcpy(text+used,"]}");return text;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    UmiProviderConnection connection=Connection();UmiComfySystemStats *owner=NULL;
    for(size_t i=0U;i<sizeof fixtures/sizeof fixtures[0];++i)if(strcmp(mode,fixtures[i].name)==0){
        const Fixture *f=&fixtures[i];
        CHECK(UmiComfySystemStatsCreate(&connection,1U,f->json,strlen(f->json),NULL,&owner)==f->status);
        if(f->status!=UMI_STATUS_OK){CHECK(owner==NULL);return 0;}
        UmiComfySystemSummary summary;CHECK(UmiComfySystemStatsInspect(owner,&summary)==UMI_STATUS_OK);
        CHECK(summary.known==f->system_known && summary.device_count==f->count);
        CHECK(summary.response_bytes==strlen(f->json) && summary.connection_revision==1U);
        UmiComfyDeviceObservation device={0};
        if(f->count){CHECK(UmiComfySystemStatsDeviceAt(owner,0U,&device)==UMI_STATUS_OK);CHECK(device.known==f->device_known);}
        if(strcmp(mode,"complete")==0){
            CHECK(strcmp(summary.os,"win32")==0 && strcmp(summary.python_version,"3.x fixture")==0);
            CHECK(strcmp(summary.pytorch_version,"fixture-torch")==0 && strcmp(summary.comfyui_version,"fixture-comfy")==0);
            CHECK(!summary.embedded_python && summary.ram_total==16384U && summary.ram_free==8192U);
            CHECK(strcmp(device.name,"Synthetic GPU 東京")==0 && strcmp(device.type,"cuda")==0);
            CHECK(device.index==2U && device.vram_total==8192U && device.vram_free==4096U);
            CHECK(device.torch_vram_total==4096U && device.torch_vram_free==2048U);
        }
        if(strcmp(mode,"inconsistent")==0){CHECK(summary.ram_inconsistent && device.vram_inconsistent && device.torch_vram_inconsistent);}
        else CHECK(!summary.ram_inconsistent && !device.vram_inconsistent && !device.torch_vram_inconsistent);
        if(strcmp(mode,"known-zero")==0)CHECK(summary.ram_total==0U && summary.ram_free==0U && device.index==0U &&
            device.vram_total==0U && device.vram_free==0U && device.torch_vram_total==0U && device.torch_vram_free==0U);
        if(strcmp(mode,"embedded-true")==0)CHECK(summary.embedded_python);
        if(strcmp(mode,"null-fields")==0 || strcmp(mode,"unknown-fields")==0)
            CHECK(summary.os[0]=='\0' && summary.python_version[0]=='\0' && summary.ram_total==0U && device.index==0U);
        if(strcmp(mode,"unknown-fields")==0)CHECK(strcmp(device.type,"future-backend")==0);
        if(strcmp(mode,"exact-integer")==0)CHECK(summary.ram_total==UINT64_C(9007199254740993) && device.index==UINT64_C(9007199254740993));
        if(strcmp(mode,"max-integer")==0)CHECK(summary.ram_total==(uint64_t)INT64_MAX && device.index==(uint64_t)INT64_MAX);
        if(strcmp(mode,"minus-zero")==0)CHECK(summary.ram_total==0U && device.index==0U);
        if(strcmp(mode,"repeated-devices")==0){UmiComfyDeviceObservation second;CHECK(UmiComfySystemStatsDeviceAt(owner,1U,&second)==UMI_STATUS_OK);CHECK(strcmp(second.name,device.name)==0);}
        UmiComfySystemStatsDestroy(owner);return 0;
    }
    if(strcmp(mode,"numeric-types")==0 || strcmp(mode,"integer-overflow")==0){
        const char *system_fields[]={"ram_total","ram_free"};
        const char *device_fields[]={"index","vram_total","vram_free","torch_vram_total","torch_vram_free"};
        const char *bad[]={"-1","1.5","1e3","\"12\"","true","{}","[]"};
        for(size_t group=0U;group<2U;++group)for(size_t field=0U;field<(group==0U?2U:5U);++field)
        for(size_t value=0U;value<(strcmp(mode,"integer-overflow")==0?1U:7U);++value){
            const char *name=group==0U?system_fields[field]:device_fields[field];
            const char *token=strcmp(mode,"integer-overflow")==0?"9223372036854775808":bad[value];
            char json[512];int n=group==0U?
                snprintf(json,sizeof json,"{\"system\":{\"%s\":%s},\"devices\":[]}",name,token):
                snprintf(json,sizeof json,"{\"system\":{},\"devices\":[{\"name\":\"CPU\",\"type\":\"cpu\",\"%s\":%s}]}",name,token);
            CHECK(n>0 && (size_t)n<sizeof json);
            CHECK(Expect(json,(size_t)n,strcmp(mode,"integer-overflow")==0?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_PARSE_ERROR)==0);
        }
        return 0;
    }
    if(strcmp(mode,"invalid-utf8")==0){const unsigned char text[]={'{','"',0xff,'"',':','0','}'};return Expect(text,sizeof text,UMI_STATUS_PARSE_ERROR);}
    if(strcmp(mode,"byte-limit")==0 || strcmp(mode,"exact-byte-limit")==0){
        size_t length=UMI_COMFY_SYSTEM_MAX_BYTES+(strcmp(mode,"byte-limit")==0?1U:0U);
        char *text=malloc(length);CHECK(text!=NULL);memset(text,' ',length);
        const char *empty="{\"system\":{},\"devices\":[]}";memcpy(text,empty,strlen(empty));
        int result=Expect(text,length,length>UMI_COMFY_SYSTEM_MAX_BYTES?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK);free(text);return result;
    }
    if(strcmp(mode,"device-limit")==0 || strcmp(mode,"maximum-devices")==0){
        size_t count=UMI_COMFY_SYSTEM_DEVICE_LIMIT+(strcmp(mode,"device-limit")==0?1U:0U);
        char *text=Many(count);CHECK(text!=NULL);
        UmiStatus status=UmiComfySystemStatsCreate(&connection,1U,text,strlen(text),NULL,&owner);free(text);
        if(count>UMI_COMFY_SYSTEM_DEVICE_LIMIT){CHECK(status==UMI_STATUS_CAPACITY_EXCEEDED && owner==NULL);return 0;}
        CHECK(status==UMI_STATUS_OK);UmiComfySystemSummary summary;UmiComfyDeviceObservation row;
        CHECK(UmiComfySystemStatsInspect(owner,&summary)==UMI_STATUS_OK && summary.device_count==64U);
        CHECK(UmiComfySystemStatsDeviceAt(owner,63U,&row)==UMI_STATUS_OK && strcmp(row.name,"device-63")==0);
        UmiComfySystemStatsDestroy(owner);return 0;
    }
    if(strcmp(mode,"depth-limit")==0){
        char text[256];size_t n=0U;for(size_t i=0U;i<40U;++i)text[n++]='[';text[n++]='0';
        for(size_t i=0U;i<40U;++i)text[n++]=']';
        return Expect(text,n,UMI_STATUS_CAPACITY_EXCEEDED);
    }
    if(strcmp(mode,"node-limit")==0){
        char *text=malloc(150000U);CHECK(text!=NULL);size_t n=1U;text[0]='[';
        for(size_t i=0U;i<65536U;++i){if(i)text[n++]=',';text[n++]='0';}text[n++]=']';
        int result=Expect(text,n,UMI_STATUS_CAPACITY_EXCEEDED);free(text);return result;
    }
    const char *sample=fixtures[0].json;size_t length=strlen(sample);
    if(strcmp(mode,"nulls")==0){
        CHECK(UmiComfySystemStatsCreate(NULL,1U,sample,length,NULL,&owner)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfySystemStatsCreate(&connection,1U,NULL,length,NULL,&owner)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfySystemStatsCreate(&connection,1U,sample,0U,NULL,&owner)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfySystemStatsCreate(&connection,1U,sample,length,NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfySystemStatsCheckConnection(NULL,&connection,1U)==UMI_STATUS_INVALID_ARGUMENT && owner==NULL);
        UmiComfySystemStatsDestroy(NULL);return 0;
    }
    if(strcmp(mode,"zero-revision")==0){CHECK(UmiComfySystemStatsCreate(&connection,0U,sample,length,NULL,&owner)==UMI_STATUS_INVALID_ARGUMENT && owner==NULL);return 0;}
    if(strcmp(mode,"disabled")==0 || strcmp(mode,"wrong-provider")==0 || strcmp(mode,"origin-path")==0){
        if(strcmp(mode,"disabled")==0)connection.enabled=false;
        if(strcmp(mode,"wrong-provider")==0)strcpy(connection.provider_id,"ollama");
        if(strcmp(mode,"origin-path")==0)strcat(connection.endpoint,"/proxy");
        CHECK(UmiComfySystemStatsCreate(&connection,1U,sample,length,NULL,&owner)==UMI_STATUS_PERMISSION_DENIED && owner==NULL);return 0;
    }
    if(strcmp(mode,"cancel")==0){
        UmiCancellationToken *token=NULL;CHECK(umi_cancellation_token_create(&token)==UMI_STATUS_OK);umi_cancellation_token_request(token);
        CHECK(UmiComfySystemStatsCreate(&connection,1U,sample,length,token,&owner)==UMI_STATUS_CANCELLED && owner==NULL);
        umi_cancellation_token_destroy(token);return 0;
    }
    if(strcmp(mode,"empty-text")==0){
        const char *text="{\"system\":{\"os\":\"\",\"python_version\":\"\",\"pytorch_version\":\"\",\"comfyui_version\":\"\"},\"devices\":[]}";
        CHECK(UmiComfySystemStatsCreate(&connection,1U,text,strlen(text),NULL,&owner)==UMI_STATUS_OK);
        UmiComfySystemSummary summary;CHECK(UmiComfySystemStatsInspect(owner,&summary)==UMI_STATUS_OK && summary.known==15U && summary.python_version[0]=='\0');
        UmiComfySystemStatsDestroy(owner);return 0;
    }
    char *input=malloc(length+1U);CHECK(input!=NULL);memcpy(input,sample,length+1U);
    CHECK(UmiComfySystemStatsCreate(&connection,1U,input,length,NULL,&owner)==UMI_STATUS_OK);
    memset(input,'x',length);free(input);
    if(strcmp(mode,"occupied")==0){UmiComfySystemStats *before=owner;CHECK(UmiComfySystemStatsCreate(&connection,1U,sample,length,NULL,&owner)==UMI_STATUS_INVALID_ARGUMENT && owner==before);}
    else if(strcmp(mode,"ownership")==0){UmiComfyDeviceObservation row;CHECK(UmiComfySystemStatsDeviceAt(owner,0U,&row)==UMI_STATUS_OK && strcmp(row.name,"Synthetic GPU 東京")==0);}
    else if(strcmp(mode,"copied-output")==0){
        UmiComfySystemSummary summary;UmiComfyDeviceObservation row;
        CHECK(UmiComfySystemStatsInspect(owner,&summary)==UMI_STATUS_OK);CHECK(UmiComfySystemStatsDeviceAt(owner,0U,&row)==UMI_STATUS_OK);
        memset(&summary,0,sizeof summary);memset(&row,0,sizeof row);
        CHECK(UmiComfySystemStatsInspect(owner,&summary)==UMI_STATUS_OK && summary.ram_total==16384U);
        CHECK(UmiComfySystemStatsDeviceAt(owner,0U,&row)==UMI_STATUS_OK && row.index==2U);
        UmiComfySystemStatsDestroy(owner);owner=NULL;CHECK(summary.ram_free==8192U && row.vram_free==4096U);
    }
    else if(strcmp(mode,"summary-refusal")==0){
        UmiComfySystemSummary out,before;memset(&out,0xa5,sizeof out);memcpy(&before,&out,sizeof out);
        CHECK(UmiComfySystemStatsInspect(NULL,&out)==UMI_STATUS_INVALID_ARGUMENT && memcmp(&out,&before,sizeof out)==0);
        CHECK(UmiComfySystemStatsInspect(owner,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    }
    else if(strcmp(mode,"device-refusal")==0){
        UmiComfyDeviceObservation out,before;memset(&out,0xa5,sizeof out);memcpy(&before,&out,sizeof out);
        CHECK(UmiComfySystemStatsDeviceAt(owner,1U,&out)==UMI_STATUS_NOT_FOUND && memcmp(&out,&before,sizeof out)==0);
        CHECK(UmiComfySystemStatsDeviceAt(owner,SIZE_MAX,&out)==UMI_STATUS_NOT_FOUND && memcmp(&out,&before,sizeof out)==0);
        CHECK(UmiComfySystemStatsDeviceAt(NULL,0U,&out)==UMI_STATUS_INVALID_ARGUMENT && memcmp(&out,&before,sizeof out)==0);
        CHECK(UmiComfySystemStatsDeviceAt(owner,0U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    }
    else if(strcmp(mode,"connection-revision")==0){
        CHECK(UmiComfySystemStatsCheckConnection(owner,&connection,1U)==UMI_STATUS_OK);
        CHECK(UmiComfySystemStatsCheckConnection(owner,&connection,2U)==UMI_STATUS_BUSY);
        CHECK(UmiComfySystemStatsCheckConnection(owner,NULL,1U)==UMI_STATUS_INVALID_ARGUMENT);
    }
    else if(strcmp(mode,"connection-content")==0){
        strcpy(connection.label,"Edited label");CHECK(UmiComfySystemStatsCheckConnection(owner,&connection,1U)==UMI_STATUS_BUSY);
        connection.enabled=false;CHECK(UmiComfySystemStatsCheckConnection(owner,&connection,1U)==UMI_STATUS_PERMISSION_DENIED);
    }
    else if(strcmp(mode,"identity")==0){
        UmiComfySystemSummary summary;char hex[UMI_SHA256_HEX_CAPACITY];unsigned char digest[UMI_SHA256_BYTES];
        CHECK(UmiComfySystemStatsInspect(owner,&summary)==UMI_STATUS_OK);
        UmiSha256Hex(summary.response_sha256,hex);CHECK(strcmp(hex,"6835e17afcb2d7e78939223126adb5551545e67db40c0864eecdf3f57ce14047")==0);
        CHECK(UmiProviderConnectionFingerprint(&connection,digest)==UMI_STATUS_OK && memcmp(digest,summary.connection_sha256,sizeof digest)==0);
    }
    else {UmiComfySystemStatsDestroy(owner);fprintf(stderr,"unknown case: %s\n",mode);return 2;}
    UmiComfySystemStatsDestroy(owner);return 0;
}
