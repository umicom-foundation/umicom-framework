/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_builtin_languages.c
 *
 * PURPOSE:
 *   Verify all built-in language packs register together and provide expected
 *   native/polyglot identities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_project/builtin_languages.h"

int main(void)
{
    UmiDeveloperProjectLanguageRegistry *registry = NULL;

    assert(umi_developer_project_builtin_language_count() == 18U);
    assert(umi_developer_project_language_registry_create(&registry) ==
           UMI_STATUS_OK);
    assert(umi_developer_project_builtin_languages_register(registry) ==
           UMI_STATUS_OK);
    assert(umi_developer_project_language_registry_count(registry) == 18U);

    assert(umi_developer_project_language_registry_find(
        registry, "developer.language.c23") != NULL);
    assert(umi_developer_project_language_registry_find(
        registry, "developer.language.cpp") != NULL);
    assert(umi_developer_project_language_registry_find(
        registry, "developer.language.assembly") != NULL);
    assert(umi_developer_project_language_registry_find(
        registry, "developer.language.rust") != NULL);
    assert(umi_developer_project_language_registry_find(
        registry, "developer.language.zig") != NULL);

    umi_developer_project_language_registry_destroy(registry);
    return 0;
}
