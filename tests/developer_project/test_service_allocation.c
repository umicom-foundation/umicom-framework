/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_service_allocation.c
 *
 * PURPOSE:
 *   Verify generation workspace refusal, diagnostic freshness, release and dry-run retry through the real service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned owned,released,attempts;
static int refuse;
static void *ServiceAllocate(size_t count,size_t size)
{
    ++attempts;
    if(refuse)return NULL;
    void *p=calloc(count,size);if(p!=NULL)++owned;return p;
}
static void ServiceRelease(void *p){if(p!=NULL)++released;free(p);}
#define calloc ServiceAllocate
#define free ServiceRelease
#include "../../src/developer_project/service.c"
#undef free
#undef calloc
#define CHECK(c) do{if(!(c)){fprintf(stderr,"%d: %s\n",__LINE__,#c);goto failed;}}while(0)
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    UmiDeveloperProjectService *service=NULL;
    UmiDeveloperProjectModel *model=malloc(sizeof *model),*before=malloc(sizeof *before);
    UmiDeveloperProjectGenerationRequest request;
    UmiDeveloperProjectGeneratorReport report;
    CHECK(model!=NULL&&before!=NULL);
    CHECK(umi_developer_project_service_create(&service)==UMI_STATUS_OK);
    unsigned base_owned=owned,base_released=released;
    umi_developer_project_generation_request_init(&request);
    strcpy(request.template_id,"developer.template.c23-console");
    strcpy(request.application_name,"Umicom Example");
    strcpy(request.application_id,"org.umicom.example");
    strcpy(request.repository_name,"umicom-example");
    strcpy(request.target_name,"umicom_example");
    CHECK(strlen(argv[1])<sizeof request.project_root);
    strcpy(request.project_root,argv[1]);request.dry_run=1;
    memset(model,0xa5,sizeof *model);memcpy(before,model,sizeof *before);
    memset(&report,0xa5,sizeof report);refuse=1;
    /* Framework declares OUT_OF_MEMORY. Preserve the earlier spelling below
     * for integration review; the active path keeps the same failure behavior. */
#if 0
    CHECK(umi_developer_project_service_generate(service,&request,&report,model)==UMI_STATUS_NO_MEMORY);
#else
    CHECK(umi_developer_project_service_generate(service,&request,&report,model)==UMI_STATUS_OUT_OF_MEMORY);
#endif
    CHECK(memcmp(model,before,sizeof *model)==0);
    CHECK(report.files_created==0U&&report.files_skipped==0U&&report.bytes_written==0U&&report.directories_created==0U);
    CHECK(owned==base_owned&&released==base_released);
    refuse=0;
    strcpy(request.template_id,"developer.template.missing");
    CHECK(umi_developer_project_service_generate(service,&request,&report,model)==UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(model,before,sizeof *model)==0&&owned==base_owned+1U&&released==base_released+1U);
    strcpy(request.template_id,"developer.template.c23-console");
    CHECK(umi_developer_project_service_generate(service,&request,&report,model)==UMI_STATUS_OK);
    CHECK(report.dry_run&&report.files_created>0U);
    CHECK(!strcmp(model->project_id,"org.umicom.example"));
    CHECK(owned==base_owned+2U&&released==base_released+2U);
    CHECK(umi_developer_project_model_registry_count(service->models)==0U);
    memcpy(before,model,sizeof *before);service->revision=UINT64_MAX;request.dry_run=0;
    unsigned before_attempts=attempts;
    CHECK(umi_developer_project_service_generate(service,&request,&report,model)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(attempts==before_attempts&&report.files_created==0U&&memcmp(model,before,sizeof *model)==0);
    umi_developer_project_service_destroy(service);free(model);free(before);
    return owned==released?0:1;
failed:
    umi_developer_project_service_destroy(service);free(model);free(before);return 1;
}
