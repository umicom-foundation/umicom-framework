/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/workspace_catalogue.c
 *
 * PURPOSE:
 *   Define toolkit-neutral workspace catalogue contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/workspace_catalogue.h"
#include <string.h>

/*
 * Initialise ui mosaic workspace catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_mosaic_workspace_catalogue_init(UmiUiMosaicWorkspaceCatalogue *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->application = UMI_UI_MOSAIC_APP_FRAMEWORK;
    value->available = true;
}

/*
 * Copy ui mosaic workspace catalogue into module-owned storage so callers keep ownership
 * of their input values.
 */
UmiStatus umi_ui_mosaic_workspace_catalogue_set(UmiUiMosaicWorkspaceCatalogue *value, const char *id, const char *name, const char *layout_id, UmiUiMosaicApplication application) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_mosaic_copy_text(value->id, sizeof(value->id), id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_mosaic_copy_text(value->name, sizeof(value->name), name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_mosaic_copy_text(value->layout_id, sizeof(value->layout_id), layout_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->application = application;
    return UMI_STATUS_OK;
}

/*
 * Check that ui mosaic workspace catalogue satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_ui_mosaic_workspace_catalogue_validate(const UmiUiMosaicWorkspaceCatalogue *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!value->available || !umi_ui_mosaic_id_is_valid(value->id) || !umi_ui_mosaic_id_is_valid(value->layout_id) || value->name[0] == '\0') return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (value->application < UMI_UI_MOSAIC_APP_FRAMEWORK || value->application > UMI_UI_MOSAIC_APP_OS) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
