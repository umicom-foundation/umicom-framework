/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/model_formats/test_safetensors_file.c
 *
 * PURPOSE:
 *   Verify checked native file identity, expected metadata and preserved inspection results using isolated synthetic fixtures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "safetensors_fixture.h"
#include "umicom/model_formats/safetensors_file.h"

static void write_fixture(const char *path,const unsigned char *bytes,size_t size)
{
    UmiOutputFile *file=NULL;CHECK(UmiOutputFileCreate(path,&file)==UMI_STATUS_OK);
    CHECK(UmiOutputFileWrite(file,bytes,size)==UMI_STATUS_OK);
    CHECK(UmiOutputFileClose(file)==UMI_STATUS_OK);UmiOutputFileDestroy(file);
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    const char *modes[]={"valid","expected","borrowed-expected","wrong-digest","expected-smaller",
        "expected-larger","invalid-expected","unicode","missing","directory","relative",
        "null-path","null-output","cancel","truncated","oversized-header","invalid-layout","payload-change","link"};
    bool known=false;for(size_t i=0U;i<sizeof(modes)/sizeof(modes[0]);++i) if(strcmp(mode,modes[i])==0) known=true;
    CHECK(known);
    char root[UMI_PATH_CAPACITY],path[UMI_PATH_CAPACITY];FixtureDirectory(root);
    FixturePath(path,root,strcmp(mode,"unicode")==0?"caf\xc3\xa9.safetensors":"model.safetensors");
    UmiSafetensorsFileInspection info;memset(&info,0xa5,sizeof(info));
    unsigned char before[sizeof(info)];memcpy(before,&info,sizeof(info));
    UmiStatus wanted=UMI_STATUS_OK;
    const char *selected_path=path;
    if(strcmp(mode,"missing")==0) wanted=UMI_STATUS_NOT_FOUND;
    else if(strcmp(mode,"relative")==0) { selected_path="relative.safetensors";wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"null-path")==0) { selected_path=NULL;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"null-output")==0) wanted=UMI_STATUS_INVALID_ARGUMENT;
    else if(strcmp(mode,"cancel")==0) wanted=UMI_STATUS_CANCELLED;
    else if(strcmp(mode,"directory")==0) {
        CHECK(UmiSafetensorsFileInspect(root,NULL,NULL,&info)!=UMI_STATUS_OK);
        CHECK(memcmp(&info,before,sizeof(info))==0);return 0;
    }
    UmiCancellationToken *cancel=NULL;CHECK(umi_cancellation_token_create(&cancel)==UMI_STATUS_OK);
    if(strcmp(mode,"cancel")==0) umi_cancellation_token_request(cancel);
    if(wanted!=UMI_STATUS_OK) {
        CHECK(UmiSafetensorsFileInspect(selected_path,NULL,cancel,
            strcmp(mode,"null-output")==0?NULL:&info)==wanted);
        CHECK(memcmp(&info,before,sizeof(info))==0);umi_cancellation_token_destroy(cancel);return 0;
    }
    const char *json=strcmp(mode,"invalid-layout")==0?
        "{\"x\":{\"dtype\":\"F32\",\"shape\":[1],\"data_offsets\":[0,4]}}":tensor_fixture_json;
    size_t size;unsigned char *bytes=tensor_fixture_prefix(json,24U,&size);CHECK(bytes!=NULL);
    size_t written=size;
    if(strcmp(mode,"truncated")==0) { written=7U;wanted=UMI_STATUS_PARSE_ERROR; }
    if(strcmp(mode,"oversized-header")==0) { tensor_fixture_length(bytes,UMI_SAFETENSORS_MAX_HEADER_BYTES+1U);wanted=UMI_STATUS_CAPACITY_EXCEEDED; }
    if(strcmp(mode,"invalid-layout")==0) wanted=UMI_STATUS_PARSE_ERROR;
    write_fixture(path,bytes,written);
    if(strcmp(mode,"link")==0) {
        char alias[UMI_PATH_CAPACITY];FixturePath(alias,root,"linked.safetensors");
#ifdef _WIN32
        if(!FixtureSymbolicLink(alias,path,0)) {
            fprintf(stderr,"SKIP: Windows symbolic-link fixture capability unavailable.\n");
            free(bytes);umi_cancellation_token_destroy(cancel);return 77;
        }
#else
        CHECK(symlink(path,alias)==0);
#endif
        CHECK(UmiSafetensorsFileInspect(alias,NULL,cancel,&info)!=UMI_STATUS_OK);
        CHECK(memcmp(&info,before,sizeof(info))==0);
        free(bytes);umi_cancellation_token_destroy(cancel);return 0;
    }
    CHECK(UmiSafetensorsFileInspect(path,NULL,cancel,&info)==wanted);
    if(wanted!=UMI_STATUS_OK) {
        CHECK(memcmp(&info,before,sizeof(info))==0);
        free(bytes);umi_cancellation_token_destroy(cancel);return 0;
    }
    CHECK(info.file.bytes==125U && info.layout.tensors==1U && info.layout.elements==6U && info.layout.data_bytes==24U);
    char digest[UMI_SHA256_HEX_CAPACITY];UmiSha256Hex(info.file.sha256,digest);
    /* Independent SHA-256 of the exact synthetic prefix and 24 bytes of 0xcd. */
    CHECK(strcmp(digest,"ae7f60d3e0de6f1dd84a3aebebc19d38af6720bc42e8e8344f8ca2dc0cfbee47")==0);
    UmiSafetensorsFileIdentity expected=info.file;
    memcpy(before,&info,sizeof(info));
    if(strcmp(mode,"wrong-digest")==0) { expected.sha256[0]^=1U;wanted=UMI_STATUS_INVALID_STATE; }
    if(strcmp(mode,"expected-smaller")==0) { --expected.bytes;wanted=UMI_STATUS_CAPACITY_EXCEEDED; }
    if(strcmp(mode,"expected-larger")==0) { ++expected.bytes;wanted=UMI_STATUS_INVALID_STATE; }
    if(strcmp(mode,"invalid-expected")==0) { expected.bytes=9U;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    char changed_path[UMI_PATH_CAPACITY];
    if(strcmp(mode,"payload-change")==0) {
        bytes[size-1U]^=1U;FixturePath(changed_path,root,"changed.safetensors");
        write_fixture(changed_path,bytes,size);selected_path=changed_path;wanted=UMI_STATUS_INVALID_STATE;
    }
    CHECK(UmiSafetensorsFileInspect(selected_path,
        strcmp(mode,"borrowed-expected")==0?&info.file:&expected,cancel,&info)==wanted);
    if(wanted!=UMI_STATUS_OK) CHECK(memcmp(&info,before,sizeof(info))==0);
    else CHECK(info.file.bytes==expected.bytes && memcmp(info.file.sha256,expected.sha256,UMI_SHA256_BYTES)==0);
    free(bytes);umi_cancellation_token_destroy(cancel);return 0;
}
