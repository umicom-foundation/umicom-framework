/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_pixverse_http.c
 * PURPOSE: Inspect native TLS restrictions, one transfer, cancellation and credential-header retirement.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <curl/curl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../src/media_generation/pixverse_internal.h"
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
} FakeHttp;
static FakeHttp fake;
static const char *scenario;
static UmiCancellationToken *cancellation;
typedef struct FakeHeader
{
    struct curl_slist item;
    size_t length;
} FakeHeader;
static CURLcode Init(long flags)
{
    (void)flags;
    return CURLE_OK;
}
static void GlobalCleanup(void) {}
static CURL *EasyInit(void) { return (CURL *)(void *)&fake; }
static void EasyCleanup(CURL *handle)
{
    CHECK(handle == (CURL *)(void *)&fake);
    ++fake.cleanup;
}
static struct curl_slist *Append(struct curl_slist *list, const char *value)
{
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
    bool post = strcmp(scenario, "post") == 0;
    CHECK((post ? fake.post == 1L : fake.get == 1L) && fake.peer == 1L && fake.host == 2L &&
          fake.follow == 0L && fake.redirects == 0L);
    CHECK(fake.proxy_off && fake.no_proxy && fake.quiet && fake.netrc == CURL_NETRC_IGNORED &&
          fake.timeout == 30000L);
    CHECK(strcmp(fake.url, "https://app-api.pixverse.ai/openapi/v2/video/text/generate") == 0);
    CHECK(strcmp(fake.protocol, "https") == 0);
    CHECK(fake.headers && fake.headers->next &&
          strcmp(fake.headers->next->data, "API-KEY: fixture-key") == 0);
    CHECK(fake.headers->next->next &&
          strcmp(fake.headers->next->next->data, "Ai-trace-id: 93cb2fa6-e5da-4c69-9588-94ca9839f238") == 0);
    if (post)
    {
        CHECK(fake.body && strcmp(fake.body, "{\"prompt\":\"A quiet sea\"}") == 0);
        CHECK(fake.headers->next->next && fake.headers->next->next->next);
        CHECK(strcmp(fake.headers->next->next->data, "Ai-trace-id: 93cb2fa6-e5da-4c69-9588-94ca9839f238") ==
              0);
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
        CHECK(fake.write("x", UMI_PIXVERSE_BODY_CAPACITY, 1U, fake.write_data) == 0U);
        return CURLE_WRITE_ERROR;
    }
    char body[] = "{\"ErrCode\":0,\"Resp\":{\"video_id\":42}}";
    CHECK(fake.write(body, 1U, strlen(body), fake.write_data) == strlen(body));
    return CURLE_OK;
}
static CURLcode GetInfo(CURL *handle, CURLINFO field, ...)
{
    (void)handle;
    CHECK(field == CURLINFO_RESPONSE_CODE);
    va_list args;
    va_start(args, field);
    long *out = va_arg(args, long *);
    *out = strcmp(scenario, "redirect") == 0     ? 302L
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
#include "../../src/media_generation/pixverse_http.c"

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    scenario = argv[1];
    CHECK(umi_cancellation_token_create(&cancellation) == UMI_STATUS_OK);
    char *reply = calloc(UMI_PIXVERSE_BODY_CAPACITY, 1U);
    CHECK(reply);
    size_t length = 0U;
    unsigned http = 0U;
    bool post = strcmp(scenario, "post") == 0;
    UmiStatus status =
        UmiPixVerseHttp(NULL, "https://app-api.pixverse.ai/openapi/v2/video/text/generate",
                        post ? "{\"prompt\":\"A quiet sea\"}" : NULL, "93cb2fa6-e5da-4c69-9588-94ca9839f238",
                        "fixture-key", cancellation, reply, UMI_PIXVERSE_BODY_CAPACITY, &length, &http);
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
    CHECK(fake.cleanup == 1U && fake.header_frees == (post ? 4U : 3U));
    CHECK(fake.perform == (strcmp(scenario, "option-failure") == 0 ? 0U : 1U));
    free(reply);
    umi_cancellation_token_destroy(cancellation);
    return 0;
}
