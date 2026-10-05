/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/seedream_plan.c
 * PURPOSE: Own reviewed image requests, credentials and one-attempt submission.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "seedream_internal.h"
#include "umicom/ai/mcp/json.h"
#include "umicom/language_runtime/json_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Bound every fixed field before calling string functions. Model identifiers
 * are data, not URLs; a restricted alphabet makes this distinction explicit. */
static bool ModelIdentifierValid(const char *text, size_t capacity)
{
    for (size_t index = 0; index < capacity; ++index)
    {
        unsigned char character = (unsigned char)text[index];
        if (!character)
            return index > 0;
        if (!((character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') ||
              (character >= '0' && character <= '9') || character == '-' || character == '_' ||
              character == '.'))
            return false;
    }
    return false;
}
UmiStatus UmiSeedreamPrepare(const UmiSeedreamDraft *draft, UmiSeedreamPlan **out)
{
    if (!draft || !out || *out || !ModelIdentifierValid(draft->model, sizeof(draft->model)) ||
        !memchr(draft->prompt, 0, sizeof(draft->prompt)))
        return UMI_STATUS_INVALID_ARGUMENT;
    bool visible = false;
    for (size_t index = 0; draft->prompt[index]; ++index)
    {
        unsigned char character = (unsigned char)draft->prompt[index];
        if ((character < 0x20U && character != '\n' && character != '\r' && character != '\t') ||
            character == 0x7fU)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (character > 0x20U)
            visible = true;
    }
    if (!visible)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiSeedreamPlan *plan = calloc(1, sizeof(*plan));
    char *escaped = calloc(49155U, 1);
    if (!plan || !escaped)
    {
        free(plan);
        free(escaped);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiStatus status = umi_ai_mcp_json_escape_string(draft->prompt, escaped, 49155U);
    if (status == UMI_STATUS_OK)
    {
        /* Request image bytes inline so an untrusted response cannot redirect a
         * credentialed download. Keep watermarking enabled and generation single. */
        int written = snprintf(plan->body, sizeof(plan->body),
                               "{\"model\":\"%s\",\"prompt\":%s,\"size\":\"2K\",\"output_format\":\"png\","
                               "\"response_format\":\"b64_json\",\"stream\":false,\"watermark\":true}",
                               draft->model, escaped);
        status = written < 0 || (size_t)written >= sizeof(plan->body) ? UMI_STATUS_CAPACITY_EXCEEDED
                                                                      : UMI_STATUS_OK;
    }
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {sizeof(plan->body), 64U, 4U};
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeCreate(plan->body, strlen(plan->body), &limits, NULL, &tree);
    UmiJsonTreeDestroy(tree);
    umi_secret_clear(escaped, 49155U);
    free(escaped);
    if (status == UMI_STATUS_OK)
        *out = plan;
    else
        UmiSeedreamDestroy(plan);
    return status;
}
UmiStatus UmiSeedreamReview(const UmiSeedreamPlan *plan, const char **url, const char **body)
{
    if (!plan || !url || !body)
        return UMI_STATUS_INVALID_ARGUMENT;
    *url = UMI_SEEDREAM_ENDPOINT;
    *body = plan->body;
    return UMI_STATUS_OK;
}
void UmiSeedreamDestroy(UmiSeedreamPlan *plan)
{
    if (plan)
    {
        umi_secret_clear(plan, sizeof(*plan));
        free(plan);
    }
}
void UmiSeedreamResultClear(UmiSeedreamResult *result)
{
    if (result)
    {
        umi_media_image_surface_destroy(result->image);
        umi_secret_clear(result, sizeof(*result));
    }
}
static bool ApiKeyValid(const char *key)
{
    if (!key || !key[0])
        return false;
    for (size_t index = 0; index < 2049U; ++index)
    {
        unsigned char character = (unsigned char)key[index];
        if (!character)
            return true;
        if (character <= 0x20U || character >= 0x7fU)
            return false;
    }
    return false;
}
UmiStatus UmiSeedreamExecuteWithTransport(UmiSeedreamPlan *plan, bool approved, const char *key,
                                          const UmiCancellationToken *cancel, UmiSeedreamExchange exchange,
                                          void *context, UmiSeedreamResult *out)
{
    if (!out || out->image)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    if (!plan || !exchange)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!approved || !ApiKeyValid(key))
        return UMI_STATUS_PERMISSION_DENIED;
    if (plan->consumed)
    {
        out->request_may_have_run = true;
        return UMI_STATUS_INVALID_STATE;
    }
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    if (!UmiMediaPngAvailable())
        return UMI_STATUS_UNAVAILABLE;
    char *reply = malloc(UMI_SEEDREAM_REPLY_LIMIT);
    if (!reply)
        return UMI_STATUS_OUT_OF_MEMORY;
    size_t length = 0;
    /* A transport error cannot prove the provider did no work. Consuming the
     * plan before transport prevents an accidental second charge from retry. */
    plan->consumed = true;
    out->request_may_have_run = true;
    UmiStatus status = exchange(context, UMI_SEEDREAM_ENDPOINT, plan->body, key, cancel, reply,
                                UMI_SEEDREAM_REPLY_LIMIT, &length, &out->http_status);
    if (status == UMI_STATUS_OK && out->http_status != 200U)
        status = out->http_status == 401U || out->http_status == 403U   ? UMI_STATUS_PERMISSION_DENIED
                 : out->http_status == 429U || out->http_status == 503U ? UMI_STATUS_BUSY
                                                                        : UMI_STATUS_IO_ERROR;
    if (status == UMI_STATUS_OK)
        status = length == 0 || length > UMI_SEEDREAM_REPLY_LIMIT
                     ? UMI_STATUS_CAPACITY_EXCEEDED
                     : UmiSeedreamDecode(reply, length, cancel, out);
    umi_secret_clear(reply, UMI_SEEDREAM_REPLY_LIMIT);
    free(reply);
    return status;
}
UmiStatus UmiSeedreamExecute(UmiSeedreamPlan *plan, bool approved, const char *key,
                             const UmiCancellationToken *cancel, UmiSeedreamResult *out)
{
    if (!out || out->image)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    if (!UmiSeedreamHttpAvailable())
        return UMI_STATUS_UNAVAILABLE;
    return UmiSeedreamExecuteWithTransport(plan, approved, key, cancel, UmiSeedreamHttp, NULL, out);
}
UmiStatus UmiSeedreamExecuteWithProfile(UmiSeedreamPlan *plan, bool approved, UmiProfileSecrets *secrets,
                                        const char *password, const char *alias,
                                        const UmiCancellationToken *cancel, UmiSeedreamResult *out)
{
    if (!out || out->image)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    if (!plan || !secrets)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!approved)
        return UMI_STATUS_PERMISSION_DENIED;
    if (plan->consumed)
    {
        out->request_may_have_run = true;
        return UMI_STATUS_INVALID_STATE;
    }
    if (!UmiSeedreamHttpAvailable() || !UmiMediaPngAvailable())
        return UMI_STATUS_UNAVAILABLE;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    /* Resolve a local alias only at the approved send boundary. Keep the key
     * out of request objects, images, export files and application settings. */
    char key[2049] = {0};
    UmiStatus status = UmiProfileSecretsGet(secrets, password, alias, key, sizeof(key));
    if (status == UMI_STATUS_OK)
        status = UmiSeedreamExecute(plan, true, key, cancel, out);
    umi_secret_clear(key, sizeof(key));
    return status;
}
