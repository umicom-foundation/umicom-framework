/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/context_link_member.c
 *
 * PURPOSE:
 *   Define toolkit-neutral context link member contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/context_link_member.h"
#include <string.h>

void umi_ui_mosaic_context_link_member_init(UmiUiMosaicContextLinkMember *value) {
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->bidirectional = true;
}

UmiStatus umi_ui_mosaic_context_link_member_set(UmiUiMosaicContextLinkMember *value, const char *group_id, const char *context_type, const char *member_id) {
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_mosaic_copy_text(value->group_id, sizeof(value->group_id), group_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_mosaic_copy_text(value->context_type, sizeof(value->context_type), context_type);
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_mosaic_copy_text(value->member_id, sizeof(value->member_id), member_id);
}

UmiStatus umi_ui_mosaic_context_link_member_validate(const UmiUiMosaicContextLinkMember *value) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_ui_mosaic_id_is_valid(value->group_id) || !umi_ui_mosaic_id_is_valid(value->context_type) || !umi_ui_mosaic_id_is_valid(value->member_id)) return UMI_STATUS_INVALID_STATE;
    if (value->colour_index >= 16U) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
