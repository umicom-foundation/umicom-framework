/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_umicom_c_preview.c
 *
 * PURPOSE:
 *   Verify the arithmetic language identity and complete project generation without advertising unsupported native debugging.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../support/assertions.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/developer_project/generation_plan.h"
#include "umicom/developer_project/generation_request.h"
#include "umicom/developer_project/languages/umicom_c.h"
#include "umicom/developer_project/templates/umicom_c_preview.h"
#include "umicom/document/language_detection.h"
#include "umicom/document/languages.h"
#include "umicom/editor/comment_style.h"

static const char *file_text(const UmiDeveloperProjectGenerationPlan *plan,
                             const char *relative_path)
{
    for (size_t i = 0U; i < plan->file_count; ++i)
        if (strcmp(plan->files[i].relative_path, relative_path) == 0)
            return plan->files[i].content;
    return NULL;
}

int main(void)
{
    const UmiDeveloperProjectLanguagePack *language = umi_developer_project_language_umicom_c();
    const UmiDeveloperProjectTemplate *recipe = umi_developer_project_template_umicom_c_preview();
    UmiDeveloperProjectGenerationPlan *plan = calloc(1U, sizeof *plan);
    UmiDeveloperProjectGenerationRequest request;
    UmiDeveloperProjectVariableSet variables;
    UmiDocumentLanguageIdentity identity;
    const UmiEditorLineCommentStyle *comments = NULL;
    assert(plan != NULL);
    assert(umi_developer_project_language_pack_validate(language) == UMI_STATUS_OK);
    assert(umi_developer_project_template_validate(recipe) == UMI_STATUS_OK);
    assert(umi_developer_project_language_pack_supports_extension(language, ".uc"));
    assert(!umi_developer_project_language_pack_supports_extension(language, ".c"));
    assert(language->capabilities == (UMI_DEVELOPER_PROJECT_LANGUAGE_BUILD |
                                      UMI_DEVELOPER_PROJECT_LANGUAGE_TEST));
    assert(language->debug_adapter[0] == '\0');
    assert(!strcmp(recipe->primary_language_id, language->language_id));
    assert(recipe->kind == UMI_DEVELOPER_PROJECT_META && recipe->supports_tests);
    assert(!recipe->supports_install && !recipe->supports_package);
    assert(umi_document_language_detect("C:/source folder/main.UC", &identity) == UMI_STATUS_OK);
    assert(!strcmp(identity.language_id, "umicom-c"));
    assert(UmiDocumentLanguageChoiceIdentity("umicom-c", &identity) == UMI_STATUS_OK);
    assert(!strcmp(identity.language_id, "umicom-c"));
    assert(UmiEditorLineCommentStyleFind("umicom-c", &comments) == UMI_STATUS_OK);
    assert(!strcmp(comments->prefix, "//"));

    umi_developer_project_generation_request_init(&request);
    strcpy(request.template_id, recipe->template_id);
    strcpy(request.application_name, "Arithmetic Preview");
    strcpy(request.application_id, "org.umicom.learning.arithmetic");
    strcpy(request.repository_name, "arithmetic-preview");
    strcpy(request.target_name, "arithmetic_preview");
    strcpy(request.project_root, "C:/work/Compiler lessons/arithmetic-preview");
    assert(umi_developer_project_generation_request_variables(&request, &variables) == UMI_STATUS_OK);
    assert(umi_developer_project_generation_plan_build(recipe, &variables,
        request.project_root, plan) == UMI_STATUS_OK);
    assert(umi_developer_project_generation_plan_validate(plan) == UMI_STATUS_OK);
    assert(plan->file_count == 8U);
    for (size_t i = 0U; i < plan->file_count; ++i) {
        assert(strstr(plan->files[i].content, "{{") == NULL);
        assert(plan->files[i].relative_path[0] != '/');
        assert(strstr(plan->files[i].relative_path, "..") == NULL);
    }
    const char *build = file_text(plan, "CMakeLists.txt");
    const char *main_source = file_text(plan, "src/main.uc");
    const char *checks = file_text(plan, "tests/check_preview.cmake");
    assert(build && main_source && checks);
    assert(strstr(build, "project(arithmetic_preview LANGUAGES NONE)"));
    assert(strstr(build, "DEPENDS") && strstr(build, "VERBATIM"));
    assert(strstr(build, "check-source ALL") && strstr(build, "TIMEOUT 10"));
    /* The generated project delegates inspection to the installed compiler. */
    assert(strstr(build, "add_custom_target(inspect-program"));
    assert(strstr(build, " inspect "));
    assert(strstr(file_text(plan, "README.md"), "Inspection does not execute arithmetic"));
    assert(strstr(main_source, "return 6 * 7;"));
    assert(strstr(main_source, "Arithmetic Preview"));
    assert(strstr(checks, "expected_exit") && strstr(checks, "TIMEOUT 5"));
    assert(strstr(checks, "Result: 42") && strstr(checks, "--max-steps 1"));
    assert(file_text(plan, "tests/invalid.uc") && file_text(plan, "tests/division.uc"));
    assert(file_text(plan, "CMakePresets.json") && file_text(plan, "README.md"));
    free(plan);
    return 0;
}
