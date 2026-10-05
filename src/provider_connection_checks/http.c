/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/provider_connection_checks/http.c
 * PURPOSE: Perform one bounded model-catalogue request with fixed transport protections.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <stdlib.h>
#include <string.h>
#ifdef UMICOM_CONNECTION_CHECK_HAS_HTTP
#include <curl/curl.h>

typedef struct CheckReply {
    char *body;
    size_t length;
    bool overflow;
    const UmiCancellationToken *cancellation;
} CheckReply;
static size_t Receive(char *bytes, size_t size, size_t count, void *data)
{
    CheckReply *reply = data;
    if (size != 0U && count > SIZE_MAX / size) { reply->overflow = true; return 0U; }
    size_t amount = size * count;
    if (amount >= UMI_CONNECTION_CHECK_BODY_CAPACITY - reply->length) { reply->overflow = true; return 0U; }
    memcpy(reply->body + reply->length, bytes, amount); reply->length += amount;
    reply->body[reply->length] = '\0'; return amount;
}
static int Progress(void *data, curl_off_t total_down, curl_off_t down, curl_off_t total_up, curl_off_t up)
{
    (void)total_down; (void)down; (void)total_up; (void)up;
    return umi_cancellation_token_is_requested(((CheckReply *)data)->cancellation) ? 1 : 0;
}
bool UmiProviderConnectionCheckAvailable(void) { return true; }
/* The catalogue-specific transfer is superseded by UmiConnectionHttpExchange
 * so catalogue and chat share destination restrictions, cancellation and
 * credential retirement. It is retained for engineering review. */
