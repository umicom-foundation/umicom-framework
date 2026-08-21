/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/source_location.c
 *
 * PURPOSE:
 *   Implement canonical source location context validation and mutation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/source_location.h"
#include <string.h>
void umi_source_location_context_init(UmiSourceLocationContext *context)
{
    if (context == NULL) return;
    memset(context, 0, sizeof(*context));
    context->structure_size = (uint32_t)sizeof(*context);
    context->revision = 1U;
}
UmiStatus umi_source_location_context_validate(const UmiSourceLocationContext *context)
{
    if (context == NULL || context->structure_size != sizeof(*context)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->workspace_id, sizeof(context->workspace_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->file_path, sizeof(context->file_path))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->symbol, sizeof(context->symbol))) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_source_location_context_copy(UmiSourceLocationContext *destination, const UmiSourceLocationContext *source)
{
    if (destination == NULL || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_source_location_context_validate(source) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    *destination = *source;
    return UMI_STATUS_OK;
}
UmiStatus umi_source_location_context_set_workspace_id(UmiSourceLocationContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->workspace_id, sizeof(context->workspace_id), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
UmiStatus umi_source_location_context_set_file_path(UmiSourceLocationContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->file_path, sizeof(context->file_path), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
UmiStatus umi_source_location_context_set_symbol(UmiSourceLocationContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->symbol, sizeof(context->symbol), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
UmiStatus umi_source_location_context_set_line(UmiSourceLocationContext *context, uint32_t value)
{
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    context->line = value;
    context->revision += 1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_source_location_context_set_column(UmiSourceLocationContext *context, uint32_t value)
{
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    context->column = value;
    context->revision += 1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_source_location_context_set_selection_length(UmiSourceLocationContext *context, uint32_t value)
{
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    context->selection_length = value;
    context->revision += 1U;
    return UMI_STATUS_OK;
}
