/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/media.c
 *
 * PURPOSE:
 *   Implement canonical media context validation and mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/media.h"
#include <string.h>
void umi_media_context_init(UmiMediaContext *context)
{
    if (context == NULL) return;
    memset(context, 0, sizeof(*context));
    context->structure_size = (uint32_t)sizeof(*context);
    context->revision = 1U;
}
UmiStatus umi_media_context_validate(const UmiMediaContext *context)
{
    if (context == NULL || context->structure_size != sizeof(*context)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->asset_id, sizeof(context->asset_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->timeline_id, sizeof(context->timeline_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->track_id, sizeof(context->track_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->media_type, sizeof(context->media_type))) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_media_context_copy(UmiMediaContext *destination, const UmiMediaContext *source)
{
    if (destination == NULL || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_media_context_validate(source) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    *destination = *source;
    return UMI_STATUS_OK;
}
UmiStatus umi_media_context_set_asset_id(UmiMediaContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->asset_id, sizeof(context->asset_id), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
UmiStatus umi_media_context_set_timeline_id(UmiMediaContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->timeline_id, sizeof(context->timeline_id), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
UmiStatus umi_media_context_set_track_id(UmiMediaContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->track_id, sizeof(context->track_id), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
UmiStatus umi_media_context_set_timecode_ms(UmiMediaContext *context, uint64_t value)
{
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    context->timecode_ms = value;
    context->revision += 1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_media_context_set_duration_ms(UmiMediaContext *context, uint64_t value)
{
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    context->duration_ms = value;
    context->revision += 1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_media_context_set_media_type(UmiMediaContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->media_type, sizeof(context->media_type), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
