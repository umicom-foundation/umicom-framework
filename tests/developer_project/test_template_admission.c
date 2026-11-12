/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_template_admission.c
 *
 * PURPOSE:
 *   Reject malformed borrowed recipes before comparing later file records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/developer_project/template.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%d: %s\n",__LINE__,#c); return 1; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1];
    const char *languages[] = {"developer.language.cmake","developer.language.assembly"};
    UmiDeveloperProjectTemplateFile files[] = {
        {"main.c","int main(void) { return 0; }\n",0,0},
        {"README.md","An original project.\n",0,0},
        {"notes.txt","",0,0}
    };
    UmiDeveloperProjectTemplate recipe = {
        .structure_size=sizeof recipe,.api_version=UMI_DEVELOPER_PROJECT_API_VERSION,
        .template_id="example.recipe",.title="Example",.description="A bounded fixture",
        .kind=UMI_DEVELOPER_PROJECT_EXECUTABLE,.build_system=UMI_DEVELOPER_PROJECT_BUILD_CMAKE,
        .primary_language_id="developer.language.c23",.secondary_language_ids=languages,
        .secondary_language_count=2U,.files=files,.file_count=3U
    };
    UmiStatus expected=UMI_STATUS_INVALID_ARGUMENT;
    if (!strcmp(name,"valid") || !strcmp(name,"empty-content")) expected=UMI_STATUS_OK;
    else if (!strcmp(name,"later-null-path")) files[1].relative_path=NULL;
    else if (!strcmp(name,"later-empty-path")) files[1].relative_path="";
    else if (!strcmp(name,"later-null-content")) files[1].content_template=NULL;
    else if (!strcmp(name,"late-invalid-before-duplicate")) {
        files[1].relative_path=files[0].relative_path; files[2].relative_path=NULL;
    } else if (!strcmp(name,"duplicate")) {
        files[2].relative_path=files[0].relative_path; expected=UMI_STATUS_ALREADY_EXISTS;
    } else if (!strcmp(name,"absolute")) files[1].relative_path="/tmp/other";
    else if (!strcmp(name,"backslash")) files[1].relative_path="\\outside";
    else if (!strcmp(name,"drive")) {
        files[1].relative_path="C:/outside"; expected=UMI_STATUS_PERMISSION_DENIED;
    } else if (!strcmp(name,"traversal")) {
        files[1].relative_path="../outside"; expected=UMI_STATUS_PERMISSION_DENIED;
    } else if (!strcmp(name,"null-language-array")) recipe.secondary_language_ids=NULL;
    else if (!strcmp(name,"null-later-language")) languages[1]=NULL;
    else if (!strcmp(name,"empty-later-language")) languages[1]="";
    else if (!strcmp(name,"language-count")) recipe.secondary_language_count=UMI_DEVELOPER_PROJECT_LANGUAGE_CAPACITY+1U;
    else if (!strcmp(name,"empty-primary")) recipe.primary_language_id="";
    else if (!strcmp(name,"file-count")) recipe.file_count=UMI_DEVELOPER_PROJECT_TEMPLATE_FILE_CAPACITY+1U;
    else if (!strcmp(name,"null-files")) recipe.files=NULL;
    else if (!strcmp(name,"api")) ++recipe.api_version;
    else if (!strcmp(name,"kind")) recipe.kind=(UmiDeveloperProjectKind)0;
    else if (!strcmp(name,"build-system")) recipe.build_system=(UmiDeveloperProjectBuildSystem)99;
    else return 2;
    CHECK(umi_developer_project_template_validate(&recipe)==expected);
    CHECK(umi_developer_project_template_validate(NULL)==UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