#if 0
UmiStatus UmiConnectionCheckHttp(const UmiProviderConnectionCheckPlan *plan,
    const char *key, const UmiCancellationToken *cancellation, UmiProviderConnectionCheckResult *out)
{
    if (out == NULL || plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    char endpoint[UMI_PROVIDER_CONNECTION_ENDPOINT_CAPACITY]; bool remote = false;
    UmiStatus status = UmiProviderConnectionCheckDescribe(&plan->connection, endpoint, sizeof(endpoint), &remote);
    if (status != UMI_STATUS_OK) return status;
    if (memchr(plan->check_endpoint, '\0', sizeof(plan->check_endpoint)) == NULL ||
        strcmp(endpoint, plan->check_endpoint) != 0 || remote != plan->requires_credential ||
        (remote ? !UmiConnectionCheckKeyValid(key) : key == NULL || key[0] != '\0'))
        return UMI_STATUS_PERMISSION_DENIED;
    if (umi_cancellation_token_is_requested(cancellation)) return UMI_STATUS_CANCELLED;
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) return UMI_STATUS_UNAVAILABLE;
    CURL *curl = curl_easy_init(); struct curl_slist *headers = NULL;
    CheckReply reply = {0}; reply.cancellation = cancellation;
    char authorization[UMI_PLATFORM_SECRET_VALUE_CAPACITY + 32U] = {0};
    reply.body = calloc(UMI_CONNECTION_CHECK_BODY_CAPACITY, 1U);
    if (curl == NULL || reply.body == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto cleanup; }
    headers = curl_slist_append(NULL, "Accept: application/json");
    if (headers == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto cleanup; }
    if (remote) {
        strcpy(authorization, "Authorization: Bearer "); strcat(authorization, key);
        struct curl_slist *next = curl_slist_append(headers, authorization);
        if (next == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto cleanup; }
        headers = next;
    }
    /* Check every option. A failed restriction must stop the operation instead
     * of falling back to curl defaults or a different authentication route. */
#define CHECK_CURL(option, value) do { if (curl_easy_setopt(curl, option, value) != CURLE_OK) { status = UMI_STATUS_UNAVAILABLE; goto cleanup; } } while (0)
    CHECK_CURL(CURLOPT_URL, endpoint);
    CHECK_CURL(CURLOPT_HTTPGET, 1L); CHECK_CURL(CURLOPT_HTTPHEADER, headers);
    CHECK_CURL(CURLOPT_PROXY, ""); CHECK_CURL(CURLOPT_NOPROXY, "*");
    CHECK_CURL(CURLOPT_NETRC, (long)CURL_NETRC_IGNORED);
    CHECK_CURL(CURLOPT_FOLLOWLOCATION, 0L); CHECK_CURL(CURLOPT_MAXREDIRS, 0L);
    CHECK_CURL(CURLOPT_PROTOCOLS_STR, remote ? "https" : "http");
    CHECK_CURL(CURLOPT_REDIR_PROTOCOLS_STR, remote ? "https" : "http");
    CHECK_CURL(CURLOPT_SSL_VERIFYPEER, 1L); CHECK_CURL(CURLOPT_SSL_VERIFYHOST, 2L);
    CHECK_CURL(CURLOPT_VERBOSE, 0L); CHECK_CURL(CURLOPT_NOSIGNAL, 1L);
    CHECK_CURL(CURLOPT_CONNECTTIMEOUT_MS, (long)(plan->connection.timeout_ms < 5000U ? plan->connection.timeout_ms : 5000U));
    CHECK_CURL(CURLOPT_TIMEOUT_MS, (long)plan->connection.timeout_ms);
    CHECK_CURL(CURLOPT_WRITEFUNCTION, Receive); CHECK_CURL(CURLOPT_WRITEDATA, &reply);
    CHECK_CURL(CURLOPT_NOPROGRESS, 0L); CHECK_CURL(CURLOPT_XFERINFOFUNCTION, Progress);
    CHECK_CURL(CURLOPT_XFERINFODATA, &reply);
    CURLcode code = curl_easy_perform(curl);
    if (code != CURLE_OK) {
        status = reply.overflow ? UMI_STATUS_CAPACITY_EXCEEDED :
            umi_cancellation_token_is_requested(cancellation) ? UMI_STATUS_CANCELLED :
            code == CURLE_OPERATION_TIMEDOUT ? UMI_STATUS_TIMEOUT : UMI_STATUS_IO_ERROR;
        goto cleanup;
    }
    long http = 0L;
    if (curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http) != CURLE_OK || http < 100L || http > 599L) {
        status = UMI_STATUS_IO_ERROR; goto cleanup;
    }
    out->http_status = (unsigned)http;
    if (umi_cancellation_token_is_requested(cancellation)) { status = UMI_STATUS_CANCELLED; goto cleanup; }
    if (http != 200L) {
        status = http == 401L || http == 403L ? UMI_STATUS_PERMISSION_DENIED :
            http == 404L ? UMI_STATUS_NOT_FOUND : http == 429L || http == 503L ? UMI_STATUS_BUSY : UMI_STATUS_IO_ERROR;
        goto cleanup;
    }
    status = UmiConnectionCheckDecode(reply.body, reply.length, plan->connection.model, out);
    out->http_status = (unsigned)http;
cleanup:
    if (curl != NULL) curl_easy_cleanup(curl);
    /* libcurl borrows this header list. Retire the handle before wiping our
     * copies; library-internal TLS buffers are outside this owner's control. */
    for (struct curl_slist *item = headers; item != NULL; item = item->next)
        if (item->data != NULL) umi_secret_clear(item->data, strlen(item->data));
    curl_slist_free_all(headers);
    umi_secret_clear(authorization, sizeof(authorization));
    if (reply.body != NULL) { umi_secret_clear(reply.body, UMI_CONNECTION_CHECK_BODY_CAPACITY); free(reply.body); }
    curl_global_cleanup(); return status;
