/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media_generation/seedream.h
 * PURPOSE: Review one image request and own its validated PNG result.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_MEDIA_GENERATION_SEEDREAM_H
#define UMICOM_MEDIA_GENERATION_SEEDREAM_H
#include <stdbool.h>
#include <stddef.h>
#include "umicom/media/png_image.h"
#include "umicom/security/profile_secrets.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* This adapter requests a single opaque PNG at 2K. Choose a ModelArk model or
 * endpoint that supports output_format=png. The provider checks entitlement
 * and model availability. No credentials belong in a draft or review body. */
    typedef struct UmiSeedreamDraft
    {
        char model[129];
        char prompt[8193];
    } UmiSeedreamDraft;
    typedef struct UmiSeedreamPlan UmiSeedreamPlan;
    typedef struct UmiSeedreamResult
    {
        unsigned http_status;
        bool request_may_have_run;
        char model[129];
        UmiMediaImageSurface *image;
    } UmiSeedreamResult;
    /* Preparation validates UTF-8 and owns an immutable JSON body without I/O.
 * Review borrows that body until Destroy. The fixed host cannot be replaced.
 * *out must be NULL. Keep a plan on one worker at a time. */
    UmiStatus UmiSeedreamPrepare(const UmiSeedreamDraft *draft, UmiSeedreamPlan **out);
    UmiStatus UmiSeedreamReview(const UmiSeedreamPlan *plan, const char **url, const char **body);
    void UmiSeedreamDestroy(UmiSeedreamPlan *plan);
    /* Results must be zero-initialized before first use. Clear destroys the owned
 * image and wipes metadata. Execute refuses to overwrite a live image. */
    void UmiSeedreamResultClear(UmiSeedreamResult *result);
    bool UmiSeedreamHttpAvailable(void);
    /* Blocking worker operation: one explicit approval permits one attempt. Once
 * transport is entered, this plan cannot be retried, even after a lost reply.
 * Cancellation stops local waiting; the provider may still generate and bill.
 * Inspect request_may_have_run on failure before preparing another request.
 * Replies are bounded to 32 MiB and PNG payloads to 16 MiB, with Framework's
 * pixel limits. Raw provider errors and temporary download URLs are not exposed.
 * No automatic retry, redirect, polling or URL download takes place. */
    UmiStatus UmiSeedreamExecute(UmiSeedreamPlan *plan, bool approved, const char *key,
                                 const UmiCancellationToken *cancel, UmiSeedreamResult *out);
    UmiStatus UmiSeedreamExecuteWithProfile(UmiSeedreamPlan *plan, bool approved, UmiProfileSecrets *secrets,
                                            const char *password, const char *alias,
                                            const UmiCancellationToken *cancel, UmiSeedreamResult *out);
    /* Trusted transport seam for native adapters and offline fixtures. Enforce TLS,
 * no redirects and no credential logging. Borrow all input only for this call;
 * write at most capacity bytes and return their exact length and HTTP status. */
    typedef UmiStatus (*UmiSeedreamExchange)(void *context, const char *url, const char *body,
                                             const char *key, const UmiCancellationToken *cancel, char *reply,
                                             size_t capacity, size_t *length, unsigned *http);
    UmiStatus UmiSeedreamExecuteWithTransport(UmiSeedreamPlan *plan, bool approved, const char *key,
                                              const UmiCancellationToken *cancel,
                                              UmiSeedreamExchange exchange, void *context,
                                              UmiSeedreamResult *out);
#ifdef __cplusplus
}
#endif
#endif
