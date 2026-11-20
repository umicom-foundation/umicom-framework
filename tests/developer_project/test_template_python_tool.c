/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_template_python_tool.c
 *
 * PURPOSE:
 *   Verify the built-in Python Tool project-template contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Required fixture setup and checks also execute in optimized builds. */
#include "../support/assertions.h"
#include <assert.h>
#include <string.h>

#include "umicom/developer_project/templates/python_tool.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiDeveloperProjectTemplate *project_template =
        umi_developer_project_template_python_tool();

    assert(project_template != NULL);
    /* The older recipe supplied four files. The current contract requires
     * the complete staged build, test and runtime inputs by name. */
#if 0
    assert(project_template->file_count == 4U);
#endif
    const char *const required[] = {
        "CMakeLists.txt", "CMakePresets.json", "src/{{TARGET_NAME}}.py",
        "src/application-name.txt", "tests/check_syntax.py", "tests/test_{{TARGET_NAME}}.py",
        "README.md", ".gitignore"
    };
    assert(project_template->file_count == sizeof required / sizeof required[0]);
    assert(project_template->kind == UMI_DEVELOPER_PROJECT_SCRIPT);
    assert(project_template->build_system == UMI_DEVELOPER_PROJECT_BUILD_CMAKE);
    assert(project_template->supports_tests && project_template->supports_install);
    for (size_t i = 0U; i < sizeof required / sizeof required[0]; ++i) {
        int found = 0;
        for (size_t j = 0U; j < project_template->file_count; ++j)
            if (strcmp(required[i], project_template->files[j].relative_path) == 0) ++found;
        assert(found == 1);
    }
    assert(umi_developer_project_template_validate(project_template) ==
           UMI_STATUS_OK);
    return 0;
}