#undef CHECK_CURL
}
#endif
UmiStatus UmiConnectionHttpExchange(const UmiProviderConnectionCheckPlan *plan,
    const char *request_body, const char *key, const UmiCancellationToken *cancellation,
    char *out_body, size_t capacity, unsigned *out_http_status)
{
    if (out_body == NULL || capacity == 0U || out_http_status == NULL || plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    out_body[0] = '\0'; *out_http_status = 0U;
    size_t request_length = 0U;
    if (request_body != NULL) {
        while (request_length < UMI_CONNECTION_CHECK_BODY_CAPACITY && request_body[request_length] != '\0') ++request_length;
        if (request_length == 0U || request_length == UMI_CONNECTION_CHECK_BODY_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    char endpoint[UMI_PROVIDER_CONNECTION_ENDPOINT_CAPACITY]; bool remote = false;
    UmiStatus status = UmiProviderConnectionCheckDescribe(&plan->connection, endpoint, sizeof(endpoint), &remote);
    if (status != UMI_STATUS_OK) return status;
    if (memchr(plan->check_endpoint, '\0', sizeof(plan->check_endpoint)) == NULL ||
        strcmp(endpoint, plan->check_endpoint) != 0 || remote != plan->requires_credential ||
        (remote ? !UmiConnectionCheckKeyValid(key) : key == NULL || key[0] != '\0'))
        return UMI_STATUS_PERMISSION_DENIED;
    /* The saved destination was checked against an exact provider policy.
     * POST never accepts an independent caller-supplied URL. */
    if (request_body != NULL) strcpy(endpoint, plan->connection.endpoint);
    if (umi_cancellation_token_is_requested(cancellation)) return UMI_STATUS_CANCELLED;
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) return UMI_STATUS_UNAVAILABLE;
    CURL *curl = curl_easy_init(); struct curl_slist *headers = NULL;
    CheckReply reply = {0}; reply.cancellation = cancellation;
    char authorization[UMI_PLATFORM_SECRET_VALUE_CAPACITY + 32U] = {0};
    reply.body = calloc(UMI_CONNECTION_CHECK_BODY_CAPACITY, 1U);
    if (curl == NULL || reply.body == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto cleanup; }
    headers = curl_slist_append(NULL, "Accept: application/json");
    if (headers == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto cleanup; }
    if (request_body != NULL) {
        struct curl_slist *next = curl_slist_append(headers, "Content-Type: application/json");
        if (next == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto cleanup; }
        headers = next;
    }
    if (remote) {
        strcpy(authorization, "Authorization: Bearer "); strcat(authorization, key);
        struct curl_slist *next = curl_slist_append(headers, authorization);
        if (next == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto cleanup; }
        headers = next;
    }
    /* Check every option. A failed restriction must stop the operation instead
     * of falling back to curl defaults or a different authentication route. */
#define CHECK_CURL(option, value) do { if (curl_easy_setopt(curl, option, value) != CURLE_OK) { status = UMI_STATUS_UNAVAILABLE; goto cleanup; } } while (0)
    CHECK_CURL(CURLOPT_URL, endpoint);
    if (request_body == NULL) CHECK_CURL(CURLOPT_HTTPGET, 1L);
    else {
        CHECK_CURL(CURLOPT_POST, 1L);
        CHECK_CURL(CURLOPT_POSTFIELDS, request_body);
        CHECK_CURL(CURLOPT_POSTFIELDSIZE, (long)request_length);
    }
    CHECK_CURL(CURLOPT_HTTPHEADER, headers);
    CHECK_CURL(CURLOPT_PROXY, ""); CHECK_CURL(CURLOPT_NOPROXY, "*");
    CHECK_CURL(CURLOPT_NETRC, (long)CURL_NETRC_IGNORED);
    CHECK_CURL(CURLOPT_FOLLOWLOCATION, 0L); CHECK_CURL(CURLOPT_MAXREDIRS, 0L);
    CHECK_CURL(CURLOPT_PROTOCOLS_STR, remote ? "https" : "http");
    CHECK_CURL(CURLOPT_REDIR_PROTOCOLS_STR, remote ? "https" : "http");
    CHECK_CURL(CURLOPT_SSL_VERIFYPEER, 1L); CHECK_CURL(CURLOPT_SSL_VERIFYHOST, 2L);
    CHECK_CURL(CURLOPT_VERBOSE, 0L); CHECK_CURL(CURLOPT_NOSIGNAL, 1L);
    CHECK_CURL(CURLOPT_CONNECTTIMEOUT_MS, (long)(plan->connection.timeout_ms < 5000U ? plan->connection.timeout_ms : 5000U));
    CHECK_CURL(CURLOPT_TIMEOUT_MS, (long)plan->connection.timeout_ms);
    CHECK_CURL(CURLOPT_WRITEFUNCTION, Receive); CHECK_CURL(CURLOPT_WRITEDATA, &reply);
    CHECK_CURL(CURLOPT_NOPROGRESS, 0L); CHECK_CURL(CURLOPT_XFERINFOFUNCTION, Progress);
    CHECK_CURL(CURLOPT_XFERINFODATA, &reply);
    CURLcode code = curl_easy_perform(curl);
    if (code != CURLE_OK) {
        status = reply.overflow ? UMI_STATUS_CAPACITY_EXCEEDED :
            umi_cancellation_token_is_requested(cancellation) ? UMI_STATUS_CANCELLED :
            code == CURLE_OPERATION_TIMEDOUT ? UMI_STATUS_TIMEOUT : UMI_STATUS_IO_ERROR;
        goto cleanup;
    }
    long http = 0L;
    if (curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http) != CURLE_OK || http < 100L || http > 599L) {
        status = UMI_STATUS_IO_ERROR; goto cleanup;
    }
    *out_http_status = (unsigned)http;
    if (umi_cancellation_token_is_requested(cancellation)) { status = UMI_STATUS_CANCELLED; goto cleanup; }
    if (http != 200L) {
        status = http == 401L || http == 403L ? UMI_STATUS_PERMISSION_DENIED :
            http == 404L ? UMI_STATUS_NOT_FOUND : http == 429L || http == 503L ? UMI_STATUS_BUSY : UMI_STATUS_IO_ERROR;
        goto cleanup;
    }
    if (memchr(reply.body, '\0', reply.length) != NULL) status = UMI_STATUS_PARSE_ERROR;
    else if (reply.length >= capacity) status = UMI_STATUS_CAPACITY_EXCEEDED;
    else { memcpy(out_body, reply.body, reply.length + 1U); status = UMI_STATUS_OK; }
cleanup:
    if (curl != NULL) curl_easy_cleanup(curl);
    /* libcurl borrows this header list. Retire the handle before wiping our
     * copies; library-internal TLS buffers are outside this owner's control. */
    for (struct curl_slist *item = headers; item != NULL; item = item->next)
        if (item->data != NULL) umi_secret_clear(item->data, strlen(item->data));
    curl_slist_free_all(headers);
    umi_secret_clear(authorization, sizeof(authorization));
    if (reply.body != NULL) { umi_secret_clear(reply.body, UMI_CONNECTION_CHECK_BODY_CAPACITY); free(reply.body); }
    curl_global_cleanup(); return status;
#undef CHECK_CURL
}
/* Keep catalogue decoding separate from transfer ownership. Both operations
 * use the same bounded HTTP policy and retire credential headers identically. */
UmiStatus UmiConnectionCheckHttp(const UmiProviderConnectionCheckPlan *plan,
    const char *key, const UmiCancellationToken *cancellation, UmiProviderConnectionCheckResult *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    char *body = calloc(UMI_CONNECTION_CHECK_BODY_CAPACITY, 1U);
    if (body == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    unsigned http = 0U;
    UmiStatus status = UmiConnectionHttpExchange(plan, NULL, key, cancellation,
        body, UMI_CONNECTION_CHECK_BODY_CAPACITY, &http);
    if (status == UMI_STATUS_OK) status = UmiConnectionCheckDecode(body, strlen(body), plan->connection.model, out);
    out->http_status = http;
    umi_secret_clear(body, UMI_CONNECTION_CHECK_BODY_CAPACITY); free(body); return status;
}

#else
bool UmiProviderConnectionCheckAvailable(void) { return false; }
UmiStatus UmiConnectionHttpExchange(const UmiProviderConnectionCheckPlan *plan,
    const char *request_body, const char *key, const UmiCancellationToken *cancellation,
    char *out_body, size_t capacity, unsigned *out_http_status)
{
    (void)plan; (void)request_body; (void)key; (void)cancellation;
    if (out_body == NULL || capacity == 0U || out_http_status == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    out_body[0] = '\0'; *out_http_status = 0U; return UMI_STATUS_UNAVAILABLE;
}

UmiStatus UmiConnectionCheckHttp(const UmiProviderConnectionCheckPlan *plan,
    const char *key, const UmiCancellationToken *cancellation, UmiProviderConnectionCheckResult *out)
{
    (void)plan; (void)key; (void)cancellation;
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out)); return UMI_STATUS_UNAVAILABLE;
}
#endif
