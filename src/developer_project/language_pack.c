/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/language_pack.c
 *
 * PURPOSE:
 *   Validate language-pack metadata and extension/capability queries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/language_pack.h"

#include <string.h>

/*
 * Check that developer project language pack satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_developer_project_language_pack_validate(
    const UmiDeveloperProjectLanguagePack *pack)
{
    size_t index;
    size_t other;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pack == NULL ||
        pack->structure_size != sizeof(*pack) ||
        pack->api_version != UMI_DEVELOPER_PROJECT_API_VERSION ||
        pack->language_id == NULL || pack->language_id[0] == '\0' ||
        pack->title == NULL || pack->title[0] == '\0' ||
        pack->description == NULL ||
        pack->extensions == NULL || pack->extension_count == 0U ||
        pack->extension_count >
            UMI_DEVELOPER_PROJECT_LANGUAGE_EXTENSION_CAPACITY ||
        pack->build_tool_count >
            UMI_DEVELOPER_PROJECT_LANGUAGE_TOOL_CAPACITY ||
        (pack->build_tool_count > 0U && pack->build_tools == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < pack->extension_count; ++index) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (pack->extensions[index] == NULL ||
            pack->extensions[index][0] == '\0') {
            return UMI_STATUS_INVALID_ARGUMENT;
        }

        /* Visit each bounded item once so every record receives the same rule. */
        for (other = index + 1U; other < pack->extension_count; ++other) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (strcmp(pack->extensions[index],
                       pack->extensions[other]) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the developer project language pack supports extension operation used by this
 * module and its client applications.
 */
int umi_developer_project_language_pack_supports_extension(
    const UmiDeveloperProjectLanguagePack *pack,
    const char *extension)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pack == NULL || extension == NULL) return 0;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < pack->extension_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(pack->extensions[index], extension) == 0) {
            return 1;
        }
    }

    return 0;
}

/*
 * Provide the developer project language pack has capability operation used by this module
 * and its client applications.
 */
int umi_developer_project_language_pack_has_capability(
    const UmiDeveloperProjectLanguagePack *pack,
    UmiDeveloperProjectLanguageCapability capability)
{
    return pack != NULL &&
        (pack->capabilities & (UmiDeveloperProjectLanguageCapabilities)capability)
            != 0U;
}
