/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/seedream_http.c
 * PURPOSE: Send a fixed-host image request with bounded output and verified TLS.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "seedream_internal.h"
#include <string.h>
#ifdef UMICOM_SEEDREAM_HAS_HTTP
#include <curl/curl.h>
typedef struct Reply
{
    char *text;
    size_t capacity, used;
    bool overflow;
    const UmiCancellationToken *cancel;
} Reply;
static size_t Receive(char *bytes, size_t size, size_t count, void *context)
{
    Reply *r = context;
    if (umi_cancellation_token_is_requested(r->cancel))
        return 0U;
    if (size != 0U && count > SIZE_MAX / size)
    {
        r->overflow = true;
        return 0U;
    }
    size_t amount = size * count;
    if (amount >= r->capacity - r->used)
    {
        r->overflow = true;
        return 0U;
    }
    memcpy(r->text + r->used, bytes, amount);
    r->used += amount;
    r->text[r->used] = '\0';
    return amount;
}
static int Progress(void *context, curl_off_t a, curl_off_t b, curl_off_t c, curl_off_t d)
{
    (void)a;
    (void)b;
    (void)c;
    (void)d;
    return umi_cancellation_token_is_requested(((Reply *)context)->cancel) ? 1 : 0;
}
static bool Header(struct curl_slist **headers, const char *value)
{
    struct curl_slist *next = curl_slist_append(*headers, value);
    if (next == NULL)
        return false;
    *headers = next;
    return true;
}
bool UmiSeedreamHttpAvailable(void) { return true; }
UmiStatus UmiSeedreamHttp(void *context, const char *url, const char *body, const char *key,
                          const UmiCancellationToken *cancel, char *reply, size_t capacity, size_t *length,
                          unsigned *http)
{
    (void)context;
    /* This function is private: only an immutable prepared plan supplies URL
     * and headers. Verify the fixed origin again at the transport boundary. */
    if (strcmp(url, UMI_SEEDREAM_ENDPOINT) != 0)
        return UMI_STATUS_PERMISSION_DENIED;
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK)
        return UMI_STATUS_UNAVAILABLE;
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;
    UmiStatus status = UMI_STATUS_OK;
    Reply received = {reply, capacity, 0U, false, cancel};
    char auth[2080] = "Authorization: Bearer ";
    strcat(auth, key);
    if (curl == NULL || !Header(&headers, "Accept: application/json") || !Header(&headers, auth))
    {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto done;
    }
    if (body != NULL)
    {
        if (!Header(&headers, "Content-Type: application/json"))
        {
            status = UMI_STATUS_OUT_OF_MEMORY;
            goto done;
        }
    }
#define SET(option, value)                                                                                   \
    do                                                                                                       \
    {                                                                                                        \
        if (curl_easy_setopt(curl, option, value) != CURLE_OK)                                               \
        {                                                                                                    \
            status = UMI_STATUS_UNAVAILABLE;                                                                 \
            goto done;                                                                                       \
        }                                                                                                    \
    } while (0)
    SET(CURLOPT_URL, url);
    SET(CURLOPT_HTTPHEADER, headers);
    SET(CURLOPT_PROXY, "");
    SET(CURLOPT_NOPROXY, "*");
    SET(CURLOPT_NETRC, (long)CURL_NETRC_IGNORED);
    SET(CURLOPT_FOLLOWLOCATION, 0L);
    SET(CURLOPT_MAXREDIRS, 0L);
    SET(CURLOPT_PROTOCOLS_STR, "https");
    SET(CURLOPT_REDIR_PROTOCOLS_STR, "https");
    SET(CURLOPT_SSL_VERIFYPEER, 1L);
    SET(CURLOPT_SSL_VERIFYHOST, 2L);
    SET(CURLOPT_VERBOSE, 0L);
    SET(CURLOPT_NOSIGNAL, 1L);
    SET(CURLOPT_CONNECTTIMEOUT_MS, 5000L);
    SET(CURLOPT_TIMEOUT_MS, 180000L);
    SET(CURLOPT_WRITEFUNCTION, Receive);
    SET(CURLOPT_WRITEDATA, &received);
    SET(CURLOPT_NOPROGRESS, 0L);
    SET(CURLOPT_XFERINFOFUNCTION, Progress);
    SET(CURLOPT_XFERINFODATA, &received);
    if (body == NULL)
        SET(CURLOPT_HTTPGET, 1L);
    else
    {
        SET(CURLOPT_POST, 1L);
        SET(CURLOPT_POSTFIELDS, body);
        SET(CURLOPT_POSTFIELDSIZE, (long)strlen(body));
    }
    if (umi_cancellation_token_is_requested(cancel))
    {
        status = UMI_STATUS_CANCELLED;
        goto done;
    }
    CURLcode code = curl_easy_perform(curl);
    if (code != CURLE_OK)
    {
        status = received.overflow                             ? UMI_STATUS_CAPACITY_EXCEEDED
                 : umi_cancellation_token_is_requested(cancel) ? UMI_STATUS_CANCELLED
                 : code == CURLE_OPERATION_TIMEDOUT            ? UMI_STATUS_TIMEOUT
                                                               : UMI_STATUS_IO_ERROR;
        goto done;
    }
    long response = 0L;
    if (curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response) != CURLE_OK || response < 100L ||
        response > 599L)
        status = UMI_STATUS_IO_ERROR;
    else
    {
        *http = (unsigned)response;
        *length = received.used;
    }
done:
    /* curl borrows the header list until cleanup. Wipe all our copies only
     * after that borrow ends; no raw curl error buffer is exposed to callers. */
    if (curl != NULL)
        curl_easy_cleanup(curl);
    for (struct curl_slist *item = headers; item != NULL; item = item->next)
        if (item->data != NULL)
            umi_secret_clear(item->data, strlen(item->data));
    curl_slist_free_all(headers);
    umi_secret_clear(auth, sizeof(auth));
    curl_global_cleanup();
    return status;
#undef SET
}
#else
bool UmiSeedreamHttpAvailable(void) { return false; }
UmiStatus UmiSeedreamHttp(void *context, const char *url, const char *body, const char *key,
                          const UmiCancellationToken *cancel, char *reply, size_t capacity, size_t *length,
                          unsigned *http)
{
    (void)context;
    (void)url;
    (void)body;
    (void)key;
    (void)cancel;
    (void)reply;
    (void)capacity;
    (void)length;
    (void)http;
    return UMI_STATUS_UNAVAILABLE;
}
#endif
