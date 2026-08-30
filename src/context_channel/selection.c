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
void umi_selection_context_init(UmiSelectionContext *context)
{
    if (context == NULL) return;
    memset(context, 0, sizeof(*context));
    context->structure_size = (uint32_t)sizeof(*context);
    context->revision = 1U;
}
UmiStatus umi_selection_context_validate(const UmiSelectionContext *context)
{
    if (context == NULL || context->structure_size != sizeof(*context)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->selection_id, sizeof(context->selection_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->selection_type, sizeof(context->selection_type))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->primary_id, sizeof(context->primary_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->secondary_id, sizeof(context->secondary_id))) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_selection_context_copy(UmiSelectionContext *destination, const UmiSelectionContext *source)
{
    if (destination == NULL || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_selection_context_validate(source) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    *destination = *source;
    return UMI_STATUS_OK;
}
UmiStatus umi_selection_context_set_selection_id(UmiSelectionContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->selection_id, sizeof(context->selection_id), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
UmiStatus umi_selection_context_set_selection_type(UmiSelectionContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->selection_type, sizeof(context->selection_type), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
UmiStatus umi_selection_context_set_primary_id(UmiSelectionContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->primary_id, sizeof(context->primary_id), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
UmiStatus umi_selection_context_set_secondary_id(UmiSelectionContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->secondary_id, sizeof(context->secondary_id), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
UmiStatus umi_selection_context_set_index(UmiSelectionContext *context, uint64_t value)
{
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    context->index = value;
    context->revision += 1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_selection_context_set_count(UmiSelectionContext *context, uint64_t value)
{
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    context->count = value;
    context->revision += 1U;
    return UMI_STATUS_OK;
}
