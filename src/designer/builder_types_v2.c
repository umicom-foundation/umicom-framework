/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/builder_types_v2.c
 * PURPOSE: Implement stable Visual Application Builder v2 enum text.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/builder_types_v2.h"

const char *umi_designer_binding_mode_v2_text(UmiDesignerBindingModeV2 mode)
{
    switch (mode) {
        case UMI_DESIGNER_BIND_ONE_WAY: return "one-way";
        case UMI_DESIGNER_BIND_TWO_WAY: return "two-way";
        case UMI_DESIGNER_BIND_ONE_TIME: return "one-time";
        default: return "unknown";
    }
}

const char *umi_designer_interaction_kind_v2_text(UmiDesignerInteractionKindV2 kind)
{
    switch (kind) {
        case UMI_DESIGNER_INTERACTION_SIGNAL: return "signal";
        case UMI_DESIGNER_INTERACTION_ACTION: return "action";
        case UMI_DESIGNER_INTERACTION_EVENT: return "event";
        case UMI_DESIGNER_INTERACTION_COMMAND: return "command";
        default: return "unknown";
    }
}

const char *umi_designer_drop_position_v2_text(UmiDesignerDropPositionV2 position)
{
    switch (position) {
        case UMI_DESIGNER_DROP_INTO: return "into";
        case UMI_DESIGNER_DROP_BEFORE: return "before";
        case UMI_DESIGNER_DROP_AFTER: return "after";
        default: return "unknown";
    }
}

const char *umi_designer_transaction_state_v2_text(
    UmiDesignerTransactionStateV2 state)
{
    switch (state) {
        case UMI_DESIGNER_TRANSACTION_DRAFT: return "draft";
        case UMI_DESIGNER_TRANSACTION_APPLIED: return "applied";
        case UMI_DESIGNER_TRANSACTION_UNDONE: return "undone";
        case UMI_DESIGNER_TRANSACTION_FAILED: return "failed";
        default: return "unknown";
    }
}

const char *umi_designer_preview_health_v2_text(UmiDesignerPreviewHealthV2 health)
{
    switch (health) {
        case UMI_DESIGNER_PREVIEW_IDLE: return "idle";
        case UMI_DESIGNER_PREVIEW_CURRENT: return "current";
        case UMI_DESIGNER_PREVIEW_STALE: return "stale";
        case UMI_DESIGNER_PREVIEW_INVALID: return "invalid";
        default: return "unknown";
    }
}
