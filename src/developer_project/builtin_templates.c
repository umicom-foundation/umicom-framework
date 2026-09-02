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

/*
 * Return the number of records represented by developer project builtin template without
 * changing their state.
 */
size_t umi_developer_project_builtin_template_count(void)
{
    return sizeof(FACTORIES) / sizeof(FACTORIES[0]);
}

/*
 * Find developer project builtin template while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDeveloperProjectTemplate *
umi_developer_project_builtin_template_at(size_t index)
{
    return index < umi_developer_project_builtin_template_count()
        ? FACTORIES[index]()
        : NULL;
}

/*
 * Add developer project builtin templates only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_developer_project_builtin_templates_register(
    UmiDeveloperProjectTemplateRegistry *registry)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_developer_project_builtin_template_count();
         ++index) {
        const UmiStatus status =
            umi_developer_project_template_registry_register(
                registry, FACTORIES[index]());
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
