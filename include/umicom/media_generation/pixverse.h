/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media_generation/pixverse.h
 * PURPOSE: Own explicit video generation requests and bounded job receipts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_MEDIA_GENERATION_PIXVERSE_H
#define UMICOM_MEDIA_GENERATION_PIXVERSE_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/platform/cancellation.h"
#include "umicom/security/profile_secrets.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_PIXVERSE_BODY_CAPACITY 65536U
    typedef enum UmiPixVerseOperation
    {
        UMI_PIXVERSE_CREATE_VIDEO = 1,
        UMI_PIXVERSE_GET_VIDEO
    } UmiPixVerseOperation;
    /* A draft contains no key. Creation accepts up to 5000 UTF-8 bytes, 1..15
 * seconds, and one documented aspect ratio. quality is 360, 540, 720 or 1080.
 * The wire model identifier is selected by this adapter, not by a URL field.
 * Status uses only video_id and trace_id; all creation fields must be zero.
 * Supply a fresh UUID for every request, including each manual status check. */
    typedef struct UmiPixVerseDraft
    {
        UmiPixVerseOperation operation;
        int64_t video_id;
        char trace_id[37];
        char prompt[5001];
        char aspect_ratio[6];
        unsigned seconds, quality;
        bool audio, multiple_shots;
    } UmiPixVerseDraft;
    typedef struct UmiPixVersePlan UmiPixVersePlan;
    typedef struct UmiPixVerseResult
    {
        unsigned http_status;
        bool creation_may_exist, creation_confirmed;
        int64_t video_id;
        /* 0 is a creation receipt; status replies use provider states 1 (ready),
     * 5 (generating), 6 (deleted), 7 (moderated) or 8 (failed). */
        unsigned state;
        char video_url[4097];
    } UmiPixVerseResult;
    /* Prepare copies and validates everything without I/O. Initialise *out=NULL.
 * Review borrows immutable strings until Destroy. All destinations are fixed
 * to app-api.pixverse.ai. JSON, trace identifiers and model names are protocol
 * data. A signed result URL is private and never downloaded automatically. */
    UmiStatus UmiPixVersePrepare(const UmiPixVerseDraft *draft, UmiPixVersePlan **out);
    UmiStatus UmiPixVerseReview(const UmiPixVersePlan *plan, const char **url, const char **body,
                                bool *creates);
    void UmiPixVerseDestroy(UmiPixVersePlan *plan);
    bool UmiPixVerseHttpAvailable(void);
    /* Blocking worker calls. Review and approve this exact plan before execution.
 * Every plan, including a status check, is single-use once transport starts:
 * PixVerse caches requests by trace ID. Prepare a fresh status request to see
 * progress. No automatic retry/polling occurs. A timeout/cancellation can leave
 * a charged remote job; inspect creation_may_exist on every return. A confirmed
 * job receipt survives a cancellation racing the response. Serialize plan use
 * and destruction. Key/result/plan must not overlap; out is cleared on entry. */
    UmiStatus UmiPixVerseExecute(UmiPixVersePlan *plan, bool approved, const char *api_key,
                                 const UmiCancellationToken *cancel, UmiPixVerseResult *out);
    UmiStatus UmiPixVerseExecuteWithProfile(UmiPixVersePlan *plan, bool approved, UmiProfileSecrets *secrets,
                                            const char *password, const char *alias,
                                            const UmiCancellationToken *cancel, UmiPixVerseResult *out);
    /* Trusted transport extension for other platforms and offline fixtures. The
 * callback borrows validated URL/body/key for this call only. Enforce verified
 * TLS, no redirects, no secret logging, and capacity-bounded response bytes.
 * NULL body means GET. Report the exact response length and HTTP status. */
    typedef UmiStatus (*UmiPixVerseExchange)(void *context, const char *url, const char *body,
                                             const char *trace_id, const char *key,
                                             const UmiCancellationToken *cancel, char *reply, size_t capacity,
                                             size_t *length, unsigned *http);
    UmiStatus UmiPixVerseExecuteWithTransport(UmiPixVersePlan *plan, bool approved, const char *key,
                                              const UmiCancellationToken *cancel,
                                              UmiPixVerseExchange exchange, void *context,
                                              UmiPixVerseResult *out);
#ifdef __cplusplus
}
#endif
#endif
