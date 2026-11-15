/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/local_chat_provider.c
 *
 * PURPOSE:
 *   Contact an explicitly configured loopback chat server without redirects, proxies or automatic tool execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "local_chat_internal.h"
#include "local_http_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef UMICOM_AI_WORKSPACE_HAS_HTTP
#include <curl/curl.h>

typedef struct AwLocalProvider {
    uint16_t port; uint32_t timeoutMillis;
    const UmiAiWorkspaceCancellation *cancellation;
} AwLocalProvider;
typedef struct AwHttpReply { char *text; size_t size; bool overflow; size_t capacity; } AwHttpReply;
static size_t AwHttpWrite(char *data, size_t size, size_t count, void *context)
{
    AwHttpReply *reply = context;
    if (size != 0U && count > SIZE_MAX / size) { reply->overflow = true; return 0U; }
    size_t bytes = size * count;
    /* Retain the former chat-only bound; each fixed route now selects its own
     * smaller-or-equal response capacity before any write callback. */
#if 0
    if (bytes >= AW_HTTP_BODY_CAPACITY - reply->size) { reply->overflow = true; return 0U; }
#endif
    if (reply->size >= reply->capacity || bytes >= reply->capacity - reply->size) {
        reply->overflow = true; return 0U;
    }
    memcpy(reply->text + reply->size, data, bytes); reply->size += bytes;
    reply->text[reply->size] = '\0'; return bytes;
}
static int AwHttpProgress(void *context, curl_off_t downloadTotal, curl_off_t downloaded,
    curl_off_t uploadTotal, curl_off_t uploaded)
{
    const AwLocalProvider *provider = context;
    (void)downloadTotal; (void)downloaded; (void)uploadTotal; (void)uploaded;
    return UmiAiWorkspaceCancellationIsRequested(provider->cancellation) ? 1 : 0;
}
static void AwLocalDestroy(void *instance)
{
    free(instance); curl_global_cleanup();
}
/* Preserve the original POST-only implementation for engineering review.
 * AwLocalExchange now owns its transport policy for both fixed routes; the
 * replacement below keeps request encoding, response decoding and statuses. */
#if 0
static UmiStatus AwLocalGenerate(void *instance, const UmiAiRequest *request, UmiAiResponse *response)
{
    AwLocalProvider *provider = instance; CURL *curl = NULL; struct curl_slist *headers = NULL;
    char url[96], *body = NULL; AwHttpReply reply = {0}; UmiStatus status;
    if (provider == NULL || response == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiAiWorkspaceCancellationIsRequested(provider->cancellation)) return UMI_STATUS_CANCELLED;
    status = AwLocalChatEncode(request, &body); if (status != UMI_STATUS_OK) return status;
    reply.text = calloc(AW_HTTP_BODY_CAPACITY, 1U); curl = curl_easy_init();
    if (reply.text == NULL || curl == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto cleanup; }
    headers = curl_slist_append(NULL, "Content-Type: application/json");
    if (headers == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto cleanup; }
    (void)snprintf(url, sizeof(url), "http://127.0.0.1:%u/v1/chat/completions", (unsigned)provider->port);
    /* Every option is checked: failure cannot silently turn a restricted
     * request into a different transport policy. No shell command is built. */
#define AW_CURL(option, value) do { if (curl_easy_setopt(curl, option, value) != CURLE_OK) { status = UMI_STATUS_UNAVAILABLE; goto cleanup; } } while (0)
    AW_CURL(CURLOPT_URL, url); AW_CURL(CURLOPT_HTTPHEADER, headers);
    AW_CURL(CURLOPT_POST, 1L); AW_CURL(CURLOPT_POSTFIELDS, body);
    AW_CURL(CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)strlen(body));
    AW_CURL(CURLOPT_PROXY, ""); AW_CURL(CURLOPT_NOPROXY, "*");
    AW_CURL(CURLOPT_FOLLOWLOCATION, 0L); AW_CURL(CURLOPT_MAXREDIRS, 0L);
    AW_CURL(CURLOPT_PROTOCOLS_STR, "http"); AW_CURL(CURLOPT_REDIR_PROTOCOLS_STR, "http");
    AW_CURL(CURLOPT_NETRC, (long)CURL_NETRC_IGNORED); AW_CURL(CURLOPT_NOSIGNAL, 1L);
    AW_CURL(CURLOPT_CONNECTTIMEOUT_MS, (long)(provider->timeoutMillis < 3000U ? provider->timeoutMillis : 3000U));
    AW_CURL(CURLOPT_TIMEOUT_MS, (long)provider->timeoutMillis);
    AW_CURL(CURLOPT_WRITEFUNCTION, AwHttpWrite); AW_CURL(CURLOPT_WRITEDATA, &reply);
    AW_CURL(CURLOPT_NOPROGRESS, 0L); AW_CURL(CURLOPT_XFERINFOFUNCTION, AwHttpProgress);
    AW_CURL(CURLOPT_XFERINFODATA, provider);
    CURLcode code = curl_easy_perform(curl); long httpStatus = 0L;
    if (code != CURLE_OK) {
        status = reply.overflow ? UMI_STATUS_CAPACITY_EXCEEDED : code == CURLE_OPERATION_TIMEDOUT ? UMI_STATUS_TIMEOUT :
            code == CURLE_ABORTED_BY_CALLBACK ? UMI_STATUS_CANCELLED : UMI_STATUS_IO_ERROR;
        goto cleanup;
    }
    if (curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpStatus) != CURLE_OK) { status = UMI_STATUS_IO_ERROR; goto cleanup; }
    if (httpStatus != 200L) {
        status = httpStatus == 429L || httpStatus == 503L ? UMI_STATUS_BUSY : UMI_STATUS_IO_ERROR;
        goto cleanup;
    }
    status = AwLocalChatDecode(request, reply.text, reply.size, response);
