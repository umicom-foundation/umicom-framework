/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_heygen_http.c
 *
 * PURPOSE:
 *   Inspect native TLS restrictions, allocation failure, chunked replies and credential-header retirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <curl/curl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../src/media_generation/heygen_internal.h"
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #c);                                                  \
            exit(1);                                                                                         \
        }                                                                                                    \
    } while (0)
typedef size_t (*WriteFn)(char *, size_t, size_t, void *);
typedef int (*ProgressFn)(void *, curl_off_t, curl_off_t, curl_off_t, curl_off_t);
typedef struct FakeHttp
{
    WriteFn write;
    ProgressFn progress;
    void *write_data, *progress_data;
    struct curl_slist *headers;
    const char *url, *protocol;
    long peer, host, follow, redirects, netrc, get, timeout, post;
    const char *body;
    bool proxy_off, no_proxy, quiet;
    unsigned perform, cleanup, header_frees;
    unsigned global_inits, global_cleanups, header_attempts;
} FakeHttp;
static FakeHttp fake;
static const char *scenario;
static UmiCancellationToken *cancellation;
typedef struct FakeHeader
{
    struct curl_slist item;
    size_t length;
} FakeHeader;
/* Inject initialization failures to verify ownership without relying on real allocation exhaustion. The former implementation is retained for owner review. */
#if 0
static CURLcode Init(long flags)
{
    (void)flags;
    return CURLE_OK;
}
static void GlobalCleanup(void) {}
static CURL *EasyInit(void) { return (CURL *)(void *)&fake; }

#endif
/* Curl failures are injected before a network operation could occur. Counts
 * prove that only resources actually acquired by the adapter are retired. */
