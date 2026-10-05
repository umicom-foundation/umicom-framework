/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/pixverse_plan.c
 * PURPOSE: Keep credential lifetime and uncertain remote creation outcomes explicit.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "pixverse_internal.h"
#include <stdlib.h>
#include <string.h>

/* Create a private request snapshot before any credential or network access. A host can show this snapshot for review while the original form keeps changing. */
UmiStatus UmiPixVersePrepare(const UmiPixVerseDraft *draft, UmiPixVersePlan **out)
{
    if (draft == NULL || out == NULL || *out != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiPixVersePlan *plan = calloc(1U, sizeof(*plan));
    if (plan == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiPixVerseEncode(draft, plan);
    if (status == UMI_STATUS_OK)
        *out = plan;
    else
        UmiPixVerseDestroy(plan);
    return status;
}
/* Borrow the already-encoded request; do not rebuild it from mutable form fields after approval. */
UmiStatus UmiPixVerseReview(const UmiPixVersePlan *plan, const char **url, const char **body, bool *creates)
{
    if (plan == NULL || url == NULL || body == NULL || creates == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *url = plan->url;
    *body = plan->body;
    *creates = plan->creates_resource;
    return UMI_STATUS_OK;
}
/* Release the snapshot after its worker has finished. Wiping also retires script and character-description bytes held in this allocation. */
void UmiPixVerseDestroy(UmiPixVersePlan *plan)
{
    if (plan != NULL)
    {
        umi_secret_clear(plan, sizeof(*plan));
        free(plan);
    }
}
static bool KeyValid(const char *key)
{
    if (key == NULL || key[0] == '\0')
        return false;
    /* A key is an opaque printable header value. Refuse whitespace/control
     * injection before a transport callback has a chance to send anything. */
    for (size_t i = 0U; i < 2049U; ++i)
    {
        unsigned char c = (unsigned char)key[i];
        if (c == 0U)
            return true;
        if (c <= 0x20U || c >= 0x7fU)
            return false;
    }
    return false;
}
/* Keep the submission policy above every transport. A successful HTTP transfer alone is not a receipt: the complete reply must also match this operation. */
UmiStatus UmiPixVerseExecuteWithTransport(UmiPixVersePlan *plan, bool approved, const char *key,
                                          const UmiCancellationToken *cancel, UmiPixVerseExchange exchange,
                                          void *context, UmiPixVerseResult *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    if (plan == NULL || exchange == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!approved || !KeyValid(key))
        return UMI_STATUS_PERMISSION_DENIED;
    if (plan->consumed)
    {
        out->creation_may_exist = plan->creates_resource;
        return UMI_STATUS_INVALID_STATE;
    }
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    char *reply = calloc(UMI_PIXVERSE_BODY_CAPACITY, 1U);
    UmiPixVerseResult *decoded = calloc(1U, sizeof(*decoded));
    if (reply == NULL || decoded == NULL)
    {
        free(reply);
        free(decoded);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    size_t length = 0U;
    unsigned http = 0U;
    /* Mark before entering an external callback. A lost reply cannot establish
     * that a paid job was not created. A fresh plan requires another review. */
    /* Every attempt consumes its trace. A new status snapshot obtains a fresh
     * trace rather than asking PixVerse for a cached previous status reply. */
    plan->consumed = true;
    if (plan->creates_resource)
    {
        out->creation_may_exist = true;
    }
    UmiStatus status =
        exchange(context, plan->url, plan->creates_resource ? plan->body : NULL, plan->request_key, key,
                 cancel, reply, UMI_PIXVERSE_BODY_CAPACITY, &length, &http);
    out->http_status = http;
    if (status == UMI_STATUS_OK && http != 200U)
        status = http == 401U || http == 402U || http == 403U   ? UMI_STATUS_PERMISSION_DENIED
                 : http == 404U                                 ? UMI_STATUS_NOT_FOUND
                 : http == 429U || http == 409U || http == 503U ? UMI_STATUS_BUSY
                                                                : UMI_STATUS_IO_ERROR;
    if (status == UMI_STATUS_OK)
        status = UmiPixVerseDecode(plan, reply, length, decoded);
    if (status == UMI_STATUS_OK)
    {
        /* Preserve a confirmed remote ID even if cancellation raced the reply.
         * Losing that receipt would encourage an accidental second purchase. */
        decoded->http_status = http;
        decoded->creation_may_exist = plan->creates_resource;
        decoded->creation_confirmed = plan->creates_resource;
        *out = *decoded;
    }
    if (status == UMI_STATUS_OK && !plan->creates_resource && umi_cancellation_token_is_requested(cancel))
    {
        memset(out, 0, sizeof(*out));
        out->http_status = http;
        status = UMI_STATUS_CANCELLED;
    }
    umi_secret_clear(reply, UMI_PIXVERSE_BODY_CAPACITY);
    free(reply);
    umi_secret_clear(decoded, sizeof(*decoded));
    free(decoded);
    return status;
}
/* Select the native adapter only when its optional dependency was built. Other frontends can supply their own trusted transport to the same request owner. */
UmiStatus UmiPixVerseExecute(UmiPixVersePlan *plan, bool approved, const char *key,
                             const UmiCancellationToken *cancel, UmiPixVerseResult *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    if (!UmiPixVerseHttpAvailable())
        return UMI_STATUS_UNAVAILABLE;
    return UmiPixVerseExecuteWithTransport(plan, approved, key, cancel, UmiPixVerseHttp, NULL, out);
}
/* Resolve the named local credential for one approved operation. Neither the reusable plan nor the result becomes a credential container. */
UmiStatus UmiPixVerseExecuteWithProfile(UmiPixVersePlan *plan, bool approved, UmiProfileSecrets *secrets,
                                        const char *password, const char *alias,
                                        const UmiCancellationToken *cancel, UmiPixVerseResult *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    if (plan == NULL || secrets == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!approved)
        return UMI_STATUS_PERMISSION_DENIED;
    if (plan->consumed)
    {
        out->creation_may_exist = plan->creates_resource;
        return UMI_STATUS_INVALID_STATE;
    }
    if (!UmiPixVerseHttpAvailable())
        return UMI_STATUS_UNAVAILABLE;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    char key[2049] = {0};
    UmiStatus status = UmiProfileSecretsGet(secrets, password, alias, key, sizeof(key));
    if (status == UMI_STATUS_OK)
        status = UmiPixVerseExecute(plan, true, key, cancel, out);
    umi_secret_clear(key, sizeof(key));
    return status;
}
