/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/model_formats/test_safetensors.c
 *
 * PURPOSE:
 *   Verify native model header bounds, exact layout, schema refusal and output preservation using synthetic metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "safetensors_fixture.h"
#define CHECK(condition) do { if (!(condition)) { fprintf(stderr,"line %d: %s\n",__LINE__,#condition); return 1; } } while (0)
typedef struct TensorVector { const char *mode,*json;uint64_t data;UmiStatus status;size_t tensors,metadata,rank;uint64_t elements; } TensorVector;
#define BAD(mode,json,data,status) {mode,json,data,status,0U,0U,0U,0U}
static const TensorVector vectors[]={
    {"valid",tensor_fixture_json,24U,UMI_STATUS_OK,1U,1U,2U,6U},
    {"empty","{}",0U,UMI_STATUS_OK,0U,0U,0U,0U},
    {"metadata-only","{\"__metadata__\":{\"name\":\"synthetic\"}}",0U,UMI_STATUS_OK,0U,1U,0U,0U},
    {"scalar","{\"x\":{\"dtype\":\"F64\",\"shape\":[],\"data_offsets\":[0,8]}}",8U,UMI_STATUS_OK,1U,0U,0U,1U},
    {"zero-dimension","{\"x\":{\"dtype\":\"F32\",\"shape\":[2,0,3],\"data_offsets\":[0,0]}}",0U,UMI_STATUS_OK,1U,0U,3U,0U},
    {"large-zero-dimension","{\"x\":{\"dtype\":\"F32\",\"shape\":[18446744073709551615,0],\"data_offsets\":[0,0]}}",0U,UMI_STATUS_OK,1U,0U,2U,0U},
    {"unordered","{\"b\":{\"dtype\":\"F32\",\"shape\":[1],\"data_offsets\":[4,8]},\"a\":{\"dtype\":\"F32\",\"shape\":[1],\"data_offsets\":[0,4]}}",8U,UMI_STATUS_OK,2U,0U,1U,2U},
    {"empty-boundary","{\"b\":{\"dtype\":\"F32\",\"shape\":[1],\"data_offsets\":[0,4]},\"a\":{\"dtype\":\"F32\",\"shape\":[0],\"data_offsets\":[0,0]}}",4U,UMI_STATUS_OK,2U,0U,1U,1U},
    {"space-padding","{}   ",0U,UMI_STATUS_OK,0U,0U,0U,0U},
    BAD("tab-padding","{}\t",0U,UMI_STATUS_PARSE_ERROR),
    BAD("leading-space"," {}",0U,UMI_STATUS_PARSE_ERROR),
    BAD("root-array","[]",0U,UMI_STATUS_PARSE_ERROR),
    BAD("bad-json","{broken}",0U,UMI_STATUS_PARSE_ERROR),
    BAD("duplicate-root","{\"x\":{},\"x\":{}}",0U,UMI_STATUS_ALREADY_EXISTS),
    BAD("escaped-root","{\"x\":{},\"\\u0078\":{}}",0U,UMI_STATUS_ALREADY_EXISTS),
    BAD("duplicate-metadata","{\"__metadata__\":{\"a\":\"one\",\"a\":\"two\"}}",0U,UMI_STATUS_ALREADY_EXISTS),
    BAD("duplicate-dtype","{\"x\":{\"dtype\":\"F32\",\"dtype\":\"F64\",\"shape\":[1],\"data_offsets\":[0,4]}}",4U,UMI_STATUS_ALREADY_EXISTS),
    BAD("metadata-value","{\"__metadata__\":{\"a\":1}}",0U,UMI_STATUS_PARSE_ERROR),
    BAD("metadata-array","{\"__metadata__\":[]}",0U,UMI_STATUS_PARSE_ERROR),
    BAD("record-array","{\"x\":[]}",0U,UMI_STATUS_PARSE_ERROR),
    BAD("missing-field","{\"x\":{\"dtype\":\"F32\",\"shape\":[1]}}",4U,UMI_STATUS_PARSE_ERROR),
    BAD("unknown-field","{\"x\":{\"dtype\":\"F32\",\"shape\":[1],\"unknown\":[0,4]}}",4U,UMI_STATUS_PARSE_ERROR),
    BAD("extra-field","{\"x\":{\"dtype\":\"F32\",\"shape\":[1],\"data_offsets\":[0,4],\"other\":1}}",4U,UMI_STATUS_NOT_IMPLEMENTED),
    BAD("unknown-dtype","{\"x\":{\"dtype\":\"CUSTOM\",\"shape\":[1],\"data_offsets\":[0,4]}}",4U,UMI_STATUS_NOT_IMPLEMENTED),
    BAD("sub-byte-dtype","{\"x\":{\"dtype\":\"F4\",\"shape\":[2],\"data_offsets\":[0,1]}}",1U,UMI_STATUS_NOT_IMPLEMENTED),
    BAD("nonstring-dtype","{\"x\":{\"dtype\":32,\"shape\":[1],\"data_offsets\":[0,4]}}",4U,UMI_STATUS_PARSE_ERROR),
    BAD("shape-not-array","{\"x\":{\"dtype\":\"F32\",\"shape\":1,\"data_offsets\":[0,4]}}",4U,UMI_STATUS_PARSE_ERROR),
    BAD("negative-dimension","{\"x\":{\"dtype\":\"F32\",\"shape\":[-1],\"data_offsets\":[0,4]}}",4U,UMI_STATUS_PARSE_ERROR),
    BAD("fractional-dimension","{\"x\":{\"dtype\":\"F32\",\"shape\":[1.0],\"data_offsets\":[0,4]}}",4U,UMI_STATUS_PARSE_ERROR),
    BAD("string-dimension","{\"x\":{\"dtype\":\"F32\",\"shape\":[\"1\"],\"data_offsets\":[0,4]}}",4U,UMI_STATUS_PARSE_ERROR),
    BAD("dimension-overflow","{\"x\":{\"dtype\":\"U8\",\"shape\":[18446744073709551616],\"data_offsets\":[0,0]}}",0U,UMI_STATUS_CAPACITY_EXCEEDED),
    BAD("product-overflow","{\"x\":{\"dtype\":\"U8\",\"shape\":[18446744073709551615,2],\"data_offsets\":[0,0]}}",0U,UMI_STATUS_CAPACITY_EXCEEDED),
    BAD("byte-overflow","{\"x\":{\"dtype\":\"F64\",\"shape\":[18446744073709551615],\"data_offsets\":[0,0]}}",0U,UMI_STATUS_CAPACITY_EXCEEDED),
    BAD("offset-count","{\"x\":{\"dtype\":\"F32\",\"shape\":[1],\"data_offsets\":[0,4,4]}}",4U,UMI_STATUS_PARSE_ERROR),
    BAD("negative-offset","{\"x\":{\"dtype\":\"F32\",\"shape\":[1],\"data_offsets\":[-1,4]}}",4U,UMI_STATUS_PARSE_ERROR),
    BAD("reversed-offset","{\"x\":{\"dtype\":\"F32\",\"shape\":[1],\"data_offsets\":[4,0]}}",4U,UMI_STATUS_PARSE_ERROR),
    BAD("span-mismatch","{\"x\":{\"dtype\":\"F32\",\"shape\":[2],\"data_offsets\":[0,4]}}",4U,UMI_STATUS_PARSE_ERROR),
    BAD("out-of-buffer","{\"x\":{\"dtype\":\"F32\",\"shape\":[1],\"data_offsets\":[0,4]}}",3U,UMI_STATUS_PARSE_ERROR),
    BAD("gap","{\"x\":{\"dtype\":\"F32\",\"shape\":[1],\"data_offsets\":[4,8]}}",8U,UMI_STATUS_PARSE_ERROR),
    BAD("overlap","{\"a\":{\"dtype\":\"F32\",\"shape\":[1],\"data_offsets\":[0,4]},\"b\":{\"dtype\":\"F32\",\"shape\":[1],\"data_offsets\":[0,4]}}",4U,UMI_STATUS_PARSE_ERROR),
    BAD("trailing-data","{\"x\":{\"dtype\":\"F32\",\"shape\":[1],\"data_offsets\":[0,4]}}",5U,UMI_STATUS_PARSE_ERROR),
    BAD("empty-payload-gap","{}",1U,UMI_STATUS_PARSE_ERROR),
    BAD("nul-text","{\"__metadata__\":{\"name\":\"\\u0000\"}}",0U,UMI_STATUS_PARSE_ERROR)
};
static int inspect(const char *json,uint64_t data,UmiStatus wanted,size_t tensors,
    size_t metadata,size_t rank,uint64_t elements,const char *mode)
{
    size_t length;unsigned char *prefix=tensor_fixture_prefix(json,strcmp(mode,"payload-ignored")==0?24U:0U,&length);
    CHECK(prefix!=NULL);
    uint64_t file_bytes=8U+strlen(json)+data;
    UmiSafetensorsHeaderInfo info;memset(&info,0xa5,sizeof(info));
    unsigned char before[sizeof(info)];memcpy(before,&info,sizeof(info));
    if(strcmp(mode,"short-prefix")==0) { length=7U;wanted=UMI_STATUS_PARSE_ERROR; }
    if(strcmp(mode,"short-header")==0) { tensor_fixture_length(prefix,1U);wanted=UMI_STATUS_PARSE_ERROR; }
    if(strcmp(mode,"long-header")==0) { tensor_fixture_length(prefix,UMI_SAFETENSORS_MAX_HEADER_BYTES+1U);wanted=UMI_STATUS_CAPACITY_EXCEEDED; }
    if(strcmp(mode,"missing-header")==0) { --length;wanted=UMI_STATUS_PARSE_ERROR; }
    if(strcmp(mode,"file-bound")==0) { file_bytes=UMI_SAFETENSORS_MAX_FILE_BYTES+1U;wanted=UMI_STATUS_CAPACITY_EXCEEDED; }
    if(strcmp(mode,"prefix-exceeds-file")==0) { file_bytes=length-1U;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    if(strcmp(mode,"null-prefix")==0 || strcmp(mode,"null-output")==0) wanted=UMI_STATUS_INVALID_ARGUMENT;
    UmiCancellationToken *cancel=NULL;
    CHECK(umi_cancellation_token_create(&cancel)==UMI_STATUS_OK);
    if(strcmp(mode,"cancel")==0) { umi_cancellation_token_request(cancel);wanted=UMI_STATUS_CANCELLED; }
    UmiStatus status=UmiSafetensorsHeaderInspect(strcmp(mode,"null-prefix")==0?NULL:prefix,
        length,file_bytes,cancel,strcmp(mode,"null-output")==0?NULL:&info);
    CHECK(status==wanted);
    if(status!=UMI_STATUS_OK) CHECK(memcmp(&info,before,sizeof(info))==0);
    else {
        CHECK(info.header_bytes==strlen(json) && info.data_bytes==data);
        CHECK(info.tensors==tensors && info.metadata_entries==metadata);
        CHECK(info.maximum_rank==rank && info.elements==elements);
    }
    free(prefix);umi_cancellation_token_destroy(cancel);return 0;
}
static int exercise(const char *mode)
{
    for(size_t i=0U;i<sizeof(vectors)/sizeof(vectors[0]);++i)
        if(strcmp(mode,vectors[i].mode)==0) {
            const TensorVector *v=&vectors[i];return inspect(v->json,v->data,v->status,
                v->tensors,v->metadata,v->rank,v->elements,mode);
        }
    const char *special[]={"short-prefix","short-header","long-header","missing-header","file-bound",
        "prefix-exceeds-file","null-prefix","null-output","cancel","payload-ignored"};
    for(size_t i=0U;i<sizeof(special)/sizeof(special[0]);++i)
        if(strcmp(mode,special[i])==0) return inspect(tensor_fixture_json,24U,UMI_STATUS_OK,1U,1U,2U,6U,mode);
    if(strcmp(mode,"rank-bound")==0) {
        char json[512];strcpy(json,"{\"x\":{\"dtype\":\"U8\",\"shape\":[");
        for(size_t i=0U;i<65U;++i) strcat(json,i?",1":"1");
        strcat(json,"],\"data_offsets\":[0,1]}}");
        return inspect(json,1U,UMI_STATUS_CAPACITY_EXCEEDED,0U,0U,0U,0U,mode);
    }
    if(strcmp(mode,"maximum-name")==0 || strcmp(mode,"name-bound")==0) {
        char *json=malloc(4300U);CHECK(json!=NULL);json[0]='{';json[1]='"';
        size_t count=strcmp(mode,"maximum-name")==0?4096U:4097U;
        memset(json+2,'a',count);strcpy(json+2U+count,"\":{\"dtype\":\"U8\",\"shape\":[0],\"data_offsets\":[0,0]}}");
        int result=inspect(json,0U,count==4096U?UMI_STATUS_OK:UMI_STATUS_CAPACITY_EXCEEDED,1U,0U,1U,0U,mode);
        free(json);return result;
    }
    if(strcmp(mode,"dtypes")==0) {
        const char *names[]={"BOOL","U8","I8","U16","I16","U32","I32","U64","I64","F16","BF16","F32","F64","C64","F8_E5M2","F8_E4M3","F8_E8M0","F8_E4M3FNUZ","F8_E5M2FNUZ"};
        uint64_t widths[]={1U,1U,1U,2U,2U,4U,4U,8U,8U,2U,2U,4U,8U,8U,1U,1U,1U,1U,1U};
        for(size_t i=0U;i<sizeof(names)/sizeof(names[0]);++i) {
            char json[200];snprintf(json,sizeof(json),"{\"x\":{\"dtype\":\"%s\",\"shape\":[1],\"data_offsets\":[0,%u]}}",names[i],(unsigned)widths[i]);
            CHECK(inspect(json,widths[i],UMI_STATUS_OK,1U,0U,1U,1U,mode)==0);
        }
        return 0;
    }
    return 2;
}
int main(int argc,char **argv) { return argc==2?exercise(argv[1]):2; }