cleanup:
    if (headers != NULL) curl_slist_free_all(headers);
    if (curl != NULL) curl_easy_cleanup(curl);
    free(reply.text); AwLocalChatFree(body); return status;
#undef AW_CURL
}
#endif
/* The exchange owns transport policy for both fixed routes. Keeping GET and
 * POST here prevents a health client from quietly gaining redirect, proxy or
 * credential behavior that the chat client explicitly excludes. */
/* Move fixed loopback configuration into one shared helper so full replies, health and streaming cannot drift in endpoint, credential or proxy policy; preserve response/cancellation behavior. The former implementation is retained for owner review. */
#if 0
static UmiStatus AwLocalExchange(const AwLocalProvider *provider, const char *body,
    size_t capacity, AwHttpReply *reply, long *httpStatus)
{
    CURL *curl = NULL; struct curl_slist *headers = NULL;
    char url[96]; UmiStatus status = UMI_STATUS_OK;
    reply->capacity = capacity;
    reply->text = calloc(capacity, 1U); curl = curl_easy_init();
    if (reply->text == NULL || curl == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto cleanup; }
    if (body != NULL) {
        headers = curl_slist_append(NULL, "Content-Type: application/json");
        if (headers == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto cleanup; }
    }
    (void)snprintf(url, sizeof(url), "http://127.0.0.1:%u%s",
        (unsigned)provider->port, body == NULL ? "/health" : "/v1/chat/completions");
#define AW_CURL(option, value) do { if (curl_easy_setopt(curl, option, value) != CURLE_OK) { status = UMI_STATUS_UNAVAILABLE; goto cleanup; } } while (0)
    AW_CURL(CURLOPT_URL, url);
    if (body != NULL) {
        AW_CURL(CURLOPT_HTTPHEADER, headers); AW_CURL(CURLOPT_POST, 1L);
        AW_CURL(CURLOPT_POSTFIELDS, body);
        AW_CURL(CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)strlen(body));
    } else {
        AW_CURL(CURLOPT_HTTPGET, 1L);
    }
    AW_CURL(CURLOPT_PROXY, ""); AW_CURL(CURLOPT_NOPROXY, "*");
    AW_CURL(CURLOPT_FOLLOWLOCATION, 0L); AW_CURL(CURLOPT_MAXREDIRS, 0L);
    AW_CURL(CURLOPT_PROTOCOLS_STR, "http"); AW_CURL(CURLOPT_REDIR_PROTOCOLS_STR, "http");
    AW_CURL(CURLOPT_NETRC, (long)CURL_NETRC_IGNORED); AW_CURL(CURLOPT_NOSIGNAL, 1L);
    AW_CURL(CURLOPT_CONNECTTIMEOUT_MS, (long)(provider->timeoutMillis < 3000U ? provider->timeoutMillis : 3000U));
    AW_CURL(CURLOPT_TIMEOUT_MS, (long)provider->timeoutMillis);
    AW_CURL(CURLOPT_WRITEFUNCTION, AwHttpWrite); AW_CURL(CURLOPT_WRITEDATA, reply);
    AW_CURL(CURLOPT_NOPROGRESS, 0L); AW_CURL(CURLOPT_XFERINFOFUNCTION, AwHttpProgress);
    AW_CURL(CURLOPT_XFERINFODATA, provider);
    CURLcode code = curl_easy_perform(curl);
    if (code != CURLE_OK) {
        status = reply->overflow ? UMI_STATUS_CAPACITY_EXCEEDED : code == CURLE_OPERATION_TIMEDOUT ? UMI_STATUS_TIMEOUT :
            code == CURLE_ABORTED_BY_CALLBACK ? UMI_STATUS_CANCELLED : UMI_STATUS_IO_ERROR;
        goto cleanup;
    }
    if (curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, httpStatus) != CURLE_OK)
        status = UMI_STATUS_IO_ERROR;
    /* Cancellation owns the final publication boundary too: a late request
     * must not publish a newly successful health observation or chat draft. */
    if (status == UMI_STATUS_OK && UmiAiWorkspaceCancellationIsRequested(provider->cancellation))
        status = UMI_STATUS_CANCELLED;
