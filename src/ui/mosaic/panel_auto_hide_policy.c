/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/panel_auto_hide_policy.c
 *
 * PURPOSE:
 *   Define toolkit-neutral panel auto hide policy contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/panel_auto_hide_policy.h"
#include <string.h>

/*
 * Initialise ui mosaic panel auto hide policy from caller-provided values so later
 * operations receive a known state.
 */
void umi_ui_mosaic_panel_auto_hide_policy_init(UmiUiMosaicPanelAutoHidePolicy *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->application = UMI_UI_MOSAIC_APP_FRAMEWORK;
    value->enabled = true;
}

/*
 * Copy ui mosaic panel auto hide policy into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_ui_mosaic_panel_auto_hide_policy_set(UmiUiMosaicPanelAutoHidePolicy *value, const char *id, const char *title) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_mosaic_copy_text(value->id, sizeof(value->id), id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_mosaic_copy_text(value->title, sizeof(value->title), title);
}

/*
 * Check that ui mosaic panel auto hide policy satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_ui_mosaic_panel_auto_hide_policy_validate(const UmiUiMosaicPanelAutoHidePolicy *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_ui_mosaic_id_is_valid(value->id) || value->title[0] == '\0') return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (value->application < UMI_UI_MOSAIC_APP_FRAMEWORK || value->application > UMI_UI_MOSAIC_APP_OS) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/*
 * Provide the ui mosaic panel auto hide policy rank operation used by this module and its
 * client applications.
 */
uint32_t umi_ui_mosaic_panel_auto_hide_policy_rank(const UmiUiMosaicPanelAutoHidePolicy *value, uint32_t context_boost) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || !value->enabled) return 0U;
    return value->priority + context_boost + 1U;
}
