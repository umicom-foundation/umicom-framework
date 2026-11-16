/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/provider_connections/test_document_file.c
 *
 * PURPOSE:
 *   Check native regular-file admission and all-or-nothing provider settings observation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/provider_connections/document_file.h"
#include "umicom/platform/native_arguments.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int CheckFile(int argc,char **argv)
{
    CHECK(argc==3);const char *mode=argv[1];
    UmiProviderConnection connection;unsigned char before[sizeof connection];
    memset(&connection,0xa5,sizeof connection);memcpy(before,&connection,sizeof connection);
    uint64_t revision=99U;UmiStatus expected=UMI_STATUS_OK;
    UmiCancellationToken *cancel=NULL;
    if(strcmp(mode,"missing")==0)expected=UMI_STATUS_NOT_FOUND;
    else if(strcmp(mode,"directory")==0)expected=UMI_STATUS_IO_ERROR;
    else if(strcmp(mode,"relative")==0)expected=UMI_STATUS_INVALID_ARGUMENT;
    else if(strcmp(mode,"empty")==0 || strcmp(mode,"malformed")==0 ||
            strcmp(mode,"crlf")==0 || strcmp(mode,"embedded-nul")==0 ||
            strcmp(mode,"within-limit")==0)expected=UMI_STATUS_PARSE_ERROR;
    else if(strcmp(mode,"unknown-version")==0)expected=UMI_STATUS_NOT_IMPLEMENTED;
    else if(strcmp(mode,"over-limit")==0)expected=UMI_STATUS_CAPACITY_EXCEEDED;
    else if(strcmp(mode,"cancel-before")==0){
        expected=UMI_STATUS_CANCELLED;CHECK(umi_cancellation_token_create(&cancel)==UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);
    }else CHECK(strcmp(mode,"valid")==0 || strcmp(mode,"unicode")==0);
    UmiStatus status=UmiProviderConnectionDocumentFileRead(argv[2],cancel,&connection,&revision);
    umi_cancellation_token_destroy(cancel);CHECK(status==expected);
    if(status==UMI_STATUS_OK){
        CHECK(revision==7U && strcmp(connection.id,"comfy-example")==0);
        CHECK(strcmp(connection.endpoint,"http://127.0.0.1:8188")==0);
    }else CHECK(revision==99U && memcmp(before,&connection,sizeof connection)==0);
    return 0;
}
int main(int argc,char **argv)
{
    int result=1;UmiStatus status=UmiNativeArgumentsDispatch(argc,argv,CheckFile,&result);
    return status==UMI_STATUS_OK?result:1;
}
