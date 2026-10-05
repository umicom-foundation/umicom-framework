/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media_generation/heygen.h
 * PURPOSE: Own reviewed avatar requests, bounded replies and explicit remote submission.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_MEDIA_GENERATION_HEYGEN_H
#define UMICOM_MEDIA_GENERATION_HEYGEN_H
#include <stdbool.h>
#include <stddef.h>
#include "umicom/platform/cancellation.h"
#include "umicom/security/profile_secrets.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_HEYGEN_ID_CAPACITY 129U
#define UMI_HEYGEN_NAME_CAPACITY 257U
#define UMI_HEYGEN_CURSOR_CAPACITY 513U
#define UMI_HEYGEN_PAGE_LIMIT 20U
#define UMI_HEYGEN_BODY_CAPACITY 65536U

    typedef enum UmiHeyGenOperation
    {
        UMI_HEYGEN_LIST_AVATARS = 1,
        UMI_HEYGEN_LIST_VOICES,
        UMI_HEYGEN_GET_AVATAR,
        UMI_HEYGEN_GET_VIDEO,
        UMI_HEYGEN_CREATE_AVATAR,
        UMI_HEYGEN_CREATE_VIDEO,
        UMI_HEYGEN_CREATE_SCENE
    } UmiHeyGenOperation;
    /* This draft contains no credential. Unused fields must be empty, avoiding
 * accidental disclosure when an application switches from one action to another.
 * Script accepts 8192 UTF-8 bytes; character prompts and motion accept 1000.
 * Character creation here means a synthetic presenter from text, not training
 * a digital twin from footage. Existing private or public looks can be used.
 * request_key is a unique caller-generated identifier for this one creation. */
    typedef struct UmiHeyGenDraft
    {
        UmiHeyGenOperation operation;
        char resource_id[UMI_HEYGEN_ID_CAPACITY];
        char voice_id[UMI_HEYGEN_ID_CAPACITY];
        char name[UMI_HEYGEN_NAME_CAPACITY];
        char script[8193];
        char motion[1001];
        char cursor[UMI_HEYGEN_CURSOR_CAPACITY];
        char request_key[129];
        /* Scene generation accepts an optional already-uploaded image asset ID.
     * An empty ID selects text-to-video. Image mode follows the image's shape,
     * so portrait must be false. scene_seconds is 5..15 for scenes, zero for
     * other operations. It does not upload or read a local file. */
        char image_asset_id[UMI_HEYGEN_ID_CAPACITY];
        unsigned scene_seconds;
        bool private_catalogue;
        bool portrait;
        bool subtitles;
    } UmiHeyGenDraft;
    typedef struct UmiHeyGenPlan UmiHeyGenPlan;
    typedef struct UmiHeyGenItem
    {
        char id[UMI_HEYGEN_ID_CAPACITY];
        char name[UMI_HEYGEN_NAME_CAPACITY];
        char status[33];
    } UmiHeyGenItem;
    /* Large results belong on the heap. Catalogue pages are never silently cut.
 * video_url may be a temporary signed URL: keep it out of logs, public files
 * and connection settings. No function automatically follows or downloads it. */
    typedef struct UmiHeyGenResult
    {
        unsigned http_status;
        bool creation_may_exist;
        bool creation_confirmed;
        bool has_more;
        size_t count;
        UmiHeyGenItem items[UMI_HEYGEN_PAGE_LIMIT];
        char next_cursor[UMI_HEYGEN_CURSOR_CAPACITY];
        char resource_id[UMI_HEYGEN_ID_CAPACITY];
        char state[33];
        char video_url[4097];
        char subtitle_url[4097];
    } UmiHeyGenResult;

    /* Prepare performs no I/O and copies all fields. *out must be NULL; a live plan
 * is never replaced. Review borrows immutable strings until Destroy. Requests
 * always use https://api.heygen.com; users cannot substitute a credential host.
 * Bodies are JSON. Provider-required route and model identifiers are protocol metadata. */
    UmiStatus UmiHeyGenPrepare(const UmiHeyGenDraft *draft, UmiHeyGenPlan **out);
    UmiStatus UmiHeyGenReview(const UmiHeyGenPlan *plan, const char **out_url, const char **out_body,
                              bool *out_creates_resource);
    void UmiHeyGenDestroy(UmiHeyGenPlan *plan);
    bool UmiHeyGenHttpAvailable(void);
    /* Blocking worker operation. Approval is for this exact reviewed request.
 * Keys are borrowed only for this call and are never kept in the plan/result.
 * Serialize a plan's execution and destruction; cancellation is thread-safe.
 * A creation plan is single-use once transport is entered, even after a timeout.
 * Cancellation stops local waiting, not a remote job. Inspect creation_may_exist
 * on every return and check HeyGen before preparing any replacement request.
 * Reads may be repeated explicitly. There is no automatic retry or polling.
 * Inputs and outputs must not overlap. out is cleared on entry. HTTP/parse
 * failures never expose raw response text. */
    UmiStatus UmiHeyGenExecute(UmiHeyGenPlan *plan, bool approved, const char *api_key,
                               const UmiCancellationToken *cancellation, UmiHeyGenResult *out);
    /* Resolve a previously saved local key only after approval. The profile store
 * is borrowed for this call, must be quiescent, and must outlive it. The entire
 * temporary key is wiped before return, including failed lookups. */
    UmiStatus UmiHeyGenExecuteWithProfile(UmiHeyGenPlan *plan, bool approved, UmiProfileSecrets *secrets,
                                          const char *password, const char *alias,
                                          const UmiCancellationToken *cancellation, UmiHeyGenResult *out);

    /* Trusted transport seam for other platforms and offline regression fixtures.
 * The callback receives a fixed, validated destination and borrowed key/body.
 * It must enforce TLS verification, no redirects, bounded output and no secret
 * logging. Production native hosts should use Execute. Set out_length to the
 * exact received bytes; embedded NUL, incomplete JSON and overflow are refused. */
    typedef UmiStatus (*UmiHeyGenExchange)(void *context, const char *url, const char *body,
                                           const char *request_key, const char *api_key,
                                           const UmiCancellationToken *cancellation, char *reply,
                                           size_t capacity, size_t *out_length, unsigned *out_http_status);
    UmiStatus UmiHeyGenExecuteWithTransport(UmiHeyGenPlan *plan, bool approved, const char *api_key,
                                            const UmiCancellationToken *cancellation,
                                            UmiHeyGenExchange exchange, void *context, UmiHeyGenResult *out);
#ifdef __cplusplus
}
#endif
#endif
