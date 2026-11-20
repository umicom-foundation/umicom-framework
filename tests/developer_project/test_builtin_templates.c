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
/* Required fixture setup and checks also execute in optimized builds. */
#include "../support/assertions.h"
#include <assert.h>

#include "umicom/developer_project/builtin_templates.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDeveloperProjectTemplateRegistry *registry = NULL;

    /* The earlier catalogue count is retained for review. VHDL and Umicom C add
     * registered entries; the current assertion covers the expanded catalogue. */
#if 0
    assert(umi_developer_project_builtin_template_count() == 18U);
#endif
    /* Preserve the former count; D and FreeBASIC add two registered recipes. */
#if 0
    assert(umi_developer_project_builtin_template_count() == 20U);
#endif
    /* Keep the pre-Pascal total for catalogue history. The current total
     * includes the Pascal recipe used by both Studio and Umicom Command. */
#if 0
    assert(umi_developer_project_builtin_template_count() == 22U);
#endif
    /* Preserve the previous total for catalogue review. Java adds one recipe. */
#if 0
    assert(umi_developer_project_builtin_template_count() == 23U);
#endif
    assert(umi_developer_project_builtin_template_count() == 24U);
    assert(umi_developer_project_template_registry_create(&registry) ==
           UMI_STATUS_OK);
    assert(umi_developer_project_builtin_templates_register(registry) ==
           UMI_STATUS_OK);
    /* The earlier catalogue count is retained for review. VHDL and Umicom C add
     * registered entries; the current assertion covers the expanded catalogue. */
#if 0
    assert(umi_developer_project_template_registry_count(registry) ==
           18U);
#endif
    /* Preserve the former count; D and FreeBASIC add two registered recipes. */
#if 0
    assert(umi_developer_project_template_registry_count(registry) ==
           20U);
#endif
    /* Keep the pre-Pascal registration check for comparison with the expanded
     * catalogue. The active count must agree with the factory list. */
#if 0
    assert(umi_developer_project_template_registry_count(registry) ==
           22U);
#endif
    /* The registration check now includes the same Java recipe as the list. */
#if 0
    assert(umi_developer_project_template_registry_count(registry) == 23U);
#endif
    assert(umi_developer_project_template_registry_count(registry) == 24U);

    assert(umi_developer_project_template_registry_find(
        registry, "developer.template.c23-console") != NULL);
    assert(umi_developer_project_template_registry_find(
        registry, "developer.template.thin-desktop-application") != NULL);
    assert(umi_developer_project_template_registry_find(
        registry, "developer.template.rust-console") != NULL);
    assert(umi_developer_project_template_registry_find(
        registry, "developer.template.zig-console") != NULL);
    assert(umi_developer_project_template_registry_find(
        registry, "developer.template.umicom-extension") != NULL);

    assert(umi_developer_project_template_registry_find(
        registry, "developer.template.vhdl-cpu") != NULL);

    assert(umi_developer_project_template_registry_find(
        registry, "developer.template.umicom-c-preview") != NULL);
    const UmiDeveloperProjectTemplate *pascal = umi_developer_project_template_registry_find(
        registry, "developer.template.pascal-console");
    assert(pascal != NULL && pascal->supports_tests && pascal->supports_install);
    const UmiDeveloperProjectTemplate *java = umi_developer_project_template_registry_find(
        registry, "developer.template.java-console");
    assert(java != NULL && java->supports_tests && java->supports_install);

    umi_developer_project_template_registry_destroy(registry);
    return 0;
}
