/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/song_plan_document.c
 * PURPOSE: Export subtitles and reopen song drafts without trusting derived data.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "song_plan_internal.h"
#include "umicom/ai/mcp/json.h"
#include "umicom/language_runtime/json_tree.h"
#include "umicom/security/secrets.h"
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
/* Keep one bounded document writer and retain the first formatting failure. Later fields cannot turn a truncated document into a reported success. */
static void Append(Writer *writer, const char *format, ...)
{
    if (writer->status != UMI_STATUS_OK)
        return;
    va_list args;
    va_start(args, format);
    size_t room = UMI_SONG_DOCUMENT_LIMIT - writer->used;
    int count = vsnprintf(writer->bytes + writer->used, room, format, args);
    va_end(args);
    if (count < 0 || (size_t)count >= room)
        writer->status = UMI_STATUS_CAPACITY_EXCEEDED;
    else
        writer->used += (size_t)count;
}
/* Delegate JSON quoting to the shared encoder; titles and lyrics must never become document structure. */
static void Quoted(Writer *writer, const char *text)
{
    if (writer->status != UMI_STATUS_OK)
        return;
    writer->status = umi_ai_mcp_json_escape_string(text, writer->bytes + writer->used,
                                                   UMI_SONG_DOCUMENT_LIMIT - writer->used);
    if (writer->status == UMI_STATUS_OK)
        writer->used += strlen(writer->bytes + writer->used);
}
/* Express integer milliseconds in SRT notation without a locale-dependent decimal separator. */
static void Timestamp(Writer *writer, unsigned time)
{
    Append(writer, "%02u:%02u:%02u,%03u", time / 3600000U, (time / 60000U) % 60U, (time / 1000U) % 60U,
           time % 1000U);
}
/* Build a complete immutable asset before handing it to a file adapter. The same bytes can be exported by native, web or future mobile frontends. */
UmiStatus UmiSongPlanExport(const UmiSongPlan *plan, UmiSongExport kind, const UmiCancellationToken *cancel,
                            UmiCreativeAsset **out)
{
    if (!plan || !out || *out || (kind != UMI_SONG_EXPORT_PLAN && kind != UMI_SONG_EXPORT_SUBTITLES))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    Writer writer = {calloc(UMI_SONG_DOCUMENT_LIMIT, 1), 0, UMI_STATUS_OK};
    if (!writer.bytes)
        return UMI_STATUS_OUT_OF_MEMORY;
    if (kind == UMI_SONG_EXPORT_SUBTITLES)
    {
        for (size_t index = 0; index < plan->cue_count; ++index)
        {
            const UmiSongCue *cue = &plan->cues[index];
            Append(&writer, "%zu\n", index + 1U);
            Timestamp(&writer, cue->start_ms);
            Append(&writer, " --> ");
            Timestamp(&writer, cue->end_ms);
            Append(&writer, "\n%s\n\n", cue->text);
        }
    }
    else
    {
        Append(&writer, "{\"format\":\"umicom-song-plan\",\"title\":");
        Quoted(&writer, plan->draft.title);
        Append(&writer, ",\"style\":");
        Quoted(&writer, plan->draft.style);
        Append(&writer, ",\"timed_lyrics\":");
        Quoted(&writer, plan->draft.timed_lyrics);
        Append(&writer,
               ",\"duration_ms\":%u,\"beats_per_minute\":%u,\"beats_per_shot\":%u,\"first_beat_ms\":%u,"
               "\"shots\":[",
               plan->draft.duration_ms, plan->draft.beats_per_minute, plan->draft.beats_per_shot,
               plan->draft.first_beat_ms);
        for (size_t index = 0; index < plan->shot_count; ++index)
        {
            const UmiSongShot *shot = &plan->shots[index];
            Append(&writer, "%s{\"start_ms\":%u,\"end_ms\":%u,\"lyrics\":[", index ? "," : "", shot->start_ms,
                   shot->end_ms);
            for (size_t cue = 0; cue < shot->cue_count; ++cue)
            {
                Append(&writer, "%s", cue ? "," : "");
                Quoted(&writer, plan->cues[shot->first_cue + cue].text);
            }
            Append(&writer, "]}");
        }
        Append(&writer, "]}\n");
    }
    if (writer.status == UMI_STATUS_OK)
        writer.status = UmiCreativeAssetCapture(plan->draft.title,
                                                kind == UMI_SONG_EXPORT_PLAN ? UMI_CREATIVE_ASSET_DOCUMENT
                                                                             : UMI_CREATIVE_ASSET_LYRICS,
                                                writer.bytes, writer.used, cancel, out);
    umi_secret_clear(writer.bytes, UMI_SONG_DOCUMENT_LIMIT);
    free(writer.bytes);
    return writer.status;
}
/* Read a required unique text field. The tree rejects duplicate decoded names rather than silently choosing one. */
static UmiStatus Text(const UmiJsonTree *tree, const char *key, char *text, size_t capacity)
{
    int node = -1;
    UmiStatus status = UmiJsonTreeMember(tree, 0, key, &node);
    return status == UMI_STATUS_OK ? UmiJsonTreeText(tree, node, text, capacity) : status;
}
/* Accept only bounded integer spellings; negative, fractional and oversized timing values are not coerced. */
static UmiStatus Number(const UmiJsonTree *tree, const char *key, unsigned *out)
{
    int node = -1;
    int64_t value = 0;
    UmiStatus status = UmiJsonTreeMember(tree, 0, key, &node);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeInteger(tree, node, &value);
    if (status == UMI_STATUS_OK && (value < 0 || value > 600000))
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK)
        *out = (unsigned)value;
    return status;
}
/* Import the editable draft and recompute all timing through the same owner used by the UI. A document cannot inject authoritative derived shots or executable actions. */
UmiStatus UmiSongPlanImport(const void *bytes, size_t length, const UmiCancellationToken *cancel,
                            UmiSongPlan **out)
{
    if (!bytes || !length || !out || *out)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_SONG_DOCUMENT_LIMIT, 16384U, 12U};
    UmiStatus status = UmiJsonTreeCreate(bytes, length, &limits, cancel, &tree);
    UmiSongDraft *draft = calloc(1, sizeof(*draft));
    if (!draft)
    {
        UmiJsonTreeDestroy(tree);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    char format[32] = {0};
    if (status == UMI_STATUS_OK)
        status = Text(tree, "format", format, sizeof(format));
    if (status == UMI_STATUS_OK && strcmp(format, "umicom-song-plan"))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = Text(tree, "title", draft->title, sizeof(draft->title));
    if (status == UMI_STATUS_OK)
        status = Text(tree, "style", draft->style, sizeof(draft->style));
    if (status == UMI_STATUS_OK)
        status = Text(tree, "timed_lyrics", draft->timed_lyrics, sizeof(draft->timed_lyrics));
    if (status == UMI_STATUS_OK)
        status = Number(tree, "duration_ms", &draft->duration_ms);
    if (status == UMI_STATUS_OK)
        status = Number(tree, "beats_per_minute", &draft->beats_per_minute);
    if (status == UMI_STATUS_OK)
        status = Number(tree, "beats_per_shot", &draft->beats_per_shot);
    if (status == UMI_STATUS_OK)
        status = Number(tree, "first_beat_ms", &draft->first_beat_ms);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = UmiSongPlanCreate(draft, out);
    UmiJsonTreeDestroy(tree);
    umi_secret_clear(draft, sizeof(*draft));
    free(draft);
    return status;
}
