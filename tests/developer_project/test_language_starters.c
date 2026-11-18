/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_language_starters.c
 *
 * PURPOSE:
 *   Exercise language generation and launch defaults through the service shared by Studio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/developer_project/service.h"
#include "umicom/developer_project/builtin_languages.h"
#include "umicom/developer_project/languages/languages.h"
#include "umicom/build/project_profile.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); result = 1; goto done; } } while (0)
int main(int argc, char **argv)
{
    UmiDeveloperProjectService *service = NULL;
    UmiDeveloperProjectGenerationRequest request;
    UmiDeveloperProjectGeneratorReport report;
    UmiDeveloperProjectModel model;
    UmiBuildProfile profile;
    char cwd[UMI_PATH_CAPACITY];
    int result = 0;
    const char *ids[] = {"d", "freebasic", "umicom-c"};
    const char *templates[] = {"developer.template.d-console",
        "developer.template.freebasic-console", "developer.template.umicom-c-preview"};
    CHECK(argc == 1 || argc == 2);
    CHECK(umi_developer_project_builtin_language_count() <= UMI_DEVELOPER_PROJECT_LANGUAGE_CAPACITY);
    CHECK(umi_developer_project_service_create(&service) == UMI_STATUS_OK);
    CHECK(umi_fs_current_directory(cwd, sizeof cwd) == UMI_STATUS_OK);
    for (size_t i = 0; i < 3; ++i) {
        umi_developer_project_generation_request_init(&request);
        strcpy(request.template_id, templates[i]);
        strcpy(request.application_name, "Language Example");
        strcpy(request.application_id, "org.umicom.language-example");
        strcpy(request.repository_name, "language-example");
        strcpy(request.target_name, "language_example");
        CHECK(umi_fs_join(request.project_root, sizeof request.project_root,
            argc == 2 ? argv[1] : cwd, ids[i]) == UMI_STATUS_OK);
        request.dry_run = argc != 2;
        CHECK(umi_developer_project_service_generate(service, &request, &report, &model) == UMI_STATUS_OK);
        CHECK(UmiBuildProfileFromProject(&model, &profile) == UMI_STATUS_OK);
        if (i < 2) {
            CHECK(model.kind == UMI_DEVELOPER_PROJECT_EXECUTABLE);
            CHECK(strcmp(model.executable, profile.run_program) == 0);
            CHECK(strstr(profile.run_program, "/bin/language_example") != NULL);
            CHECK(profile.run_arguments[0] == '\0');
        } else {
            CHECK(strcmp(profile.run_program, "cmake") == 0);
            CHECK(strcmp(profile.run_arguments, "--build build/preview --target preview") == 0);
        }
    }
    CHECK(umi_developer_project_language_pack_supports_extension(
        umi_developer_project_language_freebasic(), ".bas"));
    CHECK(!umi_developer_project_language_pack_supports_extension(
        umi_developer_project_language_freebasic(), ".vb"));
    CHECK(!umi_developer_project_language_pack_supports_extension(
        umi_developer_project_language_umicom_c(), ".c"));
done:
    umi_developer_project_service_destroy(service);
    return result;
}