cleanup:
    if (headers != NULL) curl_slist_free_all(headers);
    if (curl != NULL) curl_easy_cleanup(curl);
    return status;
#undef AW_CURL
}
#endif
static UmiStatus AwLocalExchange(const AwLocalProvider *provider,const char *body,
    size_t capacity,AwHttpReply *reply,long *httpStatus)
{
    CURL *curl=NULL;struct curl_slist *headers=NULL;UmiStatus status=UMI_STATUS_OK;
    reply->capacity=capacity;reply->text=calloc(capacity,1U);curl=curl_easy_init();
    if(reply->text==NULL || curl==NULL){status=UMI_STATUS_OUT_OF_MEMORY;goto cleanup;}
    if(body!=NULL){
        headers=curl_slist_append(NULL,"Content-Type: application/json");
        if(headers==NULL){status=UMI_STATUS_OUT_OF_MEMORY;goto cleanup;}
    }
    const AwLocalHttpCallbacks callbacks={.write=AwHttpWrite,.write_context=reply,
        .progress=AwHttpProgress,.progress_context=(void *)provider};
    status=AwLocalHttpConfigure(curl,provider->port,provider->timeoutMillis,body,headers,&callbacks,false);
    if(status!=UMI_STATUS_OK)goto cleanup;
    CURLcode code=curl_easy_perform(curl);
    if(code!=CURLE_OK){
        status=reply->overflow?UMI_STATUS_CAPACITY_EXCEEDED:code==CURLE_OPERATION_TIMEDOUT?UMI_STATUS_TIMEOUT:
            code==CURLE_ABORTED_BY_CALLBACK?UMI_STATUS_CANCELLED:UMI_STATUS_IO_ERROR;
        goto cleanup;
    }
    if(curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,httpStatus)!=CURLE_OK)status=UMI_STATUS_IO_ERROR;
    if(status==UMI_STATUS_OK && UmiAiWorkspaceCancellationIsRequested(provider->cancellation))
        status=UMI_STATUS_CANCELLED;
