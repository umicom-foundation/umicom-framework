/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_heygen.c
 * PURPOSE: Exercise reviewed request disclosure, provider reply parsing and one-shot creation without networking.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/media_generation/heygen.h"
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
typedef struct Fake
{
    const char *body;
    UmiStatus status;
    unsigned http, calls;
    size_t length;
    bool cancel_reply;
    UmiCancellationToken *cancel;
} Fake;
static UmiStatus Exchange(void *context, const char *url, const char *body, const char *request_key,
                          const char *key, const UmiCancellationToken *cancel, char *reply, size_t capacity,
                          size_t *length, unsigned *http)
{
    Fake *f = context;
    ++f->calls;
    (void)cancel;
    CHECK(strncmp(url, "https://api.heygen.com/", 22U) == 0);
    CHECK(strcmp(key, "fixture-key") == 0);
    if (body != NULL)
        CHECK(strcmp(request_key, "fixture-request") == 0);
    *http = f->http;
    *length = f->length ? f->length : strlen(f->body);
    if (*length < capacity)
        memcpy(reply, f->body, *length);
    if (f->cancel_reply)
        umi_cancellation_token_request(f->cancel);
    return f->status;
}
static void Video(UmiHeyGenDraft *d)
{
    memset(d, 0, sizeof(*d));
    d->operation = UMI_HEYGEN_CREATE_VIDEO;
    strcpy(d->resource_id, "look-1");
    strcpy(d->voice_id, "voice-1");
    strcpy(d->name, "Lesson");
    strcpy(d->script, "Hello \"team\"\nCaf\xc3\xa9");
    strcpy(d->request_key, "fixture-request");
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *test = argv[1];
    UmiHeyGenDraft *d = calloc(1U, sizeof(*d));
    UmiHeyGenResult *r = calloc(1U, sizeof(*r));
    CHECK(d && r);
    UmiHeyGenPlan *plan = NULL;
    Video(d);
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    Fake f = {"{\"data\":{\"video_id\":\"video-1\",\"status\":\"waiting\"}}",
              UMI_STATUS_OK,
              200U,
              0U,
              0U,
              false,
              cancel};
    bool encode_only = false, approved = true;
    const char *key = "fixture-key";
    UmiStatus prepare = UMI_STATUS_OK, expected = UMI_STATUS_OK;
    if (strcmp(test, "video") == 0)
    {
        d->subtitles = true;
        d->portrait = true;
    }
    else if (strcmp(test, "avatar") == 0)
    {
        memset(d, 0, sizeof(*d));
        d->operation = UMI_HEYGEN_CREATE_AVATAR;
        strcpy(d->name, "Guide");
        strcpy(d->script, "A friendly synthetic presenter");
        strcpy(d->request_key, "fixture-request");
        f.body = "{\"data\":{\"avatar_item\":{\"id\":\"look-1\",\"status\":\"pending\"}}}";
    }
    else if (strcmp(test, "deny") == 0)
    {
        approved = false;
        expected = UMI_STATUS_PERMISSION_DENIED;
    }
    else if (strcmp(test, "header-injection") == 0)
    {
        key = "fixture\r\nx: injected";
        expected = UMI_STATUS_PERMISSION_DENIED;
    }
    else if (strcmp(test, "cancel-before") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
    }
    else if (strcmp(test, "cancel-confirmed") == 0)
        f.cancel_reply = true;
    else if (strcmp(test, "timeout") == 0)
    {
        f.status = UMI_STATUS_TIMEOUT;
        expected = UMI_STATUS_TIMEOUT;
    }
    else if (strcmp(test, "redirect") == 0)
    {
        f.http = 302U;
        expected = UMI_STATUS_IO_ERROR;
    }
    else if (strcmp(test, "unauthorized") == 0)
    {
        f.http = 401U;
        expected = UMI_STATUS_PERMISSION_DENIED;
    }
    else if (strcmp(test, "rate-limit") == 0)
    {
        f.http = 429U;
        expected = UMI_STATUS_BUSY;
    }
    else if (strcmp(test, "in-progress") == 0)
    {
        f.http = 409U;
        expected = UMI_STATUS_BUSY;
    }
    else if (strcmp(test, "overflow") == 0)
    {
        f.length = UMI_HEYGEN_BODY_CAPACITY;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (strcmp(test, "duplicate-data") == 0)
    {
        f.body = "{\"data\":{},\"d\\u0061ta\":{\"video_id\":\"video-1\",\"status\":\"waiting\"}}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(test, "duplicate-id") == 0)
    {
        f.body = "{\"data\":{\"video_id\":\"video-1\",\"video_id\":\"video-2\",\"status\":\"waiting\"}}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(test, "wrong-field-type") == 0)
    {
        f.body = "{\"data\":{\"video_id\":123,\"status\":\"waiting\"}}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(test, "escaped-nul") == 0)
    {
        f.body = "{\"data\":{\"video_id\":\"video-1\\u0000-hidden\",\"status\":\"waiting\"}}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(test, "trailing") == 0)
    {
        f.body = "{\"data\":{}} false";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(test, "embedded-nul") == 0)
    {
        f.body = "{}\0x";
        f.length = 4U;
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(test, "provider-error") == 0)
    {
        f.body = "{\"error\":{\"message\":\"private failure text\"},\"data\":{}}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(test, "path-injection") == 0)
    {
        strcpy(d->resource_id, "../../keys");
        prepare = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "missing-request-key") == 0)
    {
        d->request_key[0] = '\0';
        prepare = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "unterminated") == 0)
    {
        memset(d->script, 'x', sizeof(d->script));
        prepare = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "invalid-utf8") == 0)
    {
        d->script[0] = (char)0xc0;
        d->script[1] = (char)0xaf;
        d->script[2] = '\0';
        prepare = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(test, "empty-script") == 0)
    {
        strcpy(d->script, " \t\n");
        prepare = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "immutable") == 0)
        encode_only = true;
    else if (strcmp(test, "prompt-limit") == 0)
    {
        memset(d, 0, sizeof(*d));
        d->operation = UMI_HEYGEN_CREATE_AVATAR;
        strcpy(d->name, "Guide");
        memset(d->script, 'a', 1001U);
        strcpy(d->request_key, "fixture-request");
        prepare = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (strncmp(test, "catalogue", 9U) == 0 || strcmp(test, "cursor") == 0 ||
             strcmp(test, "unexpected-field") == 0)
    {
        memset(d, 0, sizeof(*d));
        d->operation = UMI_HEYGEN_LIST_AVATARS;
        d->private_catalogue = true;
        f.body = "{\"data\":[{\"id\":\"look-1\",\"name\":\"Guide\",\"status\":\"completed\"}],\"has_more\":"
                 "true,\"next_token\":\"a+/=&b\"}";
        if (strcmp(test, "catalogue-voices") == 0)
        {
            d->operation = UMI_HEYGEN_LIST_VOICES;
            f.body = "{\"data\":[{\"voice_id\":\"voice-1\",\"name\":\"Caf\\u00e9\"}],\"has_more\":false,"
                     "\"next_token\":null}";
        }
        else if (strcmp(test, "catalogue-duplicate") == 0)
        {
            f.body = "{\"data\":[{\"id\":\"same\",\"name\":\"a\",\"status\":\"completed\"},{\"id\":\"same\","
                     "\"name\":\"b\",\"status\":\"completed\"}],\"has_more\":false}";
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else if (strcmp(test, "catalogue-missing-cursor") == 0)
        {
            f.body = "{\"data\":[{\"id\":\"a\",\"name\":\"a\",\"status\":\"completed\"}],\"has_more\":true}";
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else if (strcmp(test, "catalogue-empty") == 0)
            f.body = "{\"data\":[],\"has_more\":false,\"next_token\":null}";
        else if (strcmp(test, "catalogue-cancel") == 0)
        {
            f.cancel_reply = true;
            expected = UMI_STATUS_CANCELLED;
        }
        else if (strcmp(test, "cursor") == 0)
        {
            strcpy(d->cursor, "a+/=&b");
            encode_only = true;
        }
        else if (strcmp(test, "unexpected-field") == 0)
        {
            strcpy(d->script, "private draft");
            prepare = UMI_STATUS_INVALID_ARGUMENT;
        }
        else
            CHECK(strcmp(test, "catalogue") == 0);
    }
    else if (strncmp(test, "status", 6U) == 0)
    {
        memset(d, 0, sizeof(*d));
        d->operation = UMI_HEYGEN_GET_VIDEO;
        strcpy(d->resource_id, "video-1");
        f.body = "{\"data\":{\"id\":\"video-1\",\"status\":\"completed\",\"video_url\":\"https://"
                 "files.heygen.ai/render.mp4?token=private\"}}";
        if (strcmp(test, "status-mismatch") == 0)
        {
            strcpy(d->resource_id, "another-video");
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else if (strcmp(test, "status-url") == 0)
        {
            f.body = "{\"data\":{\"id\":\"video-1\",\"status\":\"completed\",\"video_url\":\"file:///"
                     "private.mp4\"}}";
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else if (strcmp(test, "status-subtitles") == 0)
            f.body = "{\"data\":{\"id\":\"video-1\",\"status\":\"completed\",\"video_url\":\"https://"
                     "files.heygen.ai/render.mp4\",\"subtitle_url\":\"https://files.heygen.ai/"
                     "captions.srt?token=private\"}}";
        else if (strcmp(test, "status-subtitles-null") == 0)
            f.body = "{\"data\":{\"id\":\"video-1\",\"status\":\"completed\",\"video_url\":\"https://"
                     "files.heygen.ai/render.mp4\",\"subtitle_url\":null}}";
        else if (strcmp(test, "status-subtitles-url") == 0)
        {
            f.body = "{\"data\":{\"id\":\"video-1\",\"status\":\"completed\",\"video_url\":\"https://"
                     "files.heygen.ai/render.mp4\",\"subtitle_url\":\"https://user@private.example/"
                     "captions.srt\"}}";
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else if (strcmp(test, "status-subtitles-duplicate") == 0)
        {
            f.body = "{\"data\":{\"id\":\"video-1\",\"status\":\"completed\",\"video_url\":\"https://"
                     "files.heygen.ai/render.mp4\",\"subtitle_url\":null,\"subtitle_url\":null}}";
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else if (strcmp(test, "status-pending") == 0)
            f.body = "{\"data\":{\"id\":\"video-1\",\"status\":\"processing\",\"video_url\":null}}";
        else if (strcmp(test, "status-avatar") == 0)
        {
            d->operation = UMI_HEYGEN_GET_AVATAR;
            f.body = "{\"data\":{\"id\":\"video-1\",\"status\":\"completed\"}}";
        }
        else
            CHECK(strcmp(test, "status") == 0);
    }
    else
        CHECK(false);
    CHECK(UmiHeyGenPrepare(d, &plan) == prepare);
    if (prepare != UMI_STATUS_OK)
        CHECK(plan == NULL && f.calls == 0U);
    else
    {
        const char *url, *body;
        bool creates = false;
        CHECK(UmiHeyGenReview(plan, &url, &body, &creates) == UMI_STATUS_OK);
        if (strcmp(test, "immutable") == 0)
        {
            memset(d, 0, sizeof(*d));
            CHECK(strstr(body, "look-1") && strstr(body, "Caf\xc3\xa9"));
            CHECK(strstr(body, "\\\"team\\\"") && strstr(body, "\\n"));
        }
        if (strcmp(test, "cursor") == 0)
            CHECK(strstr(url, "token=%61%2B%2F%3D%26%62") != NULL);
        if (strcmp(test, "video") == 0)
            CHECK(strstr(body, "\"caption\":{\"file_format\":\"srt\"}") && strstr(body, "9:16"));
        if (!encode_only)
        {
            CHECK(UmiHeyGenExecuteWithTransport(plan, approved, key, cancel, Exchange, &f, r) == expected);
            if (f.calls == 0U)
                CHECK(!r->creation_may_exist);
            else if (creates)
            {
                CHECK(r->creation_may_exist);
                CHECK(r->creation_confirmed == (expected == UMI_STATUS_OK));
                CHECK(UmiHeyGenExecuteWithTransport(plan, true, "fixture-key", NULL, Exchange, &f, r) ==
                      UMI_STATUS_INVALID_STATE);
                CHECK(f.calls == 1U);
            }
            else if (expected == UMI_STATUS_OK)
            {
                CHECK(!r->creation_may_exist);
                if (strcmp(test, "status") == 0)
                    CHECK(strstr(r->video_url, "https://files.heygen.ai/") != NULL);
                if (strcmp(test, "status-subtitles") == 0)
                    CHECK(strstr(r->subtitle_url, "/captions.srt?token=private") != NULL);
                if (strcmp(test, "status-subtitles-null") == 0)
                    CHECK(r->subtitle_url[0] == '\0');
                CHECK(UmiHeyGenExecuteWithTransport(plan, true, "fixture-key", NULL, Exchange, &f, r) ==
                      UMI_STATUS_OK);
                CHECK(f.calls == 2U);
            }
            if (expected != UMI_STATUS_OK)
                CHECK(r->resource_id[0] == '\0' && r->video_url[0] == '\0' && r->count == 0U);
        }
    }
    UmiHeyGenDestroy(plan);
    umi_cancellation_token_destroy(cancel);
    free(d);
    free(r);
    return 0;
}
