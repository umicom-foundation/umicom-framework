/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/view_factory.c
 *
 * PURPOSE:
 *   Dispatch stable integration view IDs to their Framework factories.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/view_factory.h"

#include <string.h>

/*
 * Initialise ide view factory from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ide_view_factory_create(
    UmiIdeIntegrationPlatform *platform,
    const char *view_id,
    UmiUiViewModel **out_view)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || view_id == NULL || out_view == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(view_id, umi_ide_view_id_overview()) == 0)
        return umi_ide_overview_view_create(view_id, platform, out_view);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(view_id, umi_ide_view_id_active_context()) == 0)
        return umi_ide_active_context_view_create(view_id, platform, out_view);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(view_id, umi_ide_view_id_workflow()) == 0)
        return umi_ide_workflow_view_create(view_id, platform, out_view);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(view_id, umi_ide_view_id_inline_ai()) == 0)
        return umi_ide_inline_ai_view_create(view_id, platform, out_view);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(view_id, umi_ide_view_id_navigation()) == 0)
        return umi_ide_navigation_view_create(view_id, platform, out_view);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(view_id, umi_ide_view_id_surfaces()) == 0)
        return umi_ide_surfaces_view_create(view_id, platform, out_view);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(view_id, umi_ide_view_id_self_host()) == 0)
        return umi_ide_self_host_view_create(view_id, platform, out_view);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(view_id, umi_ide_view_id_cross_domain()) == 0)
        return umi_ide_cross_domain_view_create(view_id, platform, out_view);

    return UMI_STATUS_NOT_FOUND;
}
