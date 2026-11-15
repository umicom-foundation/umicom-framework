/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_local_transport.c
 *
 * PURPOSE:
 *   Inject runtime initialization and capability boundaries while checking retained request ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "local_transport_internal.h"
#include "umicom/platform/threading.h"
#include <curl/curl.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static size_t initialized,cleaned;
static uint64_t owner=17U;
static bool fail_allocation,fail_init,missing_version;
static curl_version_info_data reported_version;
static void *Allocate(size_t count,size_t bytes){return fail_allocation?NULL:calloc(count,bytes);}
static uint64_t ThreadId(void){return owner;}
static CURLcode Initialize(long flags){++initialized;return flags!=CURL_GLOBAL_DEFAULT || fail_init?CURLE_FAILED_INIT:CURLE_OK;}
static void Cleanup(void){++cleaned;}
static curl_version_info_data *Version(CURLversion age){(void)age;return missing_version?NULL:&reported_version;}
/* The actual runtime implementation is compiled once with only environmental
 * boundaries intercepted. No network or real curl global state is touched. */
#define calloc Allocate
#define umi_thread_current_id ThreadId
#define curl_global_init Initialize
#define curl_global_cleanup Cleanup
#define curl_version_info Version
#include "../../src/ai_workspace/local_transport.c"
#undef calloc
#undef umi_thread_current_id
#undef curl_global_init
#undef curl_global_cleanup
#undef curl_version_info
#define CHECK(c) do{if(!(c)){fprintf(stderr,"%d: %s\n",__LINE__,#c);return 1;}}while(0)
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const char *mode=argv[1];reported_version.features=CURL_VERSION_THREADSAFE;
    UmiAiLocalTransport sentinel={0},*runtime=&sentinel;
    if(strcmp(mode,"arguments")==0){
        CHECK(UmiAiLocalTransportCreate(NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiLocalTransportDestroy(NULL)==UMI_STATUS_INVALID_ARGUMENT);
        size_t count=99U;CHECK(UmiAiLocalTransportRequestCount(NULL,&count)==UMI_STATUS_INVALID_ARGUMENT && count==99U);
        CHECK(AwLocalTransportRetain(NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(AwLocalTransportRelease(NULL)==UMI_STATUS_INVALID_ARGUMENT);
        runtime=NULL;CHECK(UmiAiLocalTransportDestroy(&runtime)==UMI_STATUS_OK);
        CHECK(initialized==0U && cleaned==0U);return 0;
    }
    UmiStatus expected=UMI_STATUS_OK;
    if(strcmp(mode,"allocation")==0){fail_allocation=true;expected=UMI_STATUS_OUT_OF_MEMORY;}
    else if(strcmp(mode,"init-failure")==0){fail_init=true;expected=UMI_STATUS_UNAVAILABLE;}
    else if(strcmp(mode,"missing-features")==0){reported_version.features=0;expected=UMI_STATUS_UNAVAILABLE;}
    else if(strcmp(mode,"missing-version")==0){missing_version=true;expected=UMI_STATUS_UNAVAILABLE;}
    else if(strcmp(mode,"lifetime")!=0 && strcmp(mode,"wrong-owner")!=0 &&
        strcmp(mode,"overflow")!=0 && strcmp(mode,"two-runtimes")!=0)return 2;
    CHECK(UmiAiLocalTransportCreate(&runtime)==expected);
    if(expected!=UMI_STATUS_OK){
        CHECK(runtime==&sentinel);
        CHECK(initialized==(fail_allocation?0U:1U));
        CHECK(cleaned==((!fail_allocation&&!fail_init)?1U:0U));return 0;
    }
    size_t count=99U;CHECK(UmiAiLocalTransportRequestCount(runtime,&count)==UMI_STATUS_OK && count==0U);
    CHECK(AwLocalTransportRelease(runtime)==UMI_STATUS_INVALID_STATE);
    CHECK(UmiAiLocalTransportRequestCount(runtime,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    if(strcmp(mode,"wrong-owner")==0){
        owner=18U;
        CHECK(AwLocalTransportRetain(runtime)==UMI_STATUS_INVALID_STATE);
        CHECK(AwLocalTransportRelease(runtime)==UMI_STATUS_INVALID_STATE);
        count=99U;CHECK(UmiAiLocalTransportRequestCount(runtime,&count)==UMI_STATUS_INVALID_STATE && count==99U);
        UmiAiLocalTransport *before=runtime;
        CHECK(UmiAiLocalTransportDestroy(&runtime)==UMI_STATUS_INVALID_STATE && runtime==before);
        owner=17U;
    }else if(strcmp(mode,"overflow")==0){
        runtime->requests=SIZE_MAX;
        CHECK(AwLocalTransportRetain(runtime)==UMI_STATUS_CAPACITY_EXCEEDED && runtime->requests==SIZE_MAX);
        runtime->requests=0U;
    }else if(strcmp(mode,"two-runtimes")==0){
        UmiAiLocalTransport *other=NULL;
        CHECK(UmiAiLocalTransportCreate(&other)==UMI_STATUS_OK && initialized==2U);
        CHECK(UmiAiLocalTransportDestroy(&other)==UMI_STATUS_OK && cleaned==1U);
    }else{
        CHECK(AwLocalTransportRetain(runtime)==UMI_STATUS_OK);
        CHECK(AwLocalTransportRetain(runtime)==UMI_STATUS_OK);
        CHECK(UmiAiLocalTransportRequestCount(runtime,&count)==UMI_STATUS_OK && count==2U);
        CHECK(UmiAiLocalTransportDestroy(&runtime)==UMI_STATUS_BUSY && cleaned==0U);
        CHECK(AwLocalTransportRelease(runtime)==UMI_STATUS_OK);
        CHECK(UmiAiLocalTransportDestroy(&runtime)==UMI_STATUS_BUSY);
        CHECK(AwLocalTransportRelease(runtime)==UMI_STATUS_OK);
    }
    CHECK(UmiAiLocalTransportDestroy(&runtime)==UMI_STATUS_OK && runtime==NULL);
    CHECK(cleaned==initialized);return 0;
}
