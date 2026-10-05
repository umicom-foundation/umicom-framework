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
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise media context from caller-provided values so later operations receive a known
 * state.
 */
void umi_media_context_init(UmiMediaContext *context)
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
/* Check that media context satisfies its contract before another service relies on it. */
UmiStatus umi_media_context_validate(const UmiMediaContext *context)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->asset_id, '\0', sizeof(context->asset_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->timeline_id, '\0', sizeof(context->timeline_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->track_id, '\0', sizeof(context->track_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(context->media_type, '\0', sizeof(context->media_type)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || context->structure_size != sizeof(*context)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->asset_id, sizeof(context->asset_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->timeline_id, sizeof(context->timeline_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->track_id, sizeof(context->track_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(context->media_type, sizeof(context->media_type))) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Copy media context into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_media_context_copy(UmiMediaContext *destination, const UmiMediaContext *source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_media_context_validate(source) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    *destination = *source;
    return UMI_STATUS_OK;
}
/*
 * Provide the media context set asset id operation used by this module and its client
 * applications.
 */
UmiStatus umi_media_context_set_asset_id(UmiMediaContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->asset_id, sizeof(context->asset_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the media context set timeline id operation used by this module and its client
 * applications.
 */
UmiStatus umi_media_context_set_timeline_id(UmiMediaContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->timeline_id, sizeof(context->timeline_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the media context set track id operation used by this module and its client
 * applications.
 */
UmiStatus umi_media_context_set_track_id(UmiMediaContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->track_id, sizeof(context->track_id), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
/*
 * Provide the media context set timecode ms operation used by this module and its client
 * applications.
 */
UmiStatus umi_media_context_set_timecode_ms(UmiMediaContext *context, uint64_t value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    context->timecode_ms = value;
    context->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the media context set duration ms operation used by this module and its client
 * applications.
 */
UmiStatus umi_media_context_set_duration_ms(UmiMediaContext *context, uint64_t value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    context->duration_ms = value;
    context->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the media context set media type operation used by this module and its client
 * applications.
 */
UmiStatus umi_media_context_set_media_type(UmiMediaContext *context, const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->media_type, sizeof(context->media_type), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiMediaContextArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7476630c8606db57);
    schema = (schema ^ (uint64_t)sizeof(((UmiMediaContext *)0)->asset_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiMediaContext *)0)->timeline_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiMediaContext *)0)->track_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiMediaContext *)0)->media_type)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiMediaContextArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiMediaContext *)0)->asset_id) - 1U +
        8U + sizeof(((UmiMediaContext *)0)->timeline_id) - 1U +
        8U + sizeof(((UmiMediaContext *)0)->track_id) - 1U +
        8U +
        8U +
        8U + sizeof(((UmiMediaContext *)0)->media_type) - 1U +
        8U;
}
static void UmiMediaContextArchiveWrite(UmiArchiveWriter *writer, const UmiMediaContext *value)
{
    UmiArchiveWriteText(writer, value->asset_id, sizeof(value->asset_id));
    UmiArchiveWriteText(writer, value->timeline_id, sizeof(value->timeline_id));
    UmiArchiveWriteText(writer, value->track_id, sizeof(value->track_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->timecode_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->duration_ms);
    UmiArchiveWriteText(writer, value->media_type, sizeof(value->media_type));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiMediaContextArchiveRead(UmiArchiveReader *reader, UmiMediaContext *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->asset_id, sizeof(value->asset_id));
    UmiArchiveReadText(reader, value->timeline_id, sizeof(value->timeline_id));
    UmiArchiveReadText(reader, value->track_id, sizeof(value->track_id));
    value->timecode_ms = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->duration_ms = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    UmiArchiveReadText(reader, value->media_type, sizeof(value->media_type));
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiMediaContextArchiveValidate(const UmiMediaContext *value)
{
    return umi_media_context_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_media_context_archive_encode, umi_media_context_archive_decode,
    UmiMediaContext, UmiMediaContextArchiveSchema, UmiMediaContextArchiveBound, UmiMediaContextArchiveWrite, UmiMediaContextArchiveRead, UmiMediaContextArchiveValidate)
