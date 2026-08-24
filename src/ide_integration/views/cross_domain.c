/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/views/cross_domain.c
 *
 * PURPOSE:
 *   Project cross-domain service availability and navigation actions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/views/cross_domain.h"

UmiStatus umi_ide_cross_domain_view_create(
    const char *view_id,
    UmiIdeIntegrationPlatform *platform,
    UmiUiViewModel **out_view)
{
    UmiIdeIntegrationBindings *bindings;
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    bindings = umi_ide_integration_platform_bindings(platform);
    if (bindings == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_ide_view_create_base(
        view_id,
        "ide-cross-domain",
        "Cross-Domain Actions",
        "Navigate between Problems, Tests, Source Control, Debug, language intelligence and AI.",
        out_view);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ide_view_set_boolean(
        *out_view, "ide.cross.problems", bindings->problems != NULL);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_boolean(
            *out_view, "ide.cross.tests", bindings->tests != NULL);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_boolean(
            *out_view, "ide.cross.source-control",
            bindings->source_control != NULL);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_boolean(
            *out_view, "ide.cross.debug", bindings->debug_runtime != NULL);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_boolean(
            *out_view, "ide.cross.language", bindings->language != NULL);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_boolean(
            *out_view, "ide.cross.ai", bindings->ai_developer != NULL);

    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_action(
            *out_view, 0U, "ide.open.problem",
            "Open Problem", "Navigate to the selected Problem.",
            bindings->problems != NULL);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_action(
            *out_view, 1U, "ide.open.test",
            "Open Test", "Navigate to the selected test.",
            bindings->tests != NULL);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_action(
            *out_view, 2U, "ide.open.symbol",
            "Open Symbol", "Navigate to the selected language symbol.",
            bindings->language != NULL);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_action(
            *out_view, 3U, "ide.open.debug-frame",
            "Open Debug Frame", "Navigate to the active debugger frame.",
            bindings->debug_runtime != NULL);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_action(
            *out_view, 4U, "ide.ai.fix-problem",
            "Fix with AI", "Prepare a governed repair for the selected Problem.",
            bindings->ai_developer != NULL && bindings->problems != NULL);

    return status;
}