static CURLcode Init(long flags)
{
    CHECK(flags == CURL_GLOBAL_DEFAULT);
    ++fake.global_inits;
    return strcmp(scenario, "init-failure") == 0 ? CURLE_FAILED_INIT : CURLE_OK;
}
static void GlobalCleanup(void) { ++fake.global_cleanups; }
static CURL *EasyInit(void)
{
    return strcmp(scenario, "easy-failure") == 0 ? NULL : (CURL *)(void *)&fake;
}
static bool IsPost(void)
{
    return strcmp(scenario, "post") == 0 ||
           strcmp(scenario, "header-content-failure") == 0 ||
           strcmp(scenario, "header-identity-failure") == 0;
}
static void EasyCleanup(CURL *handle)
{
    CHECK(handle == (CURL *)(void *)&fake);
    ++fake.cleanup;
}
static struct curl_slist *Append(struct curl_slist *list, const char *value)
{
    ++fake.header_attempts;
    unsigned fail_at = strcmp(scenario, "header-accept-failure") == 0 ? 1U
                     : strcmp(scenario, "header-key-failure") == 0 ? 2U
                     : strcmp(scenario, "header-content-failure") == 0 ? 3U
                     : strcmp(scenario, "header-identity-failure") == 0 ? 4U : 0U;
    if (fake.header_attempts == fail_at)
        return NULL;
    FakeHeader *header = calloc(1U, sizeof(*header));
    CHECK(header != NULL);
    header->length = strlen(value);
    header->item.data = malloc(header->length + 1U);
    CHECK(header->item.data != NULL);
    strcpy(header->item.data, value);
    if (list == NULL)
        return &header->item;
    struct curl_slist *last = list;
    while (last->next != NULL)
        last = last->next;
    last->next = &header->item;
    return list;
}
static void FreeHeaders(struct curl_slist *list)
{
    while (list != NULL)
    {
        struct curl_slist *next = list->next;
        FakeHeader *header = (FakeHeader *)(void *)list;
        for (size_t i = 0U; i < header->length; ++i)
            CHECK(list->data[i] == '\0');
        free(list->data);
        free(header);
        list = next;
        ++fake.header_frees;
    }
}
static CURLcode SetOption(CURL *handle, CURLoption option, ...)
{
    (void)handle;
    va_list args;
    va_start(args, option);
    if (strcmp(scenario, "option-failure") == 0 && option == CURLOPT_SSL_VERIFYHOST)
    {
        va_end(args);
        return CURLE_UNKNOWN_OPTION;
    }
    switch (option)
    {
    case CURLOPT_URL:
        fake.url = va_arg(args, const char *);
        break;
    case CURLOPT_PROTOCOLS_STR:
        fake.protocol = va_arg(args, const char *);
        break;
    case CURLOPT_REDIR_PROTOCOLS_STR:
        CHECK(strcmp(va_arg(args, const char *), fake.protocol) == 0);
        break;
    case CURLOPT_PROXY:
        fake.proxy_off = strcmp(va_arg(args, const char *), "") == 0;
        break;
    case CURLOPT_NOPROXY:
        fake.no_proxy = strcmp(va_arg(args, const char *), "*") == 0;
        break;
    case CURLOPT_HTTPHEADER:
        fake.headers = va_arg(args, struct curl_slist *);
        break;
    case CURLOPT_HTTPGET:
        fake.get = va_arg(args, long);
        break;
    case CURLOPT_POST:
        fake.post = va_arg(args, long);
        break;
    case CURLOPT_POSTFIELDS:
        fake.body = va_arg(args, const char *);
        break;
    case CURLOPT_POSTFIELDSIZE:
        CHECK((size_t)va_arg(args, long) == strlen(fake.body));
        break;
    case CURLOPT_SSL_VERIFYPEER:
        fake.peer = va_arg(args, long);
        break;
    case CURLOPT_SSL_VERIFYHOST:
        fake.host = va_arg(args, long);
        break;
    case CURLOPT_FOLLOWLOCATION:
        fake.follow = va_arg(args, long);
        break;
    case CURLOPT_MAXREDIRS:
        fake.redirects = va_arg(args, long);
        break;
    case CURLOPT_NETRC:
        fake.netrc = va_arg(args, long);
        break;
    case CURLOPT_VERBOSE:
        fake.quiet = va_arg(args, long) == 0L;
        break;
    case CURLOPT_TIMEOUT_MS:
        fake.timeout = va_arg(args, long);
        break;
    case CURLOPT_NOSIGNAL:
        CHECK(va_arg(args, long) == 1L);
        break;
    case CURLOPT_NOPROGRESS:
        CHECK(va_arg(args, long) == 0L);
        break;
    case CURLOPT_CONNECTTIMEOUT_MS:
        CHECK(va_arg(args, long) == 5000L);
        break;
    case CURLOPT_WRITEFUNCTION:
        fake.write = va_arg(args, WriteFn);
        break;
    case CURLOPT_WRITEDATA:
        fake.write_data = va_arg(args, void *);
        break;
    case CURLOPT_XFERINFOFUNCTION:
        fake.progress = va_arg(args, ProgressFn);
        break;
    case CURLOPT_XFERINFODATA:
        fake.progress_data = va_arg(args, void *);
        break;
    default:
        CHECK(false);
        break;
    }
    va_end(args);
    return CURLE_OK;
}
static CURLcode Perform(CURL *handle)
{
    (void)handle;
    ++fake.perform;
/* POST header-allocation cases use the same request body and cleanup contract as the normal POST case. The former implementation is retained for owner review. */
#if 0
    bool post = strcmp(scenario, "post") == 0;
#endif
    bool post = IsPost();
    CHECK((post ? fake.post == 1L : fake.get == 1L) && fake.peer == 1L && fake.host == 2L &&
          fake.follow == 0L && fake.redirects == 0L);
    CHECK(fake.proxy_off && fake.no_proxy && fake.quiet && fake.netrc == CURL_NETRC_IGNORED &&
          fake.timeout == 30000L);
    CHECK(strcmp(fake.url, "https://api.heygen.com/v3/videos") == 0);
    CHECK(strcmp(fake.protocol, "https") == 0);
    CHECK(fake.headers && fake.headers->next &&
          strcmp(fake.headers->next->data, "x-api-key: fixture-key") == 0);
    if (post)
    {
        CHECK(fake.body && strcmp(fake.body, "{\"type\":\"avatar\"}") == 0);
        CHECK(fake.headers->next->next && fake.headers->next->next->next);
        CHECK(strcmp(fake.headers->next->next->next->data, "Idempotency-Key: fixture-request") == 0);
    }
    if (strcmp(scenario, "timeout") == 0)
        return CURLE_OPERATION_TIMEDOUT;
    if (strcmp(scenario, "cancel") == 0)
    {
        umi_cancellation_token_request(cancellation);
        CHECK(fake.progress(fake.progress_data, 0, 0, 0, 0) != 0);
        return CURLE_ABORTED_BY_CALLBACK;
    }
    if (strcmp(scenario, "overflow") == 0)
    {
        CHECK(fake.write("x", UMI_HEYGEN_BODY_CAPACITY, 1U, fake.write_data) == 0U);
        return CURLE_WRITE_ERROR;
    }
    if (strcmp(scenario, "transfer-error") == 0)
        return CURLE_RECV_ERROR;
    if (strcmp(scenario, "empty") == 0)
        return CURLE_OK;
    if (strcmp(scenario, "multiply-overflow") == 0)
    {
        CHECK(fake.write("x", 2U, SIZE_MAX, fake.write_data) == 0U);
        return CURLE_WRITE_ERROR;
    }
    if (strcmp(scenario, "cancel-write") == 0)
    {
        umi_cancellation_token_request(cancellation);
        CHECK(fake.write("x", 1U, 1U, fake.write_data) == 0U);
        return CURLE_WRITE_ERROR;
    }
    if (strcmp(scenario, "chunked") == 0 || strcmp(scenario, "chunk-overflow") == 0)
    {
        CHECK(fake.write("abcd", 2U, 2U, fake.write_data) == 4U);
        bool overflow = strcmp(scenario, "chunk-overflow") == 0;
        CHECK(fake.write("efgh", 1U, overflow ? 4U : 3U, fake.write_data) == (overflow ? 0U : 3U));
        return overflow ? CURLE_WRITE_ERROR : CURLE_OK;
    }
    if (strcmp(scenario, "exact-capacity") == 0)
    {
        CHECK(fake.write("abcdefg", 1U, 7U, fake.write_data) == 7U);
        return CURLE_OK;
    }
    char body[] = "{\"data\":{\"video_id\":\"video-1\"}}";
    CHECK(fake.write(body, 1U, strlen(body), fake.write_data) == strlen(body));
    return CURLE_OK;
}
static CURLcode GetInfo(CURL *handle, CURLINFO field, ...)
{
    (void)handle;
    CHECK(field == CURLINFO_RESPONSE_CODE);
    if (strcmp(scenario, "info-failure") == 0)
        return CURLE_UNKNOWN_OPTION;
    va_list args;
    va_start(args, field);
    long *out = va_arg(args, long *);
    *out = strcmp(scenario, "response-low") == 0 ? 99L
           : strcmp(scenario, "response-high") == 0 ? 600L
           : strcmp(scenario, "status-first") == 0 ? 100L
           : strcmp(scenario, "status-last") == 0 ? 599L
           : strcmp(scenario, "redirect") == 0     ? 302L
           : strcmp(scenario, "denied") == 0     ? 401L
           : strcmp(scenario, "rate-limit") == 0 ? 429L
                                                 : 200L;
    va_end(args);
    return CURLE_OK;
}
/* Include the production adapter with only curl calls replaced. No socket,
 * personal credential store, real API key or provider account is accessed. */
