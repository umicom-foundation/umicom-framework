/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/task_supervision/test_background_native.c
 * PURPOSE: Exercise background discovery against a real 10,000-test CTest tree.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"
#include "umicom/test_platform/ctest.h"
int main(int argc,char **argv)
{
    CHECK(argc==2);
    UmiTestPlatformItemRegistry *items=NULL;
    UmiTestPlatformSuiteRegistry *suites=NULL;
    UmiTestPlatformDiscoveryRegistry *discoveries=NULL;
    CHECK(umi_test_platform_item_registry_create(&items)==UMI_STATUS_OK);
    CHECK(umi_test_platform_suite_registry_create(&suites)==UMI_STATUS_OK);
    CHECK(umi_test_platform_discovery_registry_create(&discoveries)==UMI_STATUS_OK);
    UmiTestPlatformCtestImportOptions options={0};
    snprintf(options.project_id,sizeof(options.project_id),"notes");
    snprintf(options.suite_id,sizeof(options.suite_id),"notes.ctest");
    snprintf(options.configuration,sizeof(options.configuration),"Debug");
    CHECK(strlen(argv[1])<sizeof(options.build_directory));
    snprintf(options.build_directory,sizeof(options.build_directory),"%s",argv[1]);
    UmiTaskQueue *queue=TestQueue(1U,4U);UmiCtestDiscoveryJob *job=NULL;
    CHECK(UmiCtestDiscoveryJobStart(queue,&options,NULL,items,suites,discoveries,7U,&job)==UMI_STATUS_OK);
    CHECK(umi_test_platform_item_registry_count(items)==0U);
    UmiTestPlatformCtestImportSummary summary={0};int published=0;UmiStatus status;
    char diagnostics[4096];size_t polls=0;uint64_t start=TestNowMs();
    do {
        status=UmiCtestDiscoveryJobPoll(job,7U,&summary,&published,diagnostics,sizeof(diagnostics));
        ++polls;
        if(status==UMI_STATUS_BUSY) {
            CHECK(umi_test_platform_item_registry_count(items)==0U);
            umi_thread_sleep_ms(1U);
        }
        CHECK(TestNowMs()-start<120000U);
    } while(status==UMI_STATUS_BUSY);
    if(status!=UMI_STATUS_OK) fprintf(stderr,"discovery=%d %s\n",status,diagnostics);
    CHECK(status==UMI_STATUS_OK && published);
    CHECK(summary.discovered_count==10000U && summary.disabled_count==100U);
    CHECK(umi_test_platform_item_registry_count(items)==10001U);
    CHECK(umi_test_platform_suite_registry_count(suites)==1U);
    CHECK(umi_test_platform_discovery_registry_count(discoveries)==1U);
    printf("PASS real background discovery: %zu tests; %zu disabled; %zu polls; %llu ms\n",
        summary.discovered_count,summary.disabled_count,polls,(unsigned long long)(TestNowMs()-start));
    CHECK(UmiCtestDiscoveryJobDestroy(job)==UMI_STATUS_OK);TestFinish(queue);
    umi_test_platform_item_registry_destroy(items);umi_test_platform_suite_registry_destroy(suites);
    umi_test_platform_discovery_registry_destroy(discoveries);return 0;
}
