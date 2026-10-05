/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/heygen_request.c
 * PURPOSE: Encode only reviewed fields and fixed provider routes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "heygen_internal.h"
#include "umicom/ai/mcp/json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Identifiers enter URL paths or JSON fields without escaping. Permit only this narrow provider identifier alphabet, never filesystem paths or URL syntax. */
bool UmiHeyGenIdentifier(const char *text, size_t capacity)
{
    if (text == NULL || text[0] == '\0')
        return false;
    for (size_t i = 0U; i < capacity; ++i)
    {
        unsigned char c = (unsigned char)text[i];
        if (c == 0U)
            return true;
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' ||
              c == '-'))
            return false;
    }
    return false;
}
/* Check termination and useful text within each fixed field. Complete JSON validation below also checks UTF-8 before any request can leave the process. */
static bool Text(const char *text, size_t capacity, bool required)
{
    bool visible = false;
    for (size_t i = 0U; i < capacity; ++i)
    {
        unsigned char c = (unsigned char)text[i];
        if (c == 0U)
            return !required || visible;
        if ((c < 0x20U && c != '\n' && c != '\r' && c != '\t') || c == 0x7fU)
            return false;
        if (c > 0x20U)
            visible = true;
    }
    return false;
}
static UmiStatus Cursor(const char *input, char *out, size_t capacity)
{
    static const char hex[] = "0123456789ABCDEF";
    size_t used = 0U;
    for (size_t i = 0U; i < UMI_HEYGEN_CURSOR_CAPACITY; ++i)
    {
        unsigned char c = (unsigned char)input[i];
        if (c == 0U)
        {
            out[used] = '\0';
            return UMI_STATUS_OK;
        }
        /* Cursor bytes are data, never query syntax. Encoding every byte also
         * preserves opaque punctuation and prevents adding another parameter. */
        if (c < 0x20U || c >= 0x7fU)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (capacity - used <= 3U)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        out[used++] = '%';
        out[used++] = hex[c >> 4U];
        out[used++] = hex[c & 15U];
    }
    return UMI_STATUS_CAPACITY_EXCEEDED;
}
/* Keep operation-specific fields explicit. Refusing unrelated fields prevents an old form value from leaking into a different provider request. */
UmiStatus UmiHeyGenEncode(const UmiHeyGenDraft *d, UmiHeyGenPlan *p)
{
    bool list = d->operation == UMI_HEYGEN_LIST_AVATARS || d->operation == UMI_HEYGEN_LIST_VOICES;
    bool get = d->operation == UMI_HEYGEN_GET_AVATAR || d->operation == UMI_HEYGEN_GET_VIDEO;
    bool avatar = d->operation == UMI_HEYGEN_CREATE_AVATAR;
    bool video = d->operation == UMI_HEYGEN_CREATE_VIDEO;
    bool scene = d->operation == UMI_HEYGEN_CREATE_SCENE;
    if (!list && !get && !avatar && !video && !scene)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!Text(d->name, sizeof(d->name), avatar || video) ||
        !Text(d->script, sizeof(d->script), avatar || video || scene) ||
        !Text(d->motion, sizeof(d->motion), false) ||
        ((get || video) && !UmiHeyGenIdentifier(d->resource_id, sizeof(d->resource_id))) ||
        (video && !UmiHeyGenIdentifier(d->voice_id, sizeof(d->voice_id))) ||
        ((avatar || video || scene) && !UmiHeyGenIdentifier(d->request_key, sizeof(d->request_key))))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (((list || avatar || scene) && d->resource_id[0]) || (!video && d->voice_id[0]) ||
        ((list || get) && (d->name[0] || d->script[0] || d->request_key[0] || d->portrait || d->subtitles)) ||
        (!video && (d->motion[0] || d->subtitles)) || (!list && (d->cursor[0] || d->private_catalogue)))
        return UMI_STATUS_INVALID_ARGUMENT;
    if ((!scene && (d->image_asset_id[0] || d->scene_seconds)) ||
        (scene && (d->name[0] || d->scene_seconds < 5U || d->scene_seconds > 15U ||
                   (d->image_asset_id[0] &&
                    (!UmiHeyGenIdentifier(d->image_asset_id, sizeof(d->image_asset_id)) || d->portrait)))))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (avatar && !Text(d->script, 1001U, true))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    p->operation = d->operation;
    p->creates_resource = avatar || video || scene;
    if (get || video)
        strcpy(p->resource_id, d->resource_id);
    int written;
    if (list)
    {
        char cursor[UMI_HEYGEN_CURSOR_CAPACITY * 3U] = {0};
        UmiStatus status = Cursor(d->cursor, cursor, sizeof(cursor));
        if (status != UMI_STATUS_OK)
            return status;
        const char *route = d->operation == UMI_HEYGEN_LIST_AVATARS ? "avatars/looks" : "voices";
        const char *filter = d->operation == UMI_HEYGEN_LIST_AVATARS ? "ownership" : "type";
        written =
            snprintf(p->url, sizeof(p->url), "https://api.heygen.com/v3/%s?limit=20&%s=%s%s%s", route, filter,
                     d->private_catalogue ? "private" : "public", cursor[0] ? "&token=" : "", cursor);
    }
    else if (get)
        written =
            snprintf(p->url, sizeof(p->url), "https://api.heygen.com/v3/%s/%s",
                     d->operation == UMI_HEYGEN_GET_AVATAR ? "avatars/looks" : "videos", d->resource_id);
    else
        written = snprintf(p->url, sizeof(p->url), "https://api.heygen.com/v3/%s",
                           avatar  ? "avatars"
                           : scene ? "models/videos"
                                   : "videos");
    if (written < 0 || (size_t)written >= sizeof(p->url))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!p->creates_resource)
        return UMI_STATUS_OK;
    strcpy(p->request_key, d->request_key);
    char *script = calloc(50000U, 1U), *name = calloc(1600U, 1U), *motion = calloc(6100U, 1U);
    UmiLanguageRuntimeJsonDocument *document = malloc(sizeof(*document));
    if (script == NULL || name == NULL || motion == NULL || document == NULL)
    {
        free(script);
        free(name);
        free(motion);
        free(document);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiStatus status = umi_ai_mcp_json_escape_string(d->script, script, 50000U);
    if (status == UMI_STATUS_OK)
        status = umi_ai_mcp_json_escape_string(d->name, name, 1600U);
    if (status == UMI_STATUS_OK)
        status = umi_ai_mcp_json_escape_string(d->motion, motion, 6100U);
    if (status == UMI_STATUS_OK)
    {
        if (avatar)
            written = snprintf(p->body, sizeof(p->body),
                               "{\"type\":\"prompt\",\"name\":%s,\"prompt\":%s,\"aspect_ratio\":\"%s\"}",
                               name, script, d->portrait ? "9:16" : "16:9");
        else if (scene)
        {
            /* A reviewed asset identifier references a file already held by
             * this provider. No local paths, credentials or arbitrary URLs
             * enter a prompt. Disable provider-side prompt rewriting. */
            if (d->image_asset_id[0])
                written = snprintf(p->body, sizeof(p->body),
                                   "{\"model\":\"heygen-video-1\",\"mode\":\"image_to_video\",\"prompt\":%s,"
                                   "\"image\":{\"type\":\"asset_id\",\"asset_id\":\"%s\"},\"duration\":%u,"
                                   "\"resolution\":\"768p\",\"prompt_enhancement\":\"disabled\"}",
                                   script, d->image_asset_id, d->scene_seconds);
            else
                written = snprintf(p->body, sizeof(p->body),
                                   "{\"model\":\"heygen-video-1\",\"mode\":\"text_to_video\",\"prompt\":%s,"
                                   "\"duration\":%u,\"aspect_ratio\":\"%s\",\"resolution\":\"768p\",\"prompt_"
                                   "enhancement\":\"disabled\"}",
                                   script, d->scene_seconds, d->portrait ? "9:16" : "16:9");
        }
        else
            written = snprintf(
                p->body, sizeof(p->body),
                "{\"type\":\"avatar\",\"avatar_id\":\"%s\",\"voice_id\":\"%s\",\"title\":%s,\"script\":%s,"
                "\"motion_prompt\":%s,\"aspect_ratio\":\"%s\",\"resolution\":\"1080p\",\"output_format\":"
                "\"mp4\"%s}",
                d->resource_id, d->voice_id, name, script, motion, d->portrait ? "9:16" : "16:9",
                d->subtitles ? ",\"caption\":{\"file_format\":\"srt\"}" : "");
        status = written < 0 || (size_t)written >= sizeof(p->body)
                     ? UMI_STATUS_CAPACITY_EXCEEDED
                     : UmiLanguageRuntimeJsonParseComplete(p->body, document);
    }
    umi_secret_clear(script, 50000U);
    umi_secret_clear(name, 1600U);
    umi_secret_clear(motion, 6100U);
    free(script);
    free(name);
    free(motion);
    free(document);
    return status;
}
