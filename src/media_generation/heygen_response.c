/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/heygen_response.c
 * PURPOSE: Decode bounded provider catalogues and match job receipts to requested resources.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "heygen_internal.h"
#include <stdlib.h>
#include <string.h>

/* Compare decoded member names so an escaped spelling cannot hide a second
 * required field. Missing and duplicated fields remain distinct outcomes. */
static int Field(const UmiLanguageRuntimeJsonDocument *d, int object, const char *name)
{
    if (object < 0 || (size_t)object >= d->token_count ||
        d->tokens[object].type != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        return -2;
    int found = -1;
    for (size_t i = 0U; i < umi_language_runtime_json_object_count(d, object); ++i)
    {
        int key, value;
        char text[64];
        if (umi_language_runtime_json_object_entry_at(d, object, i, &key, &value) != UMI_STATUS_OK)
            return -2;
        UmiStatus status = UmiLanguageRuntimeJsonText(d, key, text, sizeof(text));
        if (status == UMI_STATUS_CAPACITY_EXCEEDED)
            continue;
        if (status != UMI_STATUS_OK)
            return -2;
        if (strcmp(text, name) == 0)
        {
            if (found >= 0)
                return -2;
            found = value;
        }
    }
    return found;
}
/* Copy one complete provider string into bounded storage. Missing optional values are empty; duplicate fields and unexpected types are malformed replies. */
static UmiStatus String(const UmiLanguageRuntimeJsonDocument *d, int object, const char *field, char *out,
                        size_t capacity, bool optional)
{
    int token = Field(d, object, field);
    if (token == -2)
        return UMI_STATUS_PARSE_ERROR;
    if (token == -1 || umi_language_runtime_json_is_null(d, token))
    {
        if (!optional)
            return UMI_STATUS_PARSE_ERROR;
        out[0] = '\0';
        return UMI_STATUS_OK;
    }
    if (d->tokens[token].type != UMI_LANGUAGE_RUNTIME_JSON_STRING)
        return UMI_STATUS_PARSE_ERROR;
    UmiStatus status = UmiLanguageRuntimeJsonText(d, token, out, capacity);
    if (status != UMI_STATUS_OK)
        return status;
    for (size_t i = 0U; out[i] != '\0'; ++i)
        if ((unsigned char)out[i] < 0x20U || (unsigned char)out[i] == 0x7fU)
            return UMI_STATUS_PARSE_ERROR;
    return optional || out[0] != '\0' ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}
/* Normalise avatar and voice catalogue records into a common UI row while keeping their distinct provider identifier fields explicit. */
static UmiStatus Item(const UmiLanguageRuntimeJsonDocument *d, int object, bool voice, UmiHeyGenItem *out)
{
    UmiStatus status = String(d, object, voice ? "voice_id" : "id", out->id, sizeof(out->id), false);
    if (status == UMI_STATUS_OK && !UmiHeyGenIdentifier(out->id, sizeof(out->id)))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = String(d, object, "name", out->name, sizeof(out->name), false);
    if (status == UMI_STATUS_OK && !voice)
        status = String(d, object, "status", out->status, sizeof(out->status), false);
    return status;
}
/* A completed receipt can disclose a signed HTTPS link for the user to copy. This check does not authorise a fetch; downloader hosts need a separate policy. */
static bool DownloadUrl(const char *url)
{
    if (strncmp(url, "https://", 8U) != 0)
        return false;
    size_t i = 8U;
    for (; url[i] && url[i] != '/'; ++i)
    {
        unsigned char c = (unsigned char)url[i];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '.' ||
              c == '-'))
            return false;
    }
    if (i == 8U || url[i] != '/')
        return false;
    for (; url[i]; ++i)
        if ((unsigned char)url[i] <= 0x20U || (unsigned char)url[i] >= 0x7fU || url[i] == '\\' ||
            url[i] == '#')
            return false;
    return true;
}
/* Decode into the execution owner's temporary result. The caller publishes that result only after all required fields, IDs and page bounds are accepted. */
UmiStatus UmiHeyGenDecode(const UmiHeyGenPlan *plan, const char *body, size_t length, UmiHeyGenResult *out)
{
    if (length == 0U || length >= UMI_HEYGEN_BODY_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(body, '\0', length) != NULL)
        return UMI_STATUS_PARSE_ERROR;
    char *text = malloc(length + 1U);
    UmiLanguageRuntimeJsonDocument *d = malloc(sizeof(*d));
    if (text == NULL || d == NULL)
    {
        free(text);
        free(d);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(text, body, length);
    text[length] = '\0';
    UmiStatus status = UmiLanguageRuntimeJsonParseComplete(text, d);
    if (status != UMI_STATUS_OK)
        goto done;
    int error = Field(d, 0, "error"), data = Field(d, 0, "data");
    if (error == -2 || (error >= 0 && !umi_language_runtime_json_is_null(d, error)) || data < 0)
    {
        status = UMI_STATUS_PARSE_ERROR;
        goto done;
    }
    if (plan->operation == UMI_HEYGEN_LIST_AVATARS || plan->operation == UMI_HEYGEN_LIST_VOICES)
    {
        if (d->tokens[data].type != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        {
            status = UMI_STATUS_PARSE_ERROR;
            goto done;
        }
        out->count = umi_language_runtime_json_array_count(d, data);
        if (out->count > UMI_HEYGEN_PAGE_LIMIT)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            goto done;
        }
        for (size_t i = 0U; status == UMI_STATUS_OK && i < out->count; ++i)
        {
            status = Item(d, umi_language_runtime_json_array_at(d, data, i),
                          plan->operation == UMI_HEYGEN_LIST_VOICES, &out->items[i]);
            for (size_t j = 0U; status == UMI_STATUS_OK && j < i; ++j)
                if (strcmp(out->items[j].id, out->items[i].id) == 0)
                    status = UMI_STATUS_PARSE_ERROR;
        }
        int more = Field(d, 0, "has_more"), value = 0;
        if (status == UMI_STATUS_OK &&
            (more < 0 || umi_language_runtime_json_bool(d, more, &value) != UMI_STATUS_OK))
            status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK)
        {
            out->has_more = value != 0;
            status = String(d, 0, "next_token", out->next_cursor, sizeof(out->next_cursor), !out->has_more);
        }
        if (status == UMI_STATUS_OK && out->has_more)
        {
            if (out->count == 0U)
                status = UMI_STATUS_PARSE_ERROR;
            for (size_t i = 0U; out->next_cursor[i]; ++i)
                if ((unsigned char)out->next_cursor[i] >= 0x7fU)
                    status = UMI_STATUS_PARSE_ERROR;
        }
    }
    else
    {
        if (plan->operation == UMI_HEYGEN_CREATE_AVATAR)
            data = Field(d, data, "avatar_item");
        status =
            String(d, data,
                   (plan->operation == UMI_HEYGEN_CREATE_VIDEO || plan->operation == UMI_HEYGEN_CREATE_SCENE)
                       ? "video_id"
                       : "id",
                   out->resource_id, sizeof(out->resource_id), false);
        if (status == UMI_STATUS_OK && !UmiHeyGenIdentifier(out->resource_id, sizeof(out->resource_id)))
            status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK && !plan->creates_resource &&
            strcmp(plan->resource_id, out->resource_id) != 0)
            status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK)
            status = String(d, data, "status", out->state, sizeof(out->state), false);
        /* Only a completed video is allowed to publish a download reference.
         * The host still needs a separate credential-free download policy. */
        if (status == UMI_STATUS_OK && plan->operation == UMI_HEYGEN_GET_VIDEO &&
            strcmp(out->state, "completed") == 0)
        {
            status = String(d, data, "video_url", out->video_url, sizeof(out->video_url), false);
            if (status == UMI_STATUS_OK && !DownloadUrl(out->video_url))
                status = UMI_STATUS_PARSE_ERROR;
            if (status == UMI_STATUS_OK)
                status = String(d, data, "subtitle_url", out->subtitle_url, sizeof(out->subtitle_url), true);
            if (status == UMI_STATUS_OK && out->subtitle_url[0] && !DownloadUrl(out->subtitle_url))
                status = UMI_STATUS_PARSE_ERROR;
        }
    }
done:
    umi_secret_clear(text, length + 1U);
    free(text);
    free(d);
    return status;
}
