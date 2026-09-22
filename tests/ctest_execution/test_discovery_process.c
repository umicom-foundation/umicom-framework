/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_execution/test_discovery_process.c
 * PURPOSE: Inject discovery process failures without launching a child.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/ctest_adapter.h"
#include "umicom/platform/process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(c) do { if (!(c)) { fprintf(stderr,"Failure %s:%d: %s\n",__FILE__,__LINE__,#c); return 1; } } while(0)
static const char *scenario;
static unsigned calls;
static int argumentsValid = 1;
UmiStatus umi_process_execute(const UmiProcessRequest *request, UmiProcessResult *result)
{
    ++calls;
    memset(result,0,sizeof *result); result->launched=1;
    strcpy(result->output,"Test #1: notes.save\nTotal Tests: 1\n");
    argumentsValid = strcmp(request->program,"ctest")==0 && request->argument_count==5U &&
        strcmp(request->arguments[0],"--test-dir")==0 && strcmp(request->arguments[1],"build path & literal")==0 &&
        strcmp(request->arguments[2],"-N")==0 && strcmp(request->arguments[3],"-C")==0 &&
        strcmp(request->arguments[4],"Release")==0 && request->timeout_ms==1234U &&
        request->window_mode==UMI_PROCESS_WINDOW_HIDDEN;
    if(strcmp(scenario,"truncated")==0) result->output_truncated=1;
    else if(strcmp(scenario,"cancelled")==0) result->cancelled=1;
    else if(strcmp(scenario,"timed-out")==0) result->timed_out=1;
    else if(strcmp(scenario,"launch-missing")==0) result->launched=0;
    else if(strcmp(scenario,"exit-error")==0) result->exit_code=9;
    else if(strcmp(scenario,"process-error")==0) return UMI_STATUS_IO_ERROR;
    else if(strcmp(scenario,"unterminated")==0) memset(result->output,'x',sizeof result->output);
    else if(strcmp(scenario,"partial-listing")==0) strcpy(result->output,"Test #1: notes.save\n");
    else if(strcmp(scenario,"duplicate")==0) strcpy(result->output,"Test #1: retained\nTotal Tests: 1\n");
    return UMI_STATUS_OK;
}
/* The old symbol stays linkable for the cumulative execution-check library. */
UmiStatus umi_process_capture(const char *p,const char *const *a,size_t n,char *o,size_t c,int *e)
{ (void)p;(void)a;(void)n;(void)o;(void)c;(void)e; return UMI_STATUS_INTERNAL_ERROR; }
int main(int argc,char **argv)
{
    REQUIRE(argc==2); scenario=argv[1];
    UmiTestSuite *suite=NULL; REQUIRE(umi_test_suite_create("notes","Notes",&suite)==UMI_STATUS_OK);
    UmiTestCase retained; umi_test_case_init(&retained,"retained","Retained");
    REQUIRE(umi_test_suite_add(suite,&retained)==UMI_STATUS_OK);
    UmiCancellationToken *token=NULL; REQUIRE(umi_cancellation_token_create(&token)==UMI_STATUS_OK);
    UmiCtestDiscoveryOptions options={0}; options.configuration="Release"; options.timeout_ms=1234U; options.cancellation=token;
    if(strcmp(scenario,"cancel-before")==0) umi_cancellation_token_request(token);
    size_t discovered=777U; char diagnostics[256]; memset(diagnostics,0x4a,sizeof diagnostics);
    UmiStatus status=UmiCtestDiscoverConfigured("build path & literal",&options,suite,&discovered,diagnostics,sizeof diagnostics);
    if(strcmp(scenario,"success")==0) REQUIRE(status==UMI_STATUS_OK && discovered==1U && umi_test_suite_count(suite)==2U);
    else {
        REQUIRE(status!=UMI_STATUS_OK && discovered==0U && umi_test_suite_count(suite)==1U);
        if(strcmp(scenario,"truncated")==0) REQUIRE(status==UMI_STATUS_CAPACITY_EXCEEDED && strstr(diagnostics,"capture capacity")!=NULL);
        if(strcmp(scenario,"cancelled")==0 || strcmp(scenario,"cancel-before")==0) REQUIRE(status==UMI_STATUS_CANCELLED);
        if(strcmp(scenario,"timed-out")==0) REQUIRE(status==UMI_STATUS_TIMEOUT);
        if(strcmp(scenario,"duplicate")==0) REQUIRE(status==UMI_STATUS_ALREADY_EXISTS);
    }
    REQUIRE(memchr(diagnostics,'\0',sizeof diagnostics)!=NULL && diagnostics[0]!='\0');
    REQUIRE(memcmp(umi_test_suite_at(suite,0U),&retained,sizeof retained)==0);
    if(strcmp(scenario,"cancel-before")==0) REQUIRE(calls==0U);
    else REQUIRE(calls==1U && argumentsValid);
    umi_cancellation_token_destroy(token); umi_test_suite_destroy(suite); printf("discovery process %s passed\n",scenario); return 0;
}

/* Retain injected process outcomes while checking the executor's ownership. */
UmiStatus UmiProcessExecuteWithLifetime(const UmiProcessRequest *request,
    UmiProcessLifetime lifetime, UmiProcessResultObserver observer,
    UmiProcessOutputObserver rawObserver, void *context, UmiProcessResult *result)
{
    (void)context;
    if (lifetime != UMI_PROCESS_LIFETIME_TREE || observer != NULL || rawObserver != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return umi_process_execute(request, result);
}
