/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/audio_arrangement_document.c
 * PURPOSE: Persist editable audio placements as strict portable JSON without external file access.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/creative_workspace/audio_arrangement.h"
#include "umicom/ai/mcp/json.h"
#include "umicom/language_runtime/json_tree.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct Writer
{
    char *bytes;
    size_t used;
    UmiStatus status;
} Writer;
static void Append(Writer *writer, const char *format, ...)
{
    if (writer->status != UMI_STATUS_OK)
        return;
    va_list arguments;
    va_start(arguments, format);
    size_t room = UMI_CREATIVE_AUDIO_ARRANGEMENT_DOCUMENT_BYTES - writer->used;
    int count = vsnprintf(writer->bytes + writer->used, room, format, arguments);
    va_end(arguments);
    if (count < 0 || (size_t)count >= room)
        writer->status = UMI_STATUS_CAPACITY_EXCEEDED;
    else
        writer->used += (size_t)count;
}
static void Quoted(Writer *writer, const char *text)
{
    if (writer->status != UMI_STATUS_OK)
        return;
    writer->status = umi_ai_mcp_json_escape_string(
        text, writer->bytes + writer->used, UMI_CREATIVE_AUDIO_ARRANGEMENT_DOCUMENT_BYTES - writer->used);
    if (writer->status == UMI_STATUS_OK)
        writer->used += strlen(writer->bytes + writer->used);
}
UmiStatus UmiCreativeAudioArrangementExport(const UmiCreativeAudioArrangement *plan,
                                            const UmiCancellationToken *cancel, UmiCreativeAsset **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (*out != NULL)
        return UMI_STATUS_INVALID_STATE;
    UmiStatus status = UmiCreativeAudioArrangementValidate(plan);
    if (status != UMI_STATUS_OK)
        return status;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    Writer writer = {calloc(UMI_CREATIVE_AUDIO_ARRANGEMENT_DOCUMENT_BYTES, 1U), 0U, UMI_STATUS_OK};
    if (writer.bytes == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    Append(&writer, "{\"format\":\"umicom-audio-arrangement\",\"title\":");
    Quoted(&writer, plan->title);
    Append(&writer, ",\"sample_rate\":%u,\"clips\":[", plan->sample_rate);
    for (size_t i = 0U; i < plan->clip_count; ++i)
    {
        const UmiCreativeAudioPlacement *clip = &plan->clips[i];
        Append(&writer, "%s{\"id\":", i == 0U ? "" : ",");
        Quoted(&writer, clip->id);
        Append(&writer, ",\"asset_id\":");
        Quoted(&writer, clip->asset_id);
        Append(&writer,
               ",\"start_ms\":%u,\"source_begin_ms\":%u,\"source_end_ms\":%u,\"gain_permille\":%u,\"fade_in_"
               "ms\":%u,\"fade_out_ms\":%u}",
               clip->start_ms, clip->source_begin_ms, clip->source_end_ms, clip->gain_permille,
               clip->fade_in_ms, clip->fade_out_ms);
    }
    Append(&writer, "]}\n");
    status = writer.status;
    if (status == UMI_STATUS_OK)
        status = UmiCreativeAssetCapture(plan->title, UMI_CREATIVE_ASSET_DOCUMENT, writer.bytes, writer.used,
                                         cancel, out);
    free(writer.bytes);
    return status;
}
static UmiStatus Text(const UmiJsonTree *tree, int object, const char *key, char *out, size_t capacity)
{
    int node = -1;
    UmiStatus status = UmiJsonTreeMember(tree, object, key, &node);
    return status == UMI_STATUS_OK ? UmiJsonTreeText(tree, node, out, capacity) : status;
}
static UmiStatus Number(const UmiJsonTree *tree, int object, const char *key, uint32_t *out)
{
    int node = -1;
    int64_t value = 0;
    UmiStatus status = UmiJsonTreeMember(tree, object, key, &node);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeInteger(tree, node, &value);
    if (status == UMI_STATUS_OK && (value < 0 || value > UINT32_MAX))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        *out = (uint32_t)value;
    return status;
}
static UmiStatus Placement(const UmiJsonTree *tree, int object, UmiCreativeAudioPlacement *out)
{
    if (UmiJsonTreeKind(tree, object) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT ||
        UmiJsonTreeCount(tree, object) != 8U)
        return UMI_STATUS_PARSE_ERROR;
    UmiStatus status = Text(tree, object, "id", out->id, sizeof(out->id));
    if (status == UMI_STATUS_OK)
        status = Text(tree, object, "asset_id", out->asset_id, sizeof(out->asset_id));
    if (status == UMI_STATUS_OK)
        status = Number(tree, object, "start_ms", &out->start_ms);
    if (status == UMI_STATUS_OK)
        status = Number(tree, object, "source_begin_ms", &out->source_begin_ms);
    if (status == UMI_STATUS_OK)
        status = Number(tree, object, "source_end_ms", &out->source_end_ms);
    uint32_t gain = 0U;
    if (status == UMI_STATUS_OK)
        status = Number(tree, object, "gain_permille", &gain);
    if (status == UMI_STATUS_OK && gain > 1000U)
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK)
        out->gain_permille = (unsigned)gain;
    if (status == UMI_STATUS_OK)
        status = Number(tree, object, "fade_in_ms", &out->fade_in_ms);
    if (status == UMI_STATUS_OK)
        status = Number(tree, object, "fade_out_ms", &out->fade_out_ms);
    return status;
}
UmiStatus UmiCreativeAudioArrangementImport(const void *bytes, size_t size,
                                            const UmiCancellationToken *cancel,
                                            UmiCreativeAudioArrangement *out)
{
    if (bytes == NULL || out == NULL || size == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_CREATIVE_AUDIO_ARRANGEMENT_DOCUMENT_BYTES, 512U, 4U};
    UmiStatus status = UmiJsonTreeCreate(bytes, size, &limits, cancel, &tree);
    UmiCreativeAudioArrangement plan = {0};
    char format[64];
    int clips = -1;
    /* Exact required members prevent accepting a newer recipe while silently
     * throwing away unknown operations. Each lookup also rejects duplicate keys. */
    if (status == UMI_STATUS_OK &&
        (UmiJsonTreeKind(tree, 0) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT || UmiJsonTreeCount(tree, 0) != 4U))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = Text(tree, 0, "format", format, sizeof(format));
    if (status == UMI_STATUS_OK && strcmp(format, "umicom-audio-arrangement") != 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = Text(tree, 0, "title", plan.title, sizeof(plan.title));
    if (status == UMI_STATUS_OK)
        status = Number(tree, 0, "sample_rate", &plan.sample_rate);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeMember(tree, 0, "clips", &clips);
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, clips) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK && UmiJsonTreeCount(tree, clips) > UMI_CREATIVE_AUDIO_ARRANGEMENT_MAX_CLIPS)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    for (int node = status == UMI_STATUS_OK ? UmiJsonTreeFirst(tree, clips) : -1;
         node >= 0 && status == UMI_STATUS_OK; node = UmiJsonTreeNext(tree, node))
    {
        UmiCreativeAudioPlacement placement = {0};
        status = Placement(tree, node, &placement);
        if (status == UMI_STATUS_OK)
            status = UmiCreativeAudioArrangementPut(&plan, &placement, false);
    }
    if (status == UMI_STATUS_OK)
        status = UmiCreativeAudioArrangementValidate(&plan);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        *out = plan;
    UmiJsonTreeDestroy(tree);
    return status;
}
