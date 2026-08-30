/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/view_factory.c
 *
 * PURPOSE:
 *   Dispatch stable Studio runtime view IDs to Framework factories.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/view_factory.h"

#include <string.h>

UmiStatus umi_studio_view_factory_create(
    UmiStudioRuntimePlatform *platform,
    const char *view_id,
    const char *query,
    UmiUiViewModel **out_view)
{
    if (platform == NULL || view_id == NULL || out_view == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (strcmp(view_id, umi_studio_view_id_runtime_overview()) == 0)
        return umi_studio_runtime_overview_view_create(
            view_id, platform, out_view);
    if (strcmp(view_id, umi_studio_view_id_document_tabs()) == 0)
        return umi_studio_document_tabs_view_create(
            view_id, platform, out_view);
    if (strcmp(view_id, umi_studio_view_id_status()) == 0)
        return umi_studio_status_view_create(view_id, platform, out_view);
    if (strcmp(view_id, umi_studio_view_id_command_search()) == 0)
        return umi_studio_command_search_view_create(
            view_id, platform, query != NULL ? query : "", out_view);
    if (strcmp(view_id, umi_studio_view_id_layout_browser()) == 0)
        return umi_studio_layout_browser_view_create(
            view_id, platform, out_view);
    if (strcmp(view_id, umi_studio_view_id_close_guard()) == 0)
        return umi_studio_close_guard_view_create(
            view_id, platform, out_view);
    if (strcmp(view_id, umi_studio_view_id_contract()) == 0)
        return umi_studio_contract_view_create(
            view_id, platform, out_view);
    if (strcmp(view_id, umi_studio_view_id_selection()) == 0)
        return umi_studio_selection_view_create(
            view_id, platform, out_view);
    if (strcmp(view_id, umi_studio_view_id_shell_state()) == 0)
        return umi_studio_shell_state_view_create(
            view_id, platform, out_view);

    return UMI_STATUS_NOT_FOUND;
}
