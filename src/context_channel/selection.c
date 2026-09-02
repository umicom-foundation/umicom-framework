/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/selection.c
 *
 * PURPOSE:
 *   Implement canonical selection context validation and mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/selection.h"
#include <string.h>
/*
 * Initialise selection context from caller-provided values so later operations receive a
 * known state.
 */
void umi_selection_context_init(UmiSelectionContext *context)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL) return;
    memset(context, 0, sizeof(*context));
    context->structure_size = (uint32_t)sizeof(*context);
    context->revision = 1U;
}
/* Check that selection context satisfies its contract before another service relies on it. */
UmiStatus umi_selection_context_validate(const UmiSelectionContext *context)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || context->structure_size != sizeof(*context)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->selection_id, sizeof(context->selection_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->selection_type, sizeof(context->selection_type))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->primary_id, sizeof(context->primary_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->secondary_id, sizeof(context->secondary_id))) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Copy selection context into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_selection_context_copy(UmiSelectionContext *destination, const UmiSelectionContext *source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_selection_context_validate(source) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    *destination = *source;
    return UMI_STATUS_OK;
}
/*
 * Provide the selection context set selection id operation used by this module and its
 * client applications.
 */
UmiStatus umi_selection_context_set_selection_id(UmiSelectionContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->selection_id, sizeof(context->selection_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the selection context set selection type operation used by this module and its
 * client applications.
 */
UmiStatus umi_selection_context_set_selection_type(UmiSelectionContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->selection_type, sizeof(context->selection_type), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the selection context set primary id operation used by this module and its
 * client applications.
 */
UmiStatus umi_selection_context_set_primary_id(UmiSelectionContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->primary_id, sizeof(context->primary_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the selection context set secondary id operation used by this module and its
 * client applications.
 */
UmiStatus umi_selection_context_set_secondary_id(UmiSelectionContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->secondary_id, sizeof(context->secondary_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the selection context set index operation used by this module and its client
 * applications.
 */
UmiStatus umi_selection_context_set_index(UmiSelectionContext *context, uint64_t value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    context->index = value;
    context->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Return the number of records represented by selection context set without changing their
 * state.
 */
UmiStatus umi_selection_context_set_count(UmiSelectionContext *context, uint64_t value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    context->count = value;
    context->revision += 1U;
    return UMI_STATUS_OK;
}
