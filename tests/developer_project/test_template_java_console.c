/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_template_java_console.c
 *
 * PURPOSE:
 *   Verify that generated Java sources, CMake target and tested JAR agree.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/templates/java_console.h"
#include "../support/assertions.h"
#include <assert.h>
#include <string.h>

static const char *File(const UmiDeveloperProjectTemplate *recipe, const char *name)
{
    for (size_t i = 0U; i < recipe->file_count; ++i)
        if (strcmp(recipe->files[i].relative_path, name) == 0)
            return recipe->files[i].content_template;
    return NULL;
}

int main(void)
{
    const UmiDeveloperProjectTemplate *recipe = umi_developer_project_template_java_console();
    assert(umi_developer_project_template_validate(recipe) == UMI_STATUS_OK);
    assert(strcmp(recipe->primary_language_id, "developer.language.java") == 0);
    assert(recipe->kind == UMI_DEVELOPER_PROJECT_META && recipe->supports_tests && recipe->supports_install);
    const char *build = File(recipe, "CMakeLists.txt");
    const char *source = File(recipe, "src/Main.java");
    const char *test = File(recipe, "tests/check_console.cmake");
    assert(build != NULL && source != NULL && test != NULL && File(recipe, "CMakePresets.json") != NULL);
    assert(strstr(build, "find_package(Java REQUIRED") != NULL);
    assert(strstr(build, "src/Main.java ENTRY_POINT Main") != NULL);
    assert(strstr(build, "TARGET_PROPERTY:java_application,JAR_FILE") != NULL);
    assert(strstr(build, "install_jar(java_application DESTINATION bin COMPONENT Runtime)") != NULL);
    assert(strstr(source, "public static void main") != NULL && strstr(source, "{{APPLICATION_NAME}}") != NULL);
    assert(strstr(test, "execute_process") != NULL && strstr(test, "PROGRAM_ARGS") != NULL);
    assert(strstr(test, "diagnostic STREQUAL") != NULL);
    return 0;
}
