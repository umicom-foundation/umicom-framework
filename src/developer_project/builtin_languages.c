/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/builtin_languages.c
 *
 * PURPOSE:
 *   Enumerate and install every built-in developer language pack.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/builtin_languages.h"

#include "umicom/developer_project/languages/languages.h"

typedef const UmiDeveloperProjectLanguagePack *(*LanguageFactory)(void);

static const LanguageFactory FACTORIES[] = {
    umi_developer_project_language_c23,
    umi_developer_project_language_cpp,
    umi_developer_project_language_assembly,
    umi_developer_project_language_cpython,
    umi_developer_project_language_cmake,
    umi_developer_project_language_bash,
    umi_developer_project_language_zig,
    umi_developer_project_language_rust,
    umi_developer_project_language_java,
    umi_developer_project_language_php,
    umi_developer_project_language_javascript,
    umi_developer_project_language_typescript,
    umi_developer_project_language_html,
    umi_developer_project_language_css,
    umi_developer_project_language_markdown,
    umi_developer_project_language_json,
    umi_developer_project_language_yaml,
    umi_developer_project_language_sql
};

size_t umi_developer_project_builtin_language_count(void)
{
    return sizeof(FACTORIES) / sizeof(FACTORIES[0]);
}

const UmiDeveloperProjectLanguagePack *
umi_developer_project_builtin_language_at(size_t index)
{
    return index < umi_developer_project_builtin_language_count()
        ? FACTORIES[index]()
        : NULL;
}

UmiStatus umi_developer_project_builtin_languages_register(
    UmiDeveloperProjectLanguageRegistry *registry)
{
    size_t index;

    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    for (index = 0U;
         index < umi_developer_project_builtin_language_count();
         ++index) {
        const UmiStatus status =
            umi_developer_project_language_registry_register(
                registry, FACTORIES[index]());
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
