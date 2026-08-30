/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/views/navigation.c
 *
 * PURPOSE:
 *   Project cross-domain navigation history and back/forward actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/views/navigation.h"

#include <stdio.h>

UmiStatus umi_ide_navigation_view_create(
    const char *view_id,
    UmiIdeIntegrationPlatform *platform,
    UmiUiViewModel **out_view)
{
    UmiIdeCrossNavigation *navigation;
    UmiIdeNavigationTarget current;
    size_t count;
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    navigation = umi_ide_integration_platform_navigation(platform);
    if (navigation == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_ide_view_create_base(
        view_id,
        "ide-navigation",
        "Navigation History",
        "Cross-domain history shared by Problems, Tests, Source Control, Debug, language and AI.",
        out_view);
    if (status != UMI_STATUS_OK) return status;

    count = umi_ide_navigation_history_count(navigation->history);

    status = umi_ide_view_set_integer(
        *out_view, "ide.navigation.count", (int64_t)count);

    if (status == UMI_STATUS_OK &&
        umi_ide_navigation_history_current(
            navigation->history,
            &current) == UMI_STATUS_OK) {
        char text[512];

        (void)snprintf(
            text, sizeof(text),
            "%s | %s | %.240s | %.180s:%u:%u",
            umi_ide_domain_text(current.domain),
            umi_ide_navigation_reason_text(current.reason),
            current.label,
            current.location.has_path
                ? current.location.path
                : current.location.uri,
            current.location.line,
            current.location.column);

        status = umi_ide_view_set_string(
            *out_view, "ide.navigation.current", text);
    }

    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_action(
            *out_view, 0U, "ide.navigate.back",
            "Back", "Navigate to the previous cross-domain location.",
            count > 1U);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_action(
            *out_view, 1U, "ide.navigate.forward",
            "Forward", "Navigate to the next cross-domain location.",
            count > 1U);

    return status;
}
