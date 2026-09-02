/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/builtin_languages.c
 *
 * PURPOSE:
 *   Enumerate and install every built-in developer language pack.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/*
 * Return the number of records represented by developer project builtin language without
 * changing their state.
 */
size_t umi_developer_project_builtin_language_count(void)
{
    return sizeof(FACTORIES) / sizeof(FACTORIES[0]);
}

/*
 * Find developer project builtin language while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDeveloperProjectLanguagePack *
umi_developer_project_builtin_language_at(size_t index)
{
    return index < umi_developer_project_builtin_language_count()
        ? FACTORIES[index]()
        : NULL;
}

/*
 * Add developer project builtin languages only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_developer_project_builtin_languages_register(
    UmiDeveloperProjectLanguageRegistry *registry)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_developer_project_builtin_language_count();
         ++index) {
        const UmiStatus status =
            umi_developer_project_language_registry_register(
                registry, FACTORIES[index]());
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
