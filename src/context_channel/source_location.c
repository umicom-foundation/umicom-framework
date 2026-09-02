/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/source_location.c
 *
 * PURPOSE:
 *   Implement canonical source location context validation and mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/source_location.h"
#include <string.h>
/*
 * Initialise source location context from caller-provided values so later operations
 * receive a known state.
 */
void umi_source_location_context_init(UmiSourceLocationContext *context)
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
/*
 * Check that source location context satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_source_location_context_validate(const UmiSourceLocationContext *context)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || context->structure_size != sizeof(*context)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->workspace_id, sizeof(context->workspace_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->file_path, sizeof(context->file_path))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->symbol, sizeof(context->symbol))) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Copy source location context into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_source_location_context_copy(UmiSourceLocationContext *destination, const UmiSourceLocationContext *source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_source_location_context_validate(source) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    *destination = *source;
    return UMI_STATUS_OK;
}
/*
 * Provide the source location context set workspace id operation used by this module and
 * its client applications.
 */
UmiStatus umi_source_location_context_set_workspace_id(UmiSourceLocationContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->workspace_id, sizeof(context->workspace_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the source location context set file path operation used by this module and its
 * client applications.
 */
UmiStatus umi_source_location_context_set_file_path(UmiSourceLocationContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->file_path, sizeof(context->file_path), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the source location context set symbol operation used by this module and its
 * client applications.
 */
UmiStatus umi_source_location_context_set_symbol(UmiSourceLocationContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->symbol, sizeof(context->symbol), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the source location context set line operation used by this module and its
 * client applications.
 */
UmiStatus umi_source_location_context_set_line(UmiSourceLocationContext *context, uint32_t value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    context->line = value;
    context->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the source location context set column operation used by this module and its
 * client applications.
 */
UmiStatus umi_source_location_context_set_column(UmiSourceLocationContext *context, uint32_t value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    context->column = value;
    context->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the source location context set selection length operation used by this module
 * and its client applications.
 */
UmiStatus umi_source_location_context_set_selection_length(UmiSourceLocationContext *context, uint32_t value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    context->selection_length = value;
    context->revision += 1U;
    return UMI_STATUS_OK;
}
