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
    const char *ids[] = {"d", "freebasic", "umicom-c", "pascal", "python", "java"};
    const char *templates[] = {"developer.template.d-console",
        "developer.template.freebasic-console", "developer.template.umicom-c-preview",
        "developer.template.pascal-console", "developer.template.python-tool",
        "developer.template.java-console"};
    const char *entries[] = {"src/main.d", "src/main.bas", NULL, "src/main.pas",
        "src/language_example.py", "src/Main.java"};
    CHECK(argc == 1 || argc == 2);
    CHECK(umi_developer_project_builtin_language_count() <= UMI_DEVELOPER_PROJECT_LANGUAGE_CAPACITY);
    CHECK(umi_developer_project_service_create(&service) == UMI_STATUS_OK);
    CHECK(umi_fs_current_directory(cwd, sizeof cwd) == UMI_STATUS_OK);
    /* Derive the active loop bound from the catalogue cases. Retain the old
     * fixed bound to show which cases the earlier test covered. */
#if 0
    for (size_t i = 0; i < 3; ++i) {
#endif
    static_assert(sizeof ids / sizeof ids[0] == sizeof templates / sizeof templates[0]);
    static_assert(sizeof ids / sizeof ids[0] == sizeof entries / sizeof entries[0]);
    for (size_t i = 0; i < sizeof ids / sizeof ids[0]; ++i) {
        umi_developer_project_generation_request_init(&request);
        strcpy(request.template_id, templates[i]);
        strcpy(request.application_name, strcmp(ids[i], "python") == 0 ?
            "Language Example - Python" : "Language Example");
        strcpy(request.application_id, "org.umicom.language-example");
        strcpy(request.repository_name, "language-example");
        strcpy(request.target_name, "language_example");
        CHECK(umi_fs_join(request.project_root, sizeof request.project_root,
            argc == 2 ? argv[1] : cwd, ids[i]) == UMI_STATUS_OK);
        request.dry_run = argc != 2;
        const UmiStatus generation = umi_developer_project_service_generate(service, &request, &report, &model);
        if (generation != UMI_STATUS_OK)
            fprintf(stderr, "%s generation: %s\n", ids[i], umi_status_text(generation));
        CHECK(generation == UMI_STATUS_OK);
        CHECK(UmiBuildProfileFromProject(&model, &profile) == UMI_STATUS_OK);
        /* Identify the VM preview by kind of starter so adding another native
         * recipe does not change which launch assertions it receives. */
#if 0
        if (i < 2) {
#endif
        if (strcmp(ids[i], "python") == 0) {
            CHECK(model.kind == UMI_DEVELOPER_PROJECT_SCRIPT);
            CHECK(model.build_system == UMI_DEVELOPER_PROJECT_BUILD_CMAKE);
            CHECK(strcmp(model.entry_point, entries[i]) == 0 && model.executable[0] == '\0');
            CHECK(strcmp(profile.run_program, "cmake") == 0);
#ifdef _WIN32
            CHECK(strcmp(profile.run_arguments, "--build --preset windows-ucrt64-debug --target run-program") == 0);
#else
            CHECK(strcmp(profile.run_arguments, "--build --preset linux-debug --target run-program") == 0);
#endif
            /* An existing script-only model retains its previous launch behavior. */
            UmiDeveloperProjectModel legacy = model;
            legacy.build_system = UMI_DEVELOPER_PROJECT_BUILD_SCRIPT;
            legacy.preset[0] = '\0';
            legacy.build_directory[0] = '\0';
            CHECK(UmiBuildProfileFromProject(&legacy, &profile) == UMI_STATUS_OK);
            CHECK(profile.run_program[0] == '\0' && profile.run_arguments[0] == '\0');
        } else if (strcmp(ids[i], "java") == 0) {
            CHECK(model.kind == UMI_DEVELOPER_PROJECT_META);
            CHECK(model.build_system == UMI_DEVELOPER_PROJECT_BUILD_CMAKE);
            CHECK(strcmp(model.entry_point, entries[i]) == 0 && model.executable[0] == '\0');
            CHECK(strcmp(profile.run_program, "cmake") == 0);
            CHECK(strstr(profile.run_arguments, " --target run-program") != NULL);
            CHECK(strstr(profile.run_arguments, model.preset) != NULL);
        } else if (entries[i] != NULL) {
            CHECK(model.kind == UMI_DEVELOPER_PROJECT_EXECUTABLE);
            CHECK(strcmp(model.executable, profile.run_program) == 0);
            CHECK(strstr(profile.run_program, "/bin/language_example") != NULL);
            CHECK(profile.run_arguments[0] == '\0');
            CHECK(strcmp(model.entry_point, entries[i]) == 0);
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
    CHECK(umi_developer_project_language_pack_supports_extension(
        umi_developer_project_language_pascal(), ".pas"));
done:
    umi_developer_project_service_destroy(service);
    return result;
}
