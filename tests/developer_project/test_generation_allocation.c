/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_generation_allocation.c
 *
 * PURPOSE:
 *   Exercise real generation candidate allocation refusal and release without filesystem work.
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
static unsigned allocated,released,attempts;
static int refuse;
static void *CandidateAllocate(size_t count,size_t size)
{
    ++attempts;
    if(refuse)return NULL;
    void *value=calloc(count,size);
    if(value!=NULL)++allocated;
    return value;
}
static void CandidateRelease(void *value)
{
    if(value!=NULL)++released;
    free(value);
}
/* Only the implementation's candidate storage is intercepted. Public API
 * behaviour, rendering and validation execute through the real source. */
#define calloc CandidateAllocate
#define free CandidateRelease
#include "../../src/developer_project/generation_plan.c"
#undef free
#undef calloc
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%d: %s\n",__LINE__,#c); goto failed; } } while(0)
int main(void)
{
    UmiDeveloperProjectGenerationPlan *output=malloc(sizeof *output),*before=malloc(sizeof *before);
    UmiDeveloperProjectVariableSet variables;
    UmiDeveloperProjectTemplateFile files[]={{"main.c","int main(void){return 0;}\n",0,0}};
    UmiDeveloperProjectTemplate recipe={
        .structure_size=sizeof recipe,.api_version=UMI_DEVELOPER_PROJECT_API_VERSION,
        .template_id="allocation.recipe",.title="Allocation",.description="Owned candidate",
        .kind=UMI_DEVELOPER_PROJECT_EXECUTABLE,.build_system=UMI_DEVELOPER_PROJECT_BUILD_CMAKE,
        .primary_language_id="developer.language.c23",.files=files,.file_count=1U
    };
    CHECK(output!=NULL&&before!=NULL);
    memset(output,0xa5,sizeof *output);memcpy(before,output,sizeof *before);
    umi_developer_project_variable_set_init(&variables);
    refuse=1;
    /* Framework declares OUT_OF_MEMORY. Preserve the earlier spelling below
     * for integration review; the active path keeps the same failure behavior. */
#if 0
    CHECK(umi_developer_project_generation_plan_build(&recipe,&variables,"C:/example",output)==UMI_STATUS_NO_MEMORY);
#else
    CHECK(umi_developer_project_generation_plan_build(&recipe,&variables,"C:/example",output)==UMI_STATUS_OUT_OF_MEMORY);
#endif
    CHECK(memcmp(output,before,sizeof *output)==0&&attempts==1U&&allocated==0U&&released==0U);
    refuse=0;
    files[0].content_template="{{missing}}";
    CHECK(umi_developer_project_generation_plan_build(&recipe,&variables,"C:/example",output)==UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(output,before,sizeof *output)==0&&allocated==1U&&released==1U);
    files[0].content_template="int main(void){return 0;}\n";
    CHECK(umi_developer_project_generation_plan_build(&recipe,&variables,"C:/example",output)==UMI_STATUS_OK);
    CHECK(output->file_count==1U&&allocated==2U&&released==2U);
    free(output);free(before);return 0;
failed:
    free(output);free(before);return 1;
}
