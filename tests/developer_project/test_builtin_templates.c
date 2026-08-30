/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_builtin_templates.c
 *
 * PURPOSE:
 *   Verify all built-in project templates register together.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_project/builtin_templates.h"

int main(void)
{
    UmiDeveloperProjectTemplateRegistry *registry = NULL;

    assert(umi_developer_project_builtin_template_count() == 17U);
    assert(umi_developer_project_template_registry_create(&registry) ==
           UMI_STATUS_OK);
    assert(umi_developer_project_builtin_templates_register(registry) ==
           UMI_STATUS_OK);
    assert(umi_developer_project_template_registry_count(registry) ==
           17U);

    assert(umi_developer_project_template_registry_find(
        registry, "developer.template.c23-console") != NULL);
    assert(umi_developer_project_template_registry_find(
        registry, "developer.template.thin-desktop-application") != NULL);
    assert(umi_developer_project_template_registry_find(
        registry, "developer.template.rust-console") != NULL);
    assert(umi_developer_project_template_registry_find(
        registry, "developer.template.zig-console") != NULL);

    umi_developer_project_template_registry_destroy(registry);
    return 0;
}
