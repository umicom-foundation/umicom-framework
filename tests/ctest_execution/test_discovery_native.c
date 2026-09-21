/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_execution/test_discovery_native.c
 * PURPOSE: Exercise complete and overflowing catalogues through the real CTest process.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/ctest_adapter.h"
#include <stdio.h>
#include <string.h>
#define REQUIRE(c) do { if (!(c)) { fprintf(stderr,"Failure %s:%d: %s\n",__FILE__,__LINE__,#c); return 1; } } while(0)
int main(int argc,char **argv)
{
    REQUIRE(argc==3);
    UmiTestSuite *suite=NULL; REQUIRE(umi_test_suite_create("notes","Notes",&suite)==UMI_STATUS_OK);
    UmiTestCase retained; umi_test_case_init(&retained,"retained","Retained");
    REQUIRE(umi_test_suite_add(suite,&retained)==UMI_STATUS_OK);
    size_t discovered=444U; char diagnostics[256];
    UmiStatus status=UmiCtestDiscoverConfigured(argv[1],NULL,suite,&discovered,diagnostics,sizeof diagnostics);
    if(strcmp(argv[2],"small")==0) {
        REQUIRE(status==UMI_STATUS_OK && discovered==2U && umi_test_suite_count(suite)==3U);
        REQUIRE(strcmp(umi_test_suite_at(suite,1U)->name,"notes.case.1")==0);
    } else if(strcmp(argv[2],"empty")==0) {
        REQUIRE(status==UMI_STATUS_OK && discovered==0U && umi_test_suite_count(suite)==1U);
    } else {
        REQUIRE(status!=UMI_STATUS_OK && discovered==0U && umi_test_suite_count(suite)==1U);
        if(strcmp(argv[2],"overflow")==0 || strcmp(argv[2],"capture")==0) REQUIRE(status==UMI_STATUS_CAPACITY_EXCEEDED);
        if(strcmp(argv[2],"disabled")==0) REQUIRE(status==UMI_STATUS_UNAVAILABLE);
        if(strcmp(argv[2],"unconfigured")==0) REQUIRE(status==UMI_STATUS_NOT_FOUND);
    }
    REQUIRE(memcmp(umi_test_suite_at(suite,0U),&retained,sizeof retained)==0);
    printf("native %s: status=%d discovered=%zu retained=%zu; %s\n",argv[2],(int)status,discovered,umi_test_suite_count(suite),diagnostics);
    umi_test_suite_destroy(suite); return 0;
}
