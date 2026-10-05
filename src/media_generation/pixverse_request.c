/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/pixverse_request.c
 * PURPOSE: Build a bounded credential-free request snapshot for review.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "pixverse_internal.h"
#include "umicom/ai/mcp/json.h"
#include "umicom/language_runtime/json_tree.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* UUID spelling is deliberately strict: it becomes a header, not free text.
 * Uniqueness is the caller's responsibility; native forms use GLib's generator. */
static bool TraceValid(const char text[37])
{
    for (size_t index = 0; index < 36U; ++index)
    {
        unsigned char c = (unsigned char)text[index];
        if (index == 8U || index == 13U || index == 18U || index == 23U)
        {
            if (c != '-')
                return false;
        }
        else if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')))
            return false;
    }
    return text[36] == '\0';
}
/* Keep the documented aspect ratios in one portable allowlist. A future form can offer a chooser without changing request validation. */
static bool RatioValid(const char *text)
{
    const char *ratios[] = {"16:9", "4:3", "1:1", "3:4", "9:16", "2:3", "3:2", "21:9"};
    for (size_t index = 0; index < sizeof(ratios) / sizeof(ratios[0]); ++index)
        if (!strcmp(text, ratios[index]))
            return true;
    return false;
}
/* Select the operation before copying fields to the wire. Status checks cannot accidentally upload text left over from a creation form. */
UmiStatus UmiPixVerseEncode(const UmiPixVerseDraft *draft, UmiPixVersePlan *plan)
{
    if (!TraceValid(draft->trace_id) || !memchr(draft->prompt, 0, sizeof(draft->prompt)) ||
        !memchr(draft->aspect_ratio, 0, sizeof(draft->aspect_ratio)))
        return UMI_STATUS_INVALID_ARGUMENT;
    plan->operation = draft->operation;
    strcpy(plan->request_key, draft->trace_id);
    if (draft->operation == UMI_PIXVERSE_GET_VIDEO)
    {
        if (draft->video_id <= 0 || draft->prompt[0] || draft->aspect_ratio[0] || draft->seconds ||
            draft->quality || draft->audio || draft->multiple_shots)
            return UMI_STATUS_INVALID_ARGUMENT;
        plan->video_id = draft->video_id;
        int count = snprintf(plan->url, sizeof(plan->url), UMI_PIXVERSE_ORIGIN "video/result/%" PRId64,
                             draft->video_id);
        return count < 0 || (size_t)count >= sizeof(plan->url) ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
    }
    if (draft->operation != UMI_PIXVERSE_CREATE_VIDEO || draft->video_id ||
        !RatioValid(draft->aspect_ratio) || draft->seconds < 1U || draft->seconds > 15U ||
        (draft->quality != 360U && draft->quality != 540U && draft->quality != 720U &&
         draft->quality != 1080U))
        return UMI_STATUS_INVALID_ARGUMENT;
    bool visible = false;
    for (size_t index = 0; draft->prompt[index]; ++index)
    {
        unsigned char c = (unsigned char)draft->prompt[index];
        if ((c < 0x20U && c != '\n' && c != '\r' && c != '\t') || c == 0x7fU)
            return UMI_STATUS_INVALID_ARGUMENT;
        visible = visible || c > 0x20U;
    }
    if (!visible)
        return UMI_STATUS_INVALID_ARGUMENT;
    char *escaped = calloc(30007U, 1);
    if (!escaped)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = umi_ai_mcp_json_escape_string(draft->prompt, escaped, 30007U);
    if (status == UMI_STATUS_OK)
    {
        int count = snprintf(
            plan->body, sizeof(plan->body),
            "{\"model\":\"v6\",\"prompt\":%s,\"duration\":%u,\"quality\":\"%up\",\"aspect_ratio\":\"%s\","
            "\"generate_audio_switch\":%s,\"generate_multi_clip_switch\":%s}",
            escaped, draft->seconds, draft->quality, draft->aspect_ratio, draft->audio ? "true" : "false",
            draft->multiple_shots ? "true" : "false");
        status =
            count < 0 || (size_t)count >= sizeof(plan->body) ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
    }
    /* Validate complete UTF-8 through the shared JSON owner before accepting
     * a snapshot. This catches invalid text without depending on a UI toolkit. */
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {sizeof(plan->body), 64U, 4U};
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeCreate(plan->body, strlen(plan->body), &limits, NULL, &tree);
    UmiJsonTreeDestroy(tree);
    umi_secret_clear(escaped, 30007U);
    free(escaped);
    if (status == UMI_STATUS_OK)
    {
        plan->creates_resource = true;
        strcpy(plan->url, UMI_PIXVERSE_ORIGIN "video/text/generate");
    }
    return status;
}