cleanup:
    if(headers!=NULL)curl_slist_free_all(headers);
    if(curl!=NULL)curl_easy_cleanup(curl);
    return status;
}
static UmiStatus AwLocalGenerate(void *instance, const UmiAiRequest *request, UmiAiResponse *response)
{
    AwLocalProvider *provider = instance; char *body = NULL;
    AwHttpReply reply = {0}; long httpStatus = 0L;
    if (provider == NULL || response == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiAiWorkspaceCancellationIsRequested(provider->cancellation)) return UMI_STATUS_CANCELLED;
    UmiStatus status = AwLocalChatEncode(request, &body);
    if (status != UMI_STATUS_OK) return status;
    status = AwLocalExchange(provider, body, AW_HTTP_BODY_CAPACITY, &reply, &httpStatus);
    if (status == UMI_STATUS_OK && httpStatus != 200L)
        status = httpStatus == 429L || httpStatus == 503L ? UMI_STATUS_BUSY : UMI_STATUS_IO_ERROR;
    if (status == UMI_STATUS_OK)
        status = AwLocalChatDecode(request, reply.text, reply.size, response);
    free(reply.text); AwLocalChatFree(body);
    return status;
}
UmiStatus UmiAiWorkspaceLocalHealthRead(uint16_t port, uint32_t timeoutMillis,
    const UmiAiWorkspaceCancellation *cancellation, UmiAiLocalHealth *outHealth)
{
    if (outHealth == NULL || port < 1024U || timeoutMillis < 100U || timeoutMillis > 60000U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiAiWorkspaceCancellationIsRequested(cancellation)) return UMI_STATUS_CANCELLED;
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) return UMI_STATUS_UNAVAILABLE;
    const AwLocalProvider provider = {port, timeoutMillis, cancellation};
    AwHttpReply reply = {0}; long httpStatus = 0L;
    UmiStatus status = AwLocalExchange(&provider, NULL, AW_HEALTH_BODY_CAPACITY, &reply, &httpStatus);
    UmiAiLocalHealth observed = {0};
    if (status == UMI_STATUS_OK)
        status = AwLocalHealthDecode(httpStatus, reply.text, reply.size, &observed);
    free(reply.text); curl_global_cleanup();
    if (status == UMI_STATUS_OK && UmiAiWorkspaceCancellationIsRequested(cancellation))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK) *outHealth = observed;
    return status;
}
bool UmiAiWorkspaceLocalProviderAvailable(void) { return true; }
UmiStatus UmiAiWorkspaceLocalProviderCreate(uint16_t port, uint32_t timeoutMillis,
    const UmiAiWorkspaceCancellation *cancellation, UmiAiProvider *outProvider)
{
    if (outProvider == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(outProvider, 0, sizeof(*outProvider));
    if (port < 1024U || timeoutMillis < 100U || timeoutMillis > 60000U) return UMI_STATUS_INVALID_ARGUMENT;
    AwLocalProvider *provider = calloc(1U, sizeof(*provider));
    if (provider == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) { free(provider); return UMI_STATUS_UNAVAILABLE; }
    provider->port = port; provider->timeoutMillis = timeoutMillis; provider->cancellation = cancellation;
    outProvider->structure_size = sizeof(*outProvider); outProvider->abi_version = 1U;
    outProvider->provider_id = "umicom.local-chat"; outProvider->kind = UMI_AI_PROVIDER_LOCAL;
    outProvider->instance = provider; outProvider->generate = AwLocalGenerate; outProvider->destroy = AwLocalDestroy;
    return UMI_STATUS_OK;
}
UmiStatus AwLocalChatProviderSettingsRead(const UmiAiProvider *provider,
    AwLocalProviderSettings *out_settings)
{
    if (out_settings == NULL || umi_ai_provider_validate(provider) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Compare the canonical table before accessing private instance storage.
     * A matching string identifier alone never establishes an adapter type. */
    if (provider->generate != AwLocalGenerate || provider->destroy != AwLocalDestroy ||
        provider->kind != UMI_AI_PROVIDER_LOCAL || provider->instance == NULL ||
        strcmp(provider->provider_id, "umicom.local-chat") != 0)
        return UMI_STATUS_NOT_IMPLEMENTED;
    const AwLocalProvider *local = provider->instance;
    if (local->port < 1024U || local->timeoutMillis < 100U || local->timeoutMillis > 60000U)
        return UMI_STATUS_INVALID_STATE;
    AwLocalProviderSettings snapshot = {local->port, local->timeoutMillis, local->cancellation};
    *out_settings = snapshot;
    return UMI_STATUS_OK;
}
#else
UmiStatus UmiAiWorkspaceLocalHealthRead(uint16_t port, uint32_t timeoutMillis,
    const UmiAiWorkspaceCancellation *cancellation, UmiAiLocalHealth *outHealth)
{
    if (outHealth == NULL || port < 1024U || timeoutMillis < 100U || timeoutMillis > 60000U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiAiWorkspaceCancellationIsRequested(cancellation)) return UMI_STATUS_CANCELLED;
    return UMI_STATUS_UNAVAILABLE;
}
bool UmiAiWorkspaceLocalProviderAvailable(void) { return false; }
UmiStatus UmiAiWorkspaceLocalProviderCreate(uint16_t port, uint32_t timeoutMillis,
    const UmiAiWorkspaceCancellation *cancellation, UmiAiProvider *outProvider)
{
    (void)port; (void)timeoutMillis; (void)cancellation;
    if (outProvider == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(outProvider, 0, sizeof(*outProvider)); return UMI_STATUS_UNAVAILABLE;
}
UmiStatus AwLocalChatProviderSettingsRead(const UmiAiProvider *provider,
    AwLocalProviderSettings *out_settings)
{
    if (out_settings == NULL || umi_ai_provider_validate(provider) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* No canonical HTTP provider can be constructed in this build. Other
     * valid providers keep their existing synchronous dispatch path. */
    return UMI_STATUS_NOT_IMPLEMENTED;
}
#endif
