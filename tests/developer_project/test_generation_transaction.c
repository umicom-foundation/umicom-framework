/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_generation_transaction.c
 *
 * PURPOSE:
 *   Keep a complete reviewed plan unchanged across rendering and admission failures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/developer_project/generation_plan.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%d: %s\n",__LINE__,#c); goto failed; } } while (0)
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const char *name=argv[1];
    UmiDeveloperProjectGenerationPlan *plan=calloc(1U,sizeof *plan),*before=malloc(sizeof *before);
    UmiDeveloperProjectVariableSet variables;
    UmiDeveloperProjectTemplateFile files[UMI_DEVELOPER_PROJECT_TEMPLATE_FILE_CAPACITY]={{"first.c","first",0,0},{"second.c","{{VALUE}}",0,0}};
    char paths[UMI_DEVELOPER_PROJECT_TEMPLATE_FILE_CAPACITY][32];
    char long_content[UMI_DEVELOPER_PROJECT_GENERATED_CONTENT_CAPACITY+1U];
    char long_path[UMI_DEVELOPER_PROJECT_PATH_CAPACITY+1U];
    char long_id[UMI_DEVELOPER_PROJECT_ID_CAPACITY+1U];
    memset(long_content,'x',sizeof long_content-1U);long_content[sizeof long_content-1U]='\0';
    memset(long_path,'p',sizeof long_path-1U);long_path[sizeof long_path-1U]='\0';
    memset(long_id,'i',sizeof long_id-1U);long_id[sizeof long_id-1U]='\0';
    UmiDeveloperProjectTemplate recipe={
        .structure_size=sizeof recipe,.api_version=UMI_DEVELOPER_PROJECT_API_VERSION,
        .template_id="fixture.recipe",.title="Fixture",.description="Atomic publication",
        .kind=UMI_DEVELOPER_PROJECT_EXECUTABLE,.build_system=UMI_DEVELOPER_PROJECT_BUILD_CMAKE,
        .primary_language_id="developer.language.c23",.files=files,.file_count=2U
    };
    CHECK(plan!=NULL&&before!=NULL);
    umi_developer_project_variable_set_init(&variables);
    CHECK(umi_developer_project_variable_set(&variables,"VALUE","second")==UMI_STATUS_OK);
    CHECK(umi_developer_project_generation_plan_build(&recipe,&variables,"C:/reviewed",plan)==UMI_STATUS_OK);
    memcpy(before,plan,sizeof *before);
    const char *root_path="C:/replacement";
    UmiDeveloperProjectGenerationPlan *output=plan;
    UmiStatus expected=UMI_STATUS_OK;
    if(!strcmp(name,"replace")) { }
    else if(!strcmp(name,"late-syntax")) { files[1].content_template="prefix {{VALUE";expected=UMI_STATUS_PARSE_ERROR; }
    else if(!strcmp(name,"late-missing")) { files[1].content_template="{{MISSING}}";expected=UMI_STATUS_NOT_FOUND; }
    else if(!strcmp(name,"late-content-capacity")) { files[1].content_template=long_content;expected=UMI_STATUS_CAPACITY_EXCEEDED; }
    else if(!strcmp(name,"late-path-capacity")) { files[1].relative_path=long_path;expected=UMI_STATUS_CAPACITY_EXCEEDED; }
    else if(!strcmp(name,"root-capacity")) { root_path=long_path;expected=UMI_STATUS_CAPACITY_EXCEEDED; }
    else if(!strcmp(name,"id-capacity")) { recipe.template_id=long_id;expected=UMI_STATUS_CAPACITY_EXCEEDED; }
    else if(!strcmp(name,"rendered-duplicate")) {
        CHECK(umi_developer_project_variable_set(&variables,"PATH","first.c")==UMI_STATUS_OK);
        files[1].relative_path="{{PATH}}";expected=UMI_STATUS_ALREADY_EXISTS;
    } else if(!strcmp(name,"rendered-traversal")||!strcmp(name,"rendered-drive")) {
        CHECK(umi_developer_project_variable_set(&variables,"PATH",!strcmp(name,"rendered-drive")?"C:/outside":"../outside")==UMI_STATUS_OK);
        files[1].relative_path="{{PATH}}";expected=UMI_STATUS_PERMISSION_DENIED;
    } else if(!strcmp(name,"invalid-variable-count")) { variables.count=UMI_DEVELOPER_PROJECT_VARIABLE_CAPACITY+1U;expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(!strcmp(name,"invalid-variable-value")) { memset(variables.variables[0].value,'v',sizeof variables.variables[0].value);expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(!strcmp(name,"later-null-path")) { files[1].relative_path=NULL;expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(!strcmp(name,"empty-root")) { root_path="";expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(!strcmp(name,"null-output")) { output=NULL;expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(!strcmp(name,"aliased-root")) root_path=plan->project_root;
    else if(!strcmp(name,"aliased-content")) files[1].content_template=plan->files[0].content;
    else if(!strcmp(name,"empty-content")) files[1].content_template="";
    else if(!strcmp(name,"maximum-files")) {
        recipe.file_count=UMI_DEVELOPER_PROJECT_TEMPLATE_FILE_CAPACITY;
        for(size_t i=0U;i<recipe.file_count;++i) {
            snprintf(paths[i],sizeof paths[i],"source%zu.c",i);
            files[i]=(UmiDeveloperProjectTemplateFile){paths[i],"{{VALUE}}",0,0};
        }
    } else if(!strcmp(name,"template-refusal")) { recipe.api_version=0U;expected=UMI_STATUS_INVALID_ARGUMENT; }
    else { free(plan);free(before);return 2; }
    CHECK(umi_developer_project_generation_plan_build(&recipe,&variables,root_path,output)==expected);
    if(expected!=UMI_STATUS_OK) CHECK(memcmp(plan,before,sizeof *plan)==0);
    else {
        CHECK(umi_developer_project_generation_plan_validate(plan)==UMI_STATUS_OK);
        CHECK(plan->file_count==recipe.file_count&&plan->revision==2U);
        CHECK(!strcmp(plan->project_root,!strcmp(name,"aliased-root")?"C:/reviewed":"C:/replacement"));
        if(!strcmp(name,"aliased-content")) CHECK(!strcmp(plan->files[1].content,"first"));
        if(!strcmp(name,"empty-content")) CHECK(plan->files[1].content[0]=='\0');
        size_t bytes=0U;
        for(size_t i=0U;i<plan->file_count;++i) bytes+=strlen(plan->files[i].content);
        CHECK(plan->total_content_bytes==bytes);
    }
    free(plan);free(before);return 0;
failed:
    free(plan);free(before);return 1;
}
