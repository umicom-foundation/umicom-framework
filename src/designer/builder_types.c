/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/builder_types.c
 * PURPOSE: Implement stable Visual Application Builder v2 enum text.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/builder_types.h"

/*
 * Provide the designer binding mode text operation used by this module and its client
 * applications.
 */
const char *umi_designer_binding_mode_text(UmiDesignerBindingMode mode)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (mode) {
        case UMI_DESIGNER_BIND_ONE_WAY: return "one-way";
        case UMI_DESIGNER_BIND_TWO_WAY: return "two-way";
        case UMI_DESIGNER_BIND_ONE_TIME: return "one-time";
        default: return "unknown";
    }
}

/*
 * Provide the designer interaction kind text operation used by this module and its client
 * applications.
 */
const char *umi_designer_interaction_kind_text(UmiDesignerInteractionKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_DESIGNER_INTERACTION_SIGNAL: return "signal";
        case UMI_DESIGNER_INTERACTION_ACTION: return "action";
        case UMI_DESIGNER_INTERACTION_EVENT: return "event";
        case UMI_DESIGNER_INTERACTION_COMMAND: return "command";
        default: return "unknown";
    }
}

/*
 * Provide the designer drop position text operation used by this module and its client
 * applications.
 */
const char *umi_designer_drop_position_text(UmiDesignerDropPosition position)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (position) {
        case UMI_DESIGNER_DROP_INTO: return "into";
        case UMI_DESIGNER_DROP_BEFORE: return "before";
        case UMI_DESIGNER_DROP_AFTER: return "after";
        default: return "unknown";
    }
}

/*
 * Provide the designer transaction state text operation used by this module and its client
 * applications.
 */
const char *umi_designer_transaction_state_text(
    UmiDesignerTransactionState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
        case UMI_DESIGNER_TRANSACTION_DRAFT: return "draft";
        case UMI_DESIGNER_TRANSACTION_APPLIED: return "applied";
        case UMI_DESIGNER_TRANSACTION_UNDONE: return "undone";
        case UMI_DESIGNER_TRANSACTION_FAILED: return "failed";
        default: return "unknown";
    }
}

/*
 * Provide the designer preview health text operation used by this module and its client
 * applications.
 */
const char *umi_designer_preview_health_text(UmiDesignerPreviewHealth health)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (health) {
        case UMI_DESIGNER_PREVIEW_IDLE: return "idle";
        case UMI_DESIGNER_PREVIEW_CURRENT: return "current";
        case UMI_DESIGNER_PREVIEW_STALE: return "stale";
        case UMI_DESIGNER_PREVIEW_INVALID: return "invalid";
        default: return "unknown";
    }
}
