/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_seedream.c
 * PURPOSE: Exercise request approval, complete replies and decoded image ownership offline.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/media_generation/seedream.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                                  \
            exit(1);                                                                                         \
        }                                                                                                    \
    } while (0)
static const char *scenario;
static unsigned calls;
static const char encoded_png[] = "iVBORw0KGgoAAAANSUhEUgAAAAIAAAACCAYAAABytg0kAAAAFElEQVR4nGP4z8DwHwgbGMA0EA"
                                  "AAP9cIeY8TiooAAAAASUVORK5CYII=";
static UmiStatus Exchange(void *context, const char *url, const char *body, const char *key,
                          const UmiCancellationToken *cancel, char *reply, size_t capacity, size_t *length,
                          unsigned *http)
{
    (void)context;
    ++calls;
    CHECK(!strcmp(url, "https://ark.ap-southeast.bytepluses.com/api/v3/images/generations"));
    CHECK(!strcmp(key, "fixture-key"));
    CHECK(strstr(body, "\"response_format\":\"b64_json\""));
    CHECK(strstr(body, "\"output_format\":\"png\"") && strstr(body, "\"watermark\":true"));
    *http = 200;
    if (!strcmp(scenario, "timeout"))
        return UMI_STATUS_TIMEOUT;
    if (!strcmp(scenario, "redirect"))
        *http = 302;
    if (!strcmp(scenario, "denied"))
        *http = 403;
    if (!strcmp(scenario, "rate-limit"))
        *http = 429;
    if (!strcmp(scenario, "overflow"))
    {
        *length = capacity + 1U;
        return UMI_STATUS_OK;
    }
    if (!strcmp(scenario, "empty"))
    {
        *length = 0;
        return UMI_STATUS_OK;
    }
    if (!strcmp(scenario, "cancel-response"))
        umi_cancellation_token_request((UmiCancellationToken *)cancel);
    const char *base64 = encoded_png;
    if (!strcmp(scenario, "pad-bits"))
        base64 = "AB==";
    if (!strcmp(scenario, "bad-padding"))
        base64 = "AAA=AQ==";
    if (!strcmp(scenario, "bad-alphabet"))
        base64 = "AA?=";
    if (!strcmp(scenario, "non-png"))
        base64 = "YWJjZA==";
    if (!strcmp(scenario, "base64-length"))
        base64 = "AAAAA";
    const char *field = !strcmp(scenario, "url-only") ? "url" : "b64_json";
    int n = snprintf(reply, capacity, "{\"model\":\"fixture-model\",\"data\":[{\"%s\":\"%s\"%s}]%s}", field,
                     base64, !strcmp(scenario, "wrong-format") ? ",\"output_format\":\"jpeg\"" : "",
                     !strcmp(scenario, "duplicate-data") ? ",\"data\":[]" : "");
    CHECK(n > 0 && (size_t)n < capacity);
    *length = (size_t)n;
    if (!strcmp(scenario, "provider-error"))
        strcpy(reply, "{\"error\":{\"message\":\"private provider detail\"}}");
    if (!strcmp(scenario, "multiple"))
        strcpy(reply, "{\"model\":\"m\",\"data\":[{},{}]}");
    if (!strcmp(scenario, "wrong-data-type"))
        strcpy(reply, "{\"model\":\"m\",\"data\":{}}");
    if (!strcmp(scenario, "duplicate-image"))
        strcpy(reply, "{\"model\":\"m\",\"data\":[{\"b64_json\":\"AA==\",\"b64_json\":\"AA==\"}]}");
    if (!strcmp(scenario, "duplicate-model"))
        strcpy(reply, "{\"model\":\"m\",\"model\":\"n\",\"data\":[]}");
    if (!strcmp(scenario, "wrong-image-type"))
        strcpy(reply, "{\"model\":\"m\",\"data\":[{\"b64_json\":12}]}");
    if (!strcmp(scenario, "escaped-nul"))
        strcpy(reply, "{\"model\":\"m\\u0000\",\"data\":[]}");
    if (!strcmp(scenario, "image-error"))
        strcpy(reply, "{\"model\":\"m\",\"data\":[{\"error\":{}}]}");
    *length = strlen(reply);
    if (!strcmp(scenario, "trailing"))
    {
        reply[(*length)++] = 'x';
    }
    if (!strcmp(scenario, "embedded-nul"))
        reply[2] = '\0';
    return UMI_STATUS_OK;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    scenario = argv[1];
    UmiSeedreamDraft d = {0};
    strcpy(d.model, "fixture-model");
    strcpy(d.prompt, "A quiet library, soft light");
    if (!strcmp(scenario, "empty-prompt"))
        strcpy(d.prompt, " \t\n");
    if (!strcmp(scenario, "unterminated"))
        memset(d.prompt, 'x', sizeof(d.prompt));
    if (!strcmp(scenario, "invalid-utf8"))
    {
        d.prompt[0] = (char)0xff;
        d.prompt[1] = 0;
    }
    if (!strcmp(scenario, "invalid-model"))
        strcpy(d.model, "model\"secret");
    if (!strcmp(scenario, "prompt-limit"))
    {
        memset(d.prompt, 'x', sizeof(d.prompt) - 1U);
        d.prompt[sizeof(d.prompt) - 1U] = 0;
    }
    if (!strcmp(scenario, "quoted-prompt"))
        strcpy(d.prompt, "Say \"hello\"\nA path \\ kept as text");
    UmiSeedreamPlan *plan = NULL;
    UmiStatus status = UmiSeedreamPrepare(&d, &plan);
    bool invalid = !strcmp(scenario, "empty-prompt") || !strcmp(scenario, "unterminated") ||
                   !strcmp(scenario, "invalid-utf8") || !strcmp(scenario, "invalid-model");
    if (invalid)
    {
        CHECK(status != UMI_STATUS_OK && !plan);
        return 0;
    }
    CHECK(status == UMI_STATUS_OK && plan);
    const char *url = NULL, *body = NULL;
    CHECK(UmiSeedreamReview(plan, &url, &body) == UMI_STATUS_OK);
    if (!strcmp(scenario, "immutable"))
    {
        strcpy(d.prompt, "changed");
        CHECK(!strstr(body, "changed"));
    }
    if (!strcmp(scenario, "quoted-prompt"))
        CHECK(strstr(body, "\\\"hello\\\"") && strstr(body, "\\n"));
    bool prepare_only = !strcmp(scenario, "immutable") || !strcmp(scenario, "quoted-prompt") ||
                        !strcmp(scenario, "prompt-limit");
    if (prepare_only)
    {
        UmiSeedreamDestroy(plan);
        return 0;
    }
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    UmiSeedreamResult result = {0};
    if (!strcmp(scenario, "cancel-before"))
        umi_cancellation_token_request(cancel);
    bool approved = strcmp(scenario, "unapproved") != 0;
    const char *key = !strcmp(scenario, "header-injection") ? "x\r\nsecret:y" : "fixture-key";
    bool blocked = !approved || !strcmp(scenario, "header-injection") || !strcmp(scenario, "cancel-before");
    if (!blocked && !UmiMediaPngAvailable())
    {
        UmiSeedreamDestroy(plan);
        umi_cancellation_token_destroy(cancel);
        return 77;
    }
    status = UmiSeedreamExecuteWithTransport(plan, approved, key, cancel, Exchange, NULL, &result);
    bool success = !strcmp(scenario, "success") || !strcmp(scenario, "single-use");
    if (success)
    {
        CHECK(status == UMI_STATUS_OK && result.image && result.request_may_have_run &&
              result.http_status == 200);
        UmiMediaRgbaPixel pixel;
        CHECK(umi_media_image_surface_get_pixel(result.image, 0, 0, &pixel) == UMI_STATUS_OK);
        CHECK(pixel.red == 255U && pixel.green == 0U && pixel.blue == 0U && pixel.alpha == 255U);
        CHECK(UmiSeedreamExecuteWithTransport(plan, true, key, cancel, Exchange, NULL, &result) ==
              UMI_STATUS_INVALID_ARGUMENT);
        UmiSeedreamResultClear(&result);
        CHECK(!result.image && !result.model[0]);
    }
    else
        CHECK(status != UMI_STATUS_OK && !result.image && !result.model[0]);
    if (blocked)
        CHECK(calls == 0 && !result.request_may_have_run);
    else
    {
        CHECK(calls == 1);
        if (!success)
            CHECK(result.request_may_have_run);
        CHECK(UmiSeedreamExecuteWithTransport(plan, true, key, cancel, Exchange, NULL, &result) ==
              UMI_STATUS_INVALID_STATE);
        CHECK(calls == 1);
    }
    UmiSeedreamResultClear(&result);
    UmiSeedreamDestroy(plan);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