#undef curl_easy_setopt
#undef curl_easy_getinfo
#define curl_global_init Init
#define curl_global_cleanup GlobalCleanup
#define curl_easy_init EasyInit
#define curl_easy_cleanup EasyCleanup
#define curl_slist_append Append
#define curl_slist_free_all FreeHeaders
#define curl_easy_setopt SetOption
#define curl_easy_perform Perform
#define curl_easy_getinfo GetInfo
#include "../../src/media_generation/heygen_http.c"

/* Exercise each resource acquisition and reply boundary while retaining the original nine transport scenarios. The former implementation is retained for owner review. */
#if 0
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    scenario = argv[1];
    CHECK(umi_cancellation_token_create(&cancellation) == UMI_STATUS_OK);
    char *reply = calloc(UMI_HEYGEN_BODY_CAPACITY, 1U);
    CHECK(reply);
    size_t length = 0U;
    unsigned http = 0U;
    bool post = strcmp(scenario, "post") == 0;
    UmiStatus status = UmiHeyGenHttp(NULL, "https://api.heygen.com/v3/videos",
                                     post ? "{\"type\":\"avatar\"}" : NULL, "fixture-request", "fixture-key",
                                     cancellation, reply, UMI_HEYGEN_BODY_CAPACITY, &length, &http);
    if (strcmp(scenario, "option-failure") == 0)
        CHECK(status == UMI_STATUS_UNAVAILABLE && fake.perform == 0U);
    else if (strcmp(scenario, "timeout") == 0)
        CHECK(status == UMI_STATUS_TIMEOUT);
    else if (strcmp(scenario, "cancel") == 0)
        CHECK(status == UMI_STATUS_CANCELLED);
    else if (strcmp(scenario, "overflow") == 0)
        CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED);
    else
    {
        CHECK(status == UMI_STATUS_OK && length > 0U);
        CHECK(http == (strcmp(scenario, "redirect") == 0     ? 302U
                       : strcmp(scenario, "denied") == 0     ? 401U
                       : strcmp(scenario, "rate-limit") == 0 ? 429U
                                                             : 200U));
    }
    CHECK(fake.cleanup == 1U && fake.header_frees == (post ? 4U : 2U));
    CHECK(fake.perform == (strcmp(scenario, "option-failure") == 0 ? 0U : 1U));
    free(reply);
    umi_cancellation_token_destroy(cancellation);
    return 0;
}

