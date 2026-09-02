/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/context_link_policy.c
 *
 * PURPOSE:
 *   Define toolkit-neutral context link policy contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/context_link_policy.h"
#include <string.h>

/*
 * Initialise ui mosaic context link policy from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_mosaic_context_link_policy_init(UmiUiMosaicContextLinkPolicy *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->bidirectional = true;
}

/*
 * Copy ui mosaic context link policy into module-owned storage so callers keep ownership
 * of their input values.
 */
UmiStatus umi_ui_mosaic_context_link_policy_set(UmiUiMosaicContextLinkPolicy *value, const char *group_id, const char *context_type, const char *member_id) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_mosaic_copy_text(value->group_id, sizeof(value->group_id), group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_mosaic_copy_text(value->context_type, sizeof(value->context_type), context_type);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_mosaic_copy_text(value->member_id, sizeof(value->member_id), member_id);
}

/*
 * Check that ui mosaic context link policy satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_ui_mosaic_context_link_policy_validate(const UmiUiMosaicContextLinkPolicy *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_ui_mosaic_id_is_valid(value->group_id) || !umi_ui_mosaic_id_is_valid(value->context_type) || !umi_ui_mosaic_id_is_valid(value->member_id)) return UMI_STATUS_INVALID_STATE;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value->colour_index >= 16U) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
