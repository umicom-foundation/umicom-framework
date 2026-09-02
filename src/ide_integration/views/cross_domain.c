/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/views/cross_domain.c
 *
 * PURPOSE:
 *   Project cross-domain service availability and navigation actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/views/cross_domain.h"

/*
 * Initialise ide cross domain view from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ide_cross_domain_view_create(
    const char *view_id,
    UmiIdeIntegrationPlatform *platform,
    UmiUiViewModel **out_view)
{
    UmiIdeIntegrationBindings *bindings;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    bindings = umi_ide_integration_platform_bindings(platform);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_ide_view_create_base(
        view_id,
        "ide-cross-domain",
        "Cross-Domain Actions",
        "Navigate between Problems, Tests, Source Control, Debug, language intelligence and AI.",
        out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ide_view_set_boolean(
        *out_view, "ide.cross.problems", bindings->problems != NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_boolean(
            *out_view, "ide.cross.tests", bindings->tests != NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_boolean(
            *out_view, "ide.cross.source-control",
            bindings->source_control != NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_boolean(
            *out_view, "ide.cross.debug", bindings->debug_runtime != NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_boolean(
            *out_view, "ide.cross.language", bindings->language != NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_boolean(
            *out_view, "ide.cross.ai", bindings->ai_developer != NULL);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_action(
            *out_view, 0U, "ide.open.problem",
            "Open Problem", "Navigate to the selected Problem.",
            bindings->problems != NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_action(
            *out_view, 1U, "ide.open.test",
            "Open Test", "Navigate to the selected test.",
            bindings->tests != NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_action(
            *out_view, 2U, "ide.open.symbol",
            "Open Symbol", "Navigate to the selected language symbol.",
            bindings->language != NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_action(
            *out_view, 3U, "ide.open.debug-frame",
            "Open Debug Frame", "Navigate to the active debugger frame.",
            bindings->debug_runtime != NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_action(
            *out_view, 4U, "ide.ai.fix-problem",
            "Fix with AI", "Prepare a governed repair for the selected Problem.",
            bindings->ai_developer != NULL && bindings->problems != NULL);

    return status;
}
