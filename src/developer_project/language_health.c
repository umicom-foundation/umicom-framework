/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/language_health.c
 *
 * PURPOSE:
 *   Probe advertised language tools through the reusable Framework PATH lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/language_health.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the developer project language health probe operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_project_language_health_probe(
    const UmiDeveloperProjectLanguagePack *pack,
    UmiDeveloperProjectLanguageHealth *out_health)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pack == NULL || out_health == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_project_language_pack_validate(pack);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)memset(out_health, 0, sizeof(*out_health));
    (void)snprintf(
        out_health->language_id,
        sizeof(out_health->language_id),
        "%s",
        pack->language_id);

    out_health->tool_count = pack->build_tool_count;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < pack->build_tool_count; ++index) {
        UmiDeveloperProjectLanguageToolHealth *tool =
            &out_health->tools[index];

        (void)snprintf(tool->tool, sizeof(tool->tool), "%s",
                       pack->build_tools[index]);

        status = umi_toolchain_find_on_path(
            pack->build_tools[index],
            tool->path,
            sizeof(tool->path));

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            tool->available = 1;
            out_health->available_count += 1U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            out_health->missing_count += 1U;
        }
    }

    /*
     * Packs with no external build tool (HTML/CSS/Markdown/JSON/YAML/SQL) are
     * ready for editor/language features immediately. Packs with tool
     * requirements are considered operational only when each advertised tool is
     * currently discoverable.
     */
    out_health->ready =
        pack->build_tool_count == 0U ||
        out_health->missing_count == 0U;
    return UMI_STATUS_OK;
}
