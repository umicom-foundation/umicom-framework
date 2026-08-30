/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/builtin_templates.c
 *
 * PURPOSE:
 *   Enumerate and install all built-in project templates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/builtin_templates.h"

#include "umicom/developer_project/templates/templates.h"

typedef const UmiDeveloperProjectTemplate *(*TemplateFactory)(void);

static const TemplateFactory FACTORIES[] = {
    umi_developer_project_template_c23_console,
    umi_developer_project_template_c23_gtk4,
    umi_developer_project_template_c23_static_library,
    umi_developer_project_template_c23_shared_library,
    umi_developer_project_template_cpp_console,
    umi_developer_project_template_cpp_gtk4,
    umi_developer_project_template_assembly_console,
    umi_developer_project_template_c23_assembly,
    umi_developer_project_template_cpython_extension,
    umi_developer_project_template_bash_tool,
    umi_developer_project_template_zig_console,
    umi_developer_project_template_rust_console,
    umi_developer_project_template_web_service_c23,
    umi_developer_project_template_thin_desktop_application,
    umi_developer_project_template_integration_worker,
    umi_developer_project_template_cmake_meta_project,
    umi_developer_project_template_python_tool
};

size_t umi_developer_project_builtin_template_count(void)
{
    return sizeof(FACTORIES) / sizeof(FACTORIES[0]);
}

const UmiDeveloperProjectTemplate *
umi_developer_project_builtin_template_at(size_t index)
{
    return index < umi_developer_project_builtin_template_count()
        ? FACTORIES[index]()
        : NULL;
}

UmiStatus umi_developer_project_builtin_templates_register(
    UmiDeveloperProjectTemplateRegistry *registry)
{
    size_t index;

    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    for (index = 0U;
         index < umi_developer_project_builtin_template_count();
         ++index) {
        const UmiStatus status =
            umi_developer_project_template_registry_register(
                registry, FACTORIES[index]());
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
