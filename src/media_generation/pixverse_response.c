/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/pixverse_response.c
 * PURPOSE: Validate job identity and state before publishing a provider receipt.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "pixverse_internal.h"
#include "umicom/language_runtime/json_tree.h"
#include <string.h>
/* Accept only printable HTTPS result links for display. This is not download authorization; a future downloader needs its own destination policy and user action. */
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

/* Provider job identifiers remain exact integers. Parsing through a floating-point number could point a status check at the wrong job. */
static UmiStatus Integer(const UmiJsonTree *tree, int object, const char *key, int64_t *out)
{
    int node = -1;
    UmiStatus status = UmiJsonTreeMember(tree, object, key, &node);
    return status == UMI_STATUS_OK ? UmiJsonTreeInteger(tree, node, out) : status;
}
/* Required keys are decoded by the shared tree, which detects duplicate names
 * even when JSON escape sequences give them different wire spellings. Unknown
 * fields are tolerated for provider extensions; known fields stay strict. */
UmiStatus UmiPixVerseDecode(const UmiPixVersePlan *plan, const char *body, size_t length,
                            UmiPixVerseResult *out)
{
    if (!length || length >= UMI_PIXVERSE_BODY_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_PIXVERSE_BODY_CAPACITY, 512U, 12U};
    UmiStatus status = UmiJsonTreeCreate(body, length, &limits, NULL, &tree);
    int response = -1, node = -1;
    int64_t code = 0, identity = 0, state = 0;
    if (status == UMI_STATUS_OK)
        status = Integer(tree, 0, "ErrCode", &code);
    if (status == UMI_STATUS_OK && code != 0)
        status = UMI_STATUS_IO_ERROR;
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeMember(tree, 0, "Resp", &response);
    if (status == UMI_STATUS_OK)
        status = Integer(tree, response, plan->creates_resource ? "video_id" : "id", &identity);
    if (status == UMI_STATUS_OK && (identity <= 0 || (!plan->creates_resource && identity != plan->video_id)))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK && !plan->creates_resource)
        status = Integer(tree, response, "status", &state);
    if (status == UMI_STATUS_OK && !plan->creates_resource && state != 1 && state != 5 && state != 6 &&
        state != 7 && state != 8)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK && state == 1)
    {
        status = UmiJsonTreeMember(tree, response, "url", &node);
        if (status == UMI_STATUS_OK)
            status = UmiJsonTreeText(tree, node, out->video_url, sizeof(out->video_url));
        if (status == UMI_STATUS_OK && !DownloadUrl(out->video_url))
            status = UMI_STATUS_PARSE_ERROR;
    }
    if (status == UMI_STATUS_OK)
    {
        out->video_id = identity;
        out->state = (unsigned)state;
    }
    UmiJsonTreeDestroy(tree);
    return status;
}
