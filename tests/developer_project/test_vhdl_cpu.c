/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_vhdl_cpu.c
 *
 * PURPOSE:
 *   Verify a complete CPU simulation project, shared language identity and conservative capabilities.
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
#include "umicom/developer_project/languages/vhdl.h"
#include "umicom/developer_project/templates/vhdl_cpu.h"
#include "umicom/document/languages.h"
#include "umicom/editor/comment_style.h"

/* Search the generated plan by its public path, independently of file ordering. */
static const char *content_at(const UmiDeveloperProjectGenerationPlan *plan, const char *path)
{
    for (size_t i = 0U; i < plan->file_count; ++i)
        if (strcmp(plan->files[i].relative_path, path) == 0)
            return plan->files[i].content;
    return NULL;
}

int main(void)
{
    const UmiDeveloperProjectLanguagePack *language = umi_developer_project_language_vhdl();
    const UmiDeveloperProjectTemplate *recipe = umi_developer_project_template_vhdl_cpu();
    UmiDeveloperProjectGenerationPlan *plan = calloc(1U, sizeof *plan);
    UmiDeveloperProjectGenerationRequest request;
    UmiDeveloperProjectVariableSet variables;
    UmiDocumentLanguageIdentity identity;
    const UmiEditorLineCommentStyle *comments = NULL;
    assert(plan != NULL);
    assert(umi_developer_project_language_pack_validate(language) == UMI_STATUS_OK);
    assert(umi_developer_project_template_validate(recipe) == UMI_STATUS_OK);
    assert(umi_developer_project_language_pack_supports_extension(language, ".vhd"));
    assert(umi_developer_project_language_pack_supports_extension(language, ".vhdl"));
    assert(!umi_developer_project_language_pack_supports_extension(language, ".c"));
    assert(language->capabilities == (UMI_DEVELOPER_PROJECT_LANGUAGE_BUILD |
                                      UMI_DEVELOPER_PROJECT_LANGUAGE_TEST));
    assert(language->debug_adapter[0] == '\0');
    assert(recipe->build_system == UMI_DEVELOPER_PROJECT_BUILD_CMAKE && recipe->supports_tests);
    assert(UmiDocumentLanguageChoiceIdentity("vhdl", &identity) == UMI_STATUS_OK);
    assert(!strcmp(identity.language_id, "vhdl"));
    assert(UmiEditorLineCommentStyleFind("vhdl", &comments) == UMI_STATUS_OK);
    assert(!strcmp(comments->prefix, "--"));

    umi_developer_project_generation_request_init(&request);
    strcpy(request.template_id, recipe->template_id);
    strcpy(request.application_name, "Learning CPU");
    strcpy(request.application_id, "org.umicom.learning.cpu");
    strcpy(request.repository_name, "learning-cpu");
    strcpy(request.target_name, "learning_cpu");
    strcpy(request.project_root, "C:/work/CPU designs/learning-cpu");
    assert(umi_developer_project_generation_request_variables(&request, &variables) == UMI_STATUS_OK);
    assert(umi_developer_project_generation_plan_build(recipe, &variables, request.project_root,
                                                     plan) == UMI_STATUS_OK);
    assert(umi_developer_project_generation_plan_validate(plan) == UMI_STATUS_OK);
    /* The instruction-only recipe is retained as a historical expectation.
     * The complete project now includes ROM, composition and program coverage. */
#if 0
    assert(plan->file_count == 6U);
#endif
    assert(plan->file_count == 9U);
    assert(!strcmp(plan->project_root, request.project_root));
    for (size_t i = 0U; i < plan->file_count; ++i) {
        assert(strstr(plan->files[i].content, "{{") == NULL);
        assert(plan->files[i].relative_path[0] != '/');
        assert(strstr(plan->files[i].relative_path, "..") == NULL);
    }
    const char *build = content_at(plan, "CMakeLists.txt");
    const char *presets = content_at(plan, "CMakePresets.json");
    const char *model = content_at(plan, "rtl/learning_cpu.vhd");
    const char *bench = content_at(plan, "tests/tb_learning_cpu.vhd");
    assert(build && presets && model && bench && content_at(plan, "README.md"));
    assert(strstr(build, "LANGUAGES NONE") && strstr(build, "VERBATIM"));
    assert(strstr(build, "add_test(") && strstr(build, "--assert-level=error"));
    assert(strstr(build, "TIMEOUT 15") && strstr(build, "DEPENDS"));
    assert(strstr(presets, "simulation") && strstr(presets, "BUILD_TESTING"));
    assert(strstr(model, "rising_edge(clock)") && strstr(model, "when others"));
    assert(strstr(bench, "CPU checks passed") && strstr(bench, "severity failure"));
    assert(strstr(bench, "deadline") && strstr(bench, "finish;"));
    const char *program = content_at(plan, "rtl/learning_program.vhd");
    const char *system = content_at(plan, "rtl/learning_system.vhd");
    const char *trace = content_at(plan, "tests/tb_learning_system.vhd");
    assert(program && system && trace);
    assert(strstr(program, "is_x") && strstr(program, "others => x\"60\""));
    assert(strstr(system, "entity work.learning_cpu") &&
           strstr(system, "entity work.learning_program"));
    assert(strstr(trace, "expected_pc") && strstr(trace, "expected_acc"));
    assert(strstr(trace, "Stored program checks passed") && strstr(trace, "deadline"));
    assert(strstr(build, "program_analysis ALL") && strstr(build, "NAME cpu.program"));
    assert(strstr(build, "ghdl-program") && strstr(build, "simulate_program"));
    free(plan);
    return 0;
}