#endif
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    scenario = argv[1];
    CHECK(umi_cancellation_token_create(&cancellation) == UMI_STATUS_OK);
    bool small = strcmp(scenario, "chunked") == 0 || strcmp(scenario, "chunk-overflow") == 0 ||
                 strcmp(scenario, "exact-capacity") == 0;
    size_t capacity = small ? 8U : UMI_HEYGEN_BODY_CAPACITY;
    /* An extra byte remains a canary; it is never part of the advertised reply. */
    char *reply = malloc(capacity + 1U);
    CHECK(reply);
    memset(reply, 0, capacity);
    reply[capacity] = 'Z';
    size_t length = 42U;
    unsigned http = 777U;
    bool post = IsPost();
    bool before = strcmp(scenario, "cancel-before") == 0;
    if (before)
        umi_cancellation_token_request(cancellation);
    UmiStatus status = UmiHeyGenHttp(NULL, "https://api.heygen.com/v3/videos",
        post ? "{\"type\":\"avatar\"}" : NULL, "fixture-request", "fixture-key",
        cancellation, reply, capacity, &length, &http);
    unsigned expected_headers = post ? 4U : 2U;
    bool early = false;
    if (strcmp(scenario, "init-failure") == 0)
    {
        CHECK(status == UMI_STATUS_UNAVAILABLE && fake.cleanup == 0U &&
              fake.global_cleanups == 0U);
        expected_headers = 0U;
        early = true;
    }
    else if (strcmp(scenario, "easy-failure") == 0)
    {
        CHECK(status == UMI_STATUS_OUT_OF_MEMORY && fake.cleanup == 0U);
        expected_headers = 0U;
        early = true;
    }
    else if (strncmp(scenario, "header-", 7U) == 0)
    {
        CHECK(status == UMI_STATUS_OUT_OF_MEMORY && fake.header_attempts > 0U);
        expected_headers = fake.header_attempts - 1U;
        early = true;
    }
    else if (strcmp(scenario, "option-failure") == 0)
    {
        CHECK(status == UMI_STATUS_UNAVAILABLE);
        early = true;
    }
    else if (strcmp(scenario, "timeout") == 0)
        CHECK(status == UMI_STATUS_TIMEOUT);
    else if (strcmp(scenario, "cancel") == 0 || strcmp(scenario, "cancel-write") == 0 || before)
    {
        CHECK(status == UMI_STATUS_CANCELLED);
        early = before;
    }
    else if (strcmp(scenario, "overflow") == 0 || strcmp(scenario, "multiply-overflow") == 0 ||
             strcmp(scenario, "chunk-overflow") == 0)
        CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED);
    else if (strcmp(scenario, "info-failure") == 0 || strcmp(scenario, "response-low") == 0 ||
             strcmp(scenario, "response-high") == 0 || strcmp(scenario, "transfer-error") == 0)
        CHECK(status == UMI_STATUS_IO_ERROR);
    else
    {
        CHECK(status == UMI_STATUS_OK);
        CHECK(http == (strcmp(scenario, "redirect") == 0 ? 302U
                       : strcmp(scenario, "denied") == 0 ? 401U
                       : strcmp(scenario, "rate-limit") == 0 ? 429U
                       : strcmp(scenario, "status-first") == 0 ? 100U
                       : strcmp(scenario, "status-last") == 0 ? 599U : 200U));
        if (small)
            CHECK(length == 7U && strcmp(reply, "abcdefg") == 0);
        else if (strcmp(scenario, "empty") == 0)
            CHECK(length == 0U && reply[0] == '\0');
        else
            CHECK(length > 0U);
    }
    if (status != UMI_STATUS_OK)
        CHECK(length == 42U && http == 777U);
    CHECK(reply[capacity] == 'Z');
    if (strcmp(scenario, "init-failure") != 0)
    {
        CHECK(fake.global_cleanups == 1U);
        CHECK(fake.cleanup == (strcmp(scenario, "easy-failure") == 0 ? 0U : 1U));
    }
    CHECK(fake.global_inits == 1U && fake.header_frees == expected_headers);
    CHECK(fake.perform == (early ? 0U : 1U));
    free(reply);
    umi_cancellation_token_destroy(cancellation);
    return 0;
}
