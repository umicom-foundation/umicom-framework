/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_gtk_starter_branding.c
 *
 * PURPOSE:
 *   Inspect generated GTK recipes for shared SDK branding, native identity and honest qualification guidance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/developer_project/generation_plan.h"
#include "umicom/developer_project/generation_request.h"
#include "umicom/developer_project/templates/c23_gtk4.h"
#include "umicom/developer_project/templates/cpp_gtk4.h"
#include "umicom/developer_project/templates/thin_desktop_application.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if(!(c)){fprintf(stderr,"%d: %s\n",__LINE__,#c);free(plan);return 1;} } while(0)
static const char *Content(const UmiDeveloperProjectGenerationPlan *plan,const char *path)
{
    for(size_t i=0U;i<plan->file_count;++i)
        if(!strcmp(plan->files[i].relative_path,path))return plan->files[i].content;
    return NULL;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const UmiDeveloperProjectTemplate *recipe=NULL;
    const char *source_path="src/main.c";
    if(!strcmp(argv[1],"c23"))recipe=umi_developer_project_template_c23_gtk4();
    else if(!strcmp(argv[1],"cpp")){recipe=umi_developer_project_template_cpp_gtk4();source_path="src/main.cpp";}
    else if(!strcmp(argv[1],"thin"))recipe=umi_developer_project_template_thin_desktop_application();
    else return 2;
    UmiDeveloperProjectGenerationPlan *plan=calloc(1U,sizeof *plan);
    CHECK(plan!=NULL);
    UmiDeveloperProjectGenerationRequest request;
    UmiDeveloperProjectVariableSet variables;
    umi_developer_project_generation_request_init(&request);
    strcpy(request.template_id,recipe->template_id);
    strcpy(request.application_name,"Umicom Example");
    strcpy(request.application_id,"org.umicom.example");
    strcpy(request.repository_name,"umicom-example");
    strcpy(request.target_name,"umicom_example");
    strcpy(request.project_root,"C:/review/example");
    CHECK(umi_developer_project_generation_request_variables(&request,&variables)==UMI_STATUS_OK);
    CHECK(umi_developer_project_generation_plan_build(recipe,&variables,request.project_root,plan)==UMI_STATUS_OK);
    const char *cmake=Content(plan,"CMakeLists.txt"),*main_source=Content(plan,source_path),*readme=Content(plan,"README.md");
    CHECK(cmake!=NULL&&main_source!=NULL&&readme!=NULL);
    CHECK(strstr(cmake,"find_package(UmicomFramework CONFIG REQUIRED COMPONENTS ui_gtk4)")!=NULL);
    CHECK(strstr(cmake,"NOT TARGET Umicom::ui_gtk4")!=NULL);
    CHECK(strstr(cmake,"umicom_apply_application_branding(")!=NULL);
    CHECK(strstr(cmake,"PRODUCT_NAME \"Umicom Example\"")!=NULL);
    CHECK(strstr(cmake,"APPLICATION_ID \"org.umicom.example\"")!=NULL);
    CHECK(strstr(cmake,"RESOURCE_ROOT \"${_application_resources}\"")!=NULL);
    CHECK(strstr(cmake,"WINDOWS_GUI DESKTOP_ENTRY")!=NULL);
    CHECK(strstr(main_source,"umi_gtk4_ws_apply_window_identity(GTK_WINDOW(window))")!=NULL);
    CHECK(strstr(main_source,"g_warning(\"Umicom window identity unavailable")!=NULL);
    CHECK(strstr(main_source,"PURPOSE:")!=NULL&&strstr(main_source,"AUTHOR AND ORGANISATION:")!=NULL&&strstr(main_source,"LICENCE:")!=NULL);
    CHECK(strstr(readme,"CMAKE_PREFIX_PATH=")!=NULL&&strstr(readme,"no application-specific CTest suite yet")!=NULL);
    CHECK(strstr(readme,"GTK's inherited fonts")!=NULL);
    for(size_t i=0U;i<plan->file_count;++i) {
        CHECK(strstr(plan->files[i].content,"{{")==NULL);
        /* Asset bytes come from the same SDK at build time; a text recipe must
         * not silently replace the canonical binary/icon transport. */
        CHECK(strcmp(plan->files[i].relative_path,"umicom.ico")!=0);
    }
    free(plan);return 0;
}
