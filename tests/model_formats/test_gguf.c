/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/model_formats/test_gguf.c
 *
 * PURPOSE:
 *   Check GGUF role identity, malformed metadata, bounded arrays and unchanged output using synthetic byte fixtures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "gguf_fixture.h"
#define MODE(name) (strcmp(mode,(name))==0)
static int exercise(const char *mode)
{
    GgufFixture *f=malloc(sizeof(*f));GGUF_CHECK(f!=NULL);fixture_model(f);
    UmiStatus wanted=UMI_STATUS_OK;
    UmiGgufRole role=UMI_GGUF_ROLE_MODEL_CANDIDATE;
    if(MODE("controlvector") || MODE("projector")) {
        fixture_start(f);fixture_text(f,"general.architecture",MODE("controlvector")?"controlvector":"clip");
        role=UMI_GGUF_ROLE_AUXILIARY;
    } else if(MODE("adapter")) {fixture_text(f,"general.type","adapter");role=UMI_GGUF_ROLE_AUXILIARY;}
    else if(MODE("adapter-key")) {fixture_text(f,"adapter.type","lora");role=UMI_GGUF_ROLE_AUXILIARY;}
    else if(MODE("declared-model")) fixture_text(f,"general.type","model");
    else if(MODE("unknown-type")) {fixture_text(f,"general.type","future-type");role=UMI_GGUF_ROLE_UNKNOWN;}
    else if(MODE("no-architecture")) {fixture_start(f);fixture_text(f,"general.name","sample");role=UMI_GGUF_ROLE_UNKNOWN;}
    else if(MODE("no-metadata")) {fixture_start(f);role=UMI_GGUF_ROLE_UNKNOWN;}
    else if(MODE("vocabulary")) {fixture_number_at(f->bytes+8U,0U,8U);role=UMI_GGUF_ROLE_VOCABULARY_ONLY;}
    else if(MODE("version-two")) f->bytes[4]=2U;
    else if(MODE("version-one") || MODE("big-endian")) {fixture_number_at(f->bytes+4U,MODE("version-one")?1U:UINT64_C(0x03000000),4U);wanted=UMI_STATUS_NOT_IMPLEMENTED;}
    else if(MODE("magic")) {f->bytes[0]='X';wanted=UMI_STATUS_PARSE_ERROR;}
    else if(MODE("duplicate-architecture")) {fixture_text(f,"general.architecture","qwen2");wanted=UMI_STATUS_PARSE_ERROR;}
    else if(MODE("duplicate-type")) {fixture_text(f,"general.type","model");fixture_text(f,"general.type","adapter");wanted=UMI_STATUS_PARSE_ERROR;}
    else if(MODE("duplicate-adapter")) {fixture_text(f,"adapter.type","lora");fixture_text(f,"adapter.type","lora");wanted=UMI_STATUS_PARSE_ERROR;}
    else if(MODE("nonstring-role")) {fixture_key(f,"general.type",4U);fixture_number(f,1U,4U);wanted=UMI_STATUS_PARSE_ERROR;}
    else if(MODE("empty-role")) {fixture_text(f,"general.type","");wanted=UMI_STATUS_PARSE_ERROR;}
    else if(MODE("long-role") || MODE("maximum-role")) {
        char value[65];memset(value,'a',sizeof(value));value[MODE("long-role")?64U:63U]='\0';
        fixture_text(f,"general.type",value);wanted=MODE("long-role")?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK;role=UMI_GGUF_ROLE_UNKNOWN;
    } else if(MODE("nul-role")) {fixture_text(f,"general.type","model");f->bytes[f->size-2U]=0U;wanted=UMI_STATUS_PARSE_ERROR;}
    else if(MODE("nonascii-role")) {fixture_text(f,"general.type","model");f->bytes[f->size-2U]=0xffU;wanted=UMI_STATUS_PARSE_ERROR;}
    else if(MODE("empty-key")) {fixture_key(f,"",0U);fixture_number(f,0U,1U);wanted=UMI_STATUS_PARSE_ERROR;}
    else if(MODE("key-bound") || MODE("maximum-key")) {
        char key[513];memset(key,'a',sizeof(key));key[MODE("key-bound")?512U:511U]='\0';
        fixture_key(f,key,0U);fixture_number(f,0U,1U);if(MODE("key-bound")) wanted=UMI_STATUS_CAPACITY_EXCEEDED;
    } else if(MODE("nul-key")) {fixture_key(f,"other",0U);f->bytes[f->size-5U]=0U;fixture_number(f,0U,1U);wanted=UMI_STATUS_PARSE_ERROR;}
    else if(MODE("unknown-value")) {fixture_key(f,"other",13U);wanted=UMI_STATUS_PARSE_ERROR;}
    else if(MODE("bool-invalid")) {fixture_key(f,"other",7U);fixture_number(f,2U,1U);wanted=UMI_STATUS_PARSE_ERROR;}
    else if(MODE("scalar-types")) {
        const unsigned widths[]={1U,1U,2U,2U,4U,4U,4U,1U,0U,0U,8U,8U,8U};
        for(unsigned i=0U;i<13U;++i) if(widths[i]!=0U) {char key[24];snprintf(key,sizeof(key),"scalar.%u",i);fixture_key(f,key,i);fixture_number(f,i==7U?1U:0U,widths[i]);}
    } else if(MODE("string-array")) {
        fixture_key(f,"tokenizer.tokens",9U);fixture_number(f,8U,4U);fixture_number(f,3U,8U);
        fixture_string(f,"one");fixture_string(f,"");fixture_string(f,"three");
    } else if(MODE("numeric-array") || MODE("array-bool-invalid")) {
        fixture_key(f,"values",9U);fixture_number(f,MODE("numeric-array")?4U:7U,4U);fixture_number(f,2U,8U);
        fixture_number(f,1U,MODE("numeric-array")?4U:1U);fixture_number(f,2U,MODE("numeric-array")?4U:1U);
        if(MODE("array-bool-invalid"))wanted=UMI_STATUS_PARSE_ERROR;
    } else if(MODE("empty-array") || MODE("array-value-bound") || MODE("array-type")) {
        fixture_key(f,"values",9U);fixture_number(f,MODE("array-type")?99U:0U,4U);
        fixture_number(f,MODE("array-value-bound")?UINT64_MAX:0U,8U);
        if(MODE("array-value-bound"))wanted=UMI_STATUS_CAPACITY_EXCEEDED;
        if(MODE("array-type"))wanted=UMI_STATUS_PARSE_ERROR;
    } else if(MODE("nested-arrays") || MODE("array-depth-bound")) {
        fixture_key(f,"values",9U);unsigned depth=MODE("nested-arrays")?8U:9U;
        for(unsigned i=0U;i<depth;++i) {fixture_number(f,i+1U==depth?7U:9U,4U);fixture_number(f,1U,8U);}
        fixture_number(f,1U,1U);if(MODE("array-depth-bound"))wanted=UMI_STATUS_CAPACITY_EXCEEDED;
    } else if(MODE("entry-bound")) {fixture_number_at(f->bytes+16U,UMI_GGUF_MAX_METADATA_ENTRIES+1U,8U);wanted=UMI_STATUS_CAPACITY_EXCEEDED;}
    else if(MODE("string-byte-bound")) {fixture_key(f,"large",8U);fixture_number(f,UMI_GGUF_MAX_METADATA_BYTES,8U);wanted=UMI_STATUS_CAPACITY_EXCEEDED;}
    else if(MODE("raw-unselected-string")) {fixture_text(f,"other","raw");f->bytes[f->size-1U]=0xffU;}
    else if(MODE("trailing-payload")) {unsigned char payload[80];memset(payload,0xff,sizeof(payload));fixture_bytes(f,payload,sizeof(payload));}
    else if(!MODE("valid") && !MODE("truncation") && !MODE("null-prefix") && !MODE("null-output") && !MODE("cancel") && !MODE("copied-output")) {free(f);return 2;}
    UmiGgufMetadata output;memset(&output,0xa5,sizeof(output));
    unsigned char before[sizeof(output)];memcpy(before,&output,sizeof(output));
    UmiCancellationToken *cancel=NULL;
    if(MODE("cancel")) {GGUF_CHECK(umi_cancellation_token_create(&cancel)==UMI_STATUS_OK);umi_cancellation_token_request(cancel);wanted=UMI_STATUS_CANCELLED;}
    if(MODE("null-prefix") || MODE("null-output")) wanted=UMI_STATUS_INVALID_ARGUMENT;
    if(MODE("truncation")) {
        for(size_t i=0U;i<f->size;++i) {
            GGUF_CHECK(UmiGgufMetadataInspect(f->bytes,i,NULL,&output)==UMI_STATUS_PARSE_ERROR);
            GGUF_CHECK(memcmp(before,&output,sizeof(output))==0);
        }
    }
    GGUF_CHECK(UmiGgufMetadataInspect(MODE("null-prefix")?NULL:f->bytes,f->size,cancel,MODE("null-output")?NULL:&output)==wanted);
    if(wanted==UMI_STATUS_OK) {
        GGUF_CHECK(output.role==role && output.entries==f->entries && output.metadata_bytes<=f->size);
        GGUF_CHECK(output.version==(MODE("version-two")?2U:3U));
        if(MODE("copied-output")) {memset(f->bytes,0,f->size);GGUF_CHECK(strcmp(output.architecture,"llama")==0);}
    } else GGUF_CHECK(memcmp(before,&output,sizeof(output))==0);
    umi_cancellation_token_destroy(cancel);free(f);return 0;
}
int main(int argc,char **argv) { return argc==2?exercise(argv[1]):2; }
