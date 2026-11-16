/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai/test_ollama_pull_file.c
 *
 * PURPOSE:
 *   Verify complete file observation publication and bounded metadata JSON without exposing provider text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/ollama_pull_file.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv)
{
    CHECK(argc==3);const char *mode=argv[1],*path=argv[2];
    UmiOllamaPullFileReport report,before;memset(&report,0xa5,sizeof report);memcpy(&before,&report,sizeof before);
    UmiStatus expected=UMI_STATUS_OK;UmiCancellationToken *cancel=NULL;
    if(strcmp(mode,"incomplete")==0 || strcmp(mode,"malformed")==0)expected=UMI_STATUS_PARSE_ERROR;
    if(strcmp(mode,"missing")==0)expected=UMI_STATUS_NOT_FOUND;
    if(strcmp(mode,"relative")==0){path="relative.ndjson";expected=UMI_STATUS_INVALID_ARGUMENT;}
    if(strcmp(mode,"cancel")==0){CHECK(umi_cancellation_token_create(&cancel)==UMI_STATUS_OK);umi_cancellation_token_request(cancel);expected=UMI_STATUS_CANCELLED;}
    if(strcmp(mode,"nulls")==0){CHECK(UmiOllamaPullFileInspect(NULL,NULL,&report)==UMI_STATUS_INVALID_ARGUMENT);CHECK(memcmp(&report,&before,sizeof report)==0);CHECK(UmiOllamaPullFileInspect(path,NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);return 0;}
    UmiStatus status=UmiOllamaPullFileInspect(path,cancel,&report);umi_cancellation_token_destroy(cancel);
    CHECK(status==expected);
    if(status!=UMI_STATUS_OK){CHECK(memcmp(&report,&before,sizeof report)==0);return 0;}
    CHECK(report.events==2U && report.bytes!=0U);
    CHECK(report.provider_outcome==(strcmp(mode,"error")==0 || strcmp(mode,"json-error")==0?UMI_OLLAMA_PULL_ERROR:UMI_OLLAMA_PULL_SUCCESS));
    char output[UMI_OLLAMA_PULL_FILE_JSON_CAPACITY],original[sizeof output];size_t length=0U;
    CHECK(UmiOllamaPullFileJson(&report,output,sizeof output,&length)==UMI_STATUS_OK);CHECK(length==strlen(output));
    CHECK(strstr(output,"offline-pull-progress-file")!=NULL);CHECK(strstr(output,"PRIVATE_SENTINEL")==NULL);
    CHECK(strstr(output,"\"model_installation_verified\":false")!=NULL);
    if(strcmp(mode,"json-exact")==0){size_t exact=length+1U;CHECK(UmiOllamaPullFileJson(&report,output,exact,&length)==UMI_STATUS_OK);}
    if(strcmp(mode,"json-short")==0 || strcmp(mode,"json-zero")==0){
        size_t capacity=strcmp(mode,"json-zero")==0?0U:length;memset(output,0x5a,sizeof output);memcpy(original,output,sizeof output);length=77U;
        CHECK(UmiOllamaPullFileJson(&report,output,capacity,&length)==UMI_STATUS_CAPACITY_EXCEEDED);CHECK(length==77U && memcmp(output,original,sizeof output)==0);
    }
    if(strcmp(mode,"json-null")==0){CHECK(UmiOllamaPullFileJson(NULL,output,sizeof output,&length)==UMI_STATUS_INVALID_ARGUMENT);CHECK(UmiOllamaPullFileJson(&report,NULL,sizeof output,&length)==UMI_STATUS_INVALID_ARGUMENT);CHECK(UmiOllamaPullFileJson(&report,output,sizeof output,NULL)==UMI_STATUS_INVALID_ARGUMENT);}
    if(strcmp(mode,"json-bounds")==0){
        for(unsigned i=0U;i<5U;++i){UmiOllamaPullFileReport bad=report;if(i==0U)bad.bytes=0U;if(i==1U)bad.bytes=UMI_OLLAMA_PULL_STREAM_BYTES+1U;if(i==2U)bad.events=0U;if(i==3U)bad.events=UMI_OLLAMA_PULL_MAX_EVENTS+1U;if(i==4U)bad.provider_outcome=UMI_OLLAMA_PULL_STATUS;
            memset(output,0x5a,sizeof output);memcpy(original,output,sizeof output);length=77U;CHECK(UmiOllamaPullFileJson(&bad,output,sizeof output,&length)==UMI_STATUS_INVALID_ARGUMENT);CHECK(length==77U && memcmp(output,original,sizeof output)==0);
        }
    }
    return 0;
}
