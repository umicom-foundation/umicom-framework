/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/provider_connections/test_document_file_faults.c
 *
 * PURPOSE:
 *   Inject file-read and cancellation outcomes to verify atomic settings publication and temporary-data clearing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/provider_connections/document_file.h"
#include "umicom/platform/input_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static const char golden[]="umicom-provider-connection\n1\n7\nconnection\n2\n30000\n1\n636f6d66792d6578616d706c65\n636f6d66797569\n436f6d66795549206c6f63616c\n687474703a2f2f3132372e302e302e313a38313838\n\n\n";
static const char *mode;
static UmiCancellationToken *token;
static size_t allocated_length;
static unsigned reads,frees;
static int cleared=1,read_contract=1;
/* Only this target redirects the production boundary's two platform calls.
 * Canonical import and cancellation remain real Framework implementations. */
UmiStatus FileFixtureRead(const char *path,size_t maximum,unsigned char **out,size_t *length)
{
    ++reads;*out=NULL;*length=0U;
    if(strcmp(path,"C:/fixture/settings.umiconnection")!=0 || maximum!=UMI_PROVIDER_CONNECTION_DOCUMENT_MAX_BYTES-1U)read_contract=0;
    if(strcmp(mode,"not-found")==0)return UMI_STATUS_NOT_FOUND;
    if(strcmp(mode,"permission")==0)return UMI_STATUS_PERMISSION_DENIED;
    if(strcmp(mode,"io-failure")==0)return UMI_STATUS_IO_ERROR;
    if(strcmp(mode,"allocation-failure")==0)return UMI_STATUS_OUT_OF_MEMORY;
    if(strcmp(mode,"capacity")==0)return UMI_STATUS_CAPACITY_EXCEEDED;
    const char *text=golden;
    if(strcmp(mode,"malformed")==0 || strcmp(mode,"parse-before-cancel")==0)text="broken\n";
    if(strcmp(mode,"unknown-version")==0)text="umicom-provider-connection\n9\n";
    if(strcmp(mode,"empty")==0)text="";
    allocated_length=strlen(text)+1U;*out=malloc(allocated_length);
    if(*out==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(*out,text,allocated_length);*length=allocated_length-1U;
    if(strcmp(mode,"embedded-nul")==0)(*out)[1]=0U;
    if(strcmp(mode,"cancel-after-read")==0)umi_cancellation_token_request(token);
    return UMI_STATUS_OK;
}
void FileFixtureFree(void *bytes)
{
    ++frees;for(size_t i=0U;i<allocated_length;++i)if(((unsigned char *)bytes)[i]!=0U)cleared=0;
    free(bytes);
    if(strcmp(mode,"cancel-at-release")==0 || strcmp(mode,"parse-before-cancel")==0)umi_cancellation_token_request(token);
}
int main(int argc,char **argv)
{
    CHECK(argc==2);mode=argv[1];CHECK(umi_cancellation_token_create(&token)==UMI_STATUS_OK);
    UmiProviderConnection out;unsigned char before[sizeof out];memset(&out,0xa5,sizeof out);memcpy(before,&out,sizeof out);
    uint64_t revision=99U;UmiStatus expected=UMI_STATUS_OK;int no_read=0;
    if(strcmp(mode,"not-found")==0)expected=UMI_STATUS_NOT_FOUND;
    else if(strcmp(mode,"permission")==0)expected=UMI_STATUS_PERMISSION_DENIED;
    else if(strcmp(mode,"io-failure")==0)expected=UMI_STATUS_IO_ERROR;
    else if(strcmp(mode,"allocation-failure")==0)expected=UMI_STATUS_OUT_OF_MEMORY;
    else if(strcmp(mode,"capacity")==0)expected=UMI_STATUS_CAPACITY_EXCEEDED;
    else if(strcmp(mode,"malformed")==0 || strcmp(mode,"parse-before-cancel")==0)expected=UMI_STATUS_PARSE_ERROR;
    else if(strcmp(mode,"embedded-nul")==0)expected=UMI_STATUS_PARSE_ERROR;
    else if(strcmp(mode,"unknown-version")==0)expected=UMI_STATUS_NOT_IMPLEMENTED;
/* An empty allocated file is a malformed document, not a null argument. Retain the initial expectation for review; match the existing canonical decoder. The former implementation is retained for owner review. */
#if 0
    else if(strcmp(mode,"empty")==0)expected=UMI_STATUS_INVALID_ARGUMENT;
#endif
    else if(strcmp(mode,"empty")==0)expected=UMI_STATUS_PARSE_ERROR;
    else if(strcmp(mode,"cancel-before")==0){expected=UMI_STATUS_CANCELLED;no_read=1;umi_cancellation_token_request(token);}
    else if(strcmp(mode,"cancel-after-read")==0 || strcmp(mode,"cancel-at-release")==0)expected=UMI_STATUS_CANCELLED;
    else if(strcmp(mode,"null-path")==0 || strcmp(mode,"null-connection")==0 || strcmp(mode,"null-revision")==0){expected=UMI_STATUS_INVALID_ARGUMENT;no_read=1;}
    else CHECK(strcmp(mode,"success")==0);
    UmiStatus status=UmiProviderConnectionDocumentFileRead(strcmp(mode,"null-path")==0?NULL:"C:/fixture/settings.umiconnection",token,
        strcmp(mode,"null-connection")==0?NULL:&out,strcmp(mode,"null-revision")==0?NULL:&revision);
    CHECK(status==expected && reads==(no_read?0U:1U) && read_contract && cleared);
    CHECK(frees==(allocated_length?1U:0U));
    if(status==UMI_STATUS_OK){CHECK(revision==7U && strcmp(out.id,"comfy-example")==0 && strcmp(out.provider_id,"comfyui")==0);}
    else CHECK(revision==99U && memcmp(before,&out,sizeof out)==0);
    umi_cancellation_token_destroy(token);return 0;
}
