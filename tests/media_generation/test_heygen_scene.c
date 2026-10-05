/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_heygen_scene.c
 * PURPOSE: Check scene request disclosure and asynchronous receipts without contacting a provider.
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
typedef struct Reply
{
    unsigned http, calls;
    const char *json;
} Reply;
static UmiStatus Exchange(void *context, const char *url, const char *body, const char *request_key,
                          const char *key, const UmiCancellationToken *cancel, char *reply, size_t capacity,
                          size_t *length, unsigned *http)
{
    Reply *r = context;
    ++r->calls;
    (void)cancel;
    CHECK(strcmp(url, "https://api.heygen.com/v3/models/videos") == 0);
    CHECK(body && strstr(body, "\"prompt_enhancement\":\"disabled\""));
    CHECK(strcmp(key, "fixture-key") == 0 && strcmp(request_key, "fixture-scene") == 0);
    *length = strlen(r->json);
    CHECK(*length < capacity);
    memcpy(reply, r->json, *length);
    *http = r->http;
    return UMI_STATUS_OK;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *test = argv[1];
    UmiHeyGenDraft *d = calloc(1U, sizeof(*d));
    UmiHeyGenResult *r = calloc(1U, sizeof(*r));
    CHECK(d && r);
    d->operation = UMI_HEYGEN_CREATE_SCENE;
    d->scene_seconds = 5U;
    strcpy(d->script, "A character waves \"hello\".\nCaf\xc3\xa9");
    strcpy(d->request_key, "fixture-scene");
    UmiStatus prepare = UMI_STATUS_OK, execute = UMI_STATUS_OK;
    Reply f = {202U, 0U, "{\"data\":{\"video_id\":\"scene-1\",\"status\":\"pending\"}}"};
    bool image = false;
    if (strcmp(test, "text") == 0)
    {
    }
    else if (strcmp(test, "portrait") == 0)
        d->portrait = true;
    else if (strcmp(test, "image") == 0)
    {
        strcpy(d->image_asset_id, "asset_123");
        image = true;
    }
    else if (strcmp(test, "maximum") == 0)
        d->scene_seconds = 15U;
    else if (strcmp(test, "short") == 0)
    {
        d->scene_seconds = 4U;
        prepare = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "long") == 0)
    {
        d->scene_seconds = 16U;
        prepare = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "zero") == 0)
    {
        d->scene_seconds = 0U;
        prepare = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "asset-path") == 0)
    {
        strcpy(d->image_asset_id, "C:/photos/person.png");
        prepare = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "image-portrait") == 0)
    {
        strcpy(d->image_asset_id, "asset_123");
        d->portrait = true;
        prepare = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "unused-title") == 0)
    {
        strcpy(d->name, "Do not leak this title");
        prepare = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "unused-voice") == 0)
    {
        strcpy(d->voice_id, "voice-1");
        prepare = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "unused-motion") == 0)
    {
        strcpy(d->motion, "move");
        prepare = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "unused-subtitles") == 0)
    {
        d->subtitles = true;
        prepare = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "read-scene-fields") == 0)
    {
        d->operation = UMI_HEYGEN_LIST_VOICES;
        d->script[0] = 0;
        d->request_key[0] = 0;
        prepare = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "wrong-success-code") == 0)
    {
        f.http = 200U;
        execute = UMI_STATUS_IO_ERROR;
    }
    else if (strcmp(test, "credits") == 0)
    {
        f.http = 402U;
        execute = UMI_STATUS_PERMISSION_DENIED;
    }
    else if (strcmp(test, "missing-id") == 0)
    {
        f.json = "{\"data\":{\"status\":\"pending\"}}";
        execute = UMI_STATUS_PARSE_ERROR;
    }
    else
        CHECK(false);
    UmiHeyGenPlan *plan = NULL;
    CHECK(UmiHeyGenPrepare(d, &plan) == prepare);
    if (prepare != UMI_STATUS_OK)
        CHECK(plan == NULL && f.calls == 0U);
    else
    {
        const char *url, *body;
        bool creates = false;
        CHECK(UmiHeyGenReview(plan, &url, &body, &creates) == UMI_STATUS_OK && creates);
        CHECK(strstr(body, "\"resolution\":\"768p\"") && strstr(body, "Caf\xc3\xa9"));
        CHECK(strstr(body, "\\\"hello\\\"") && strstr(body, "\\n"));
        CHECK(!strstr(body, "\"voice_id\"") && !strstr(body, "\"title\"") && !strstr(body, "\"caption\""));
        if (image)
            CHECK(strstr(body, "\"mode\":\"image_to_video\"") && strstr(body, "\"asset_id\":\"asset_123\"") &&
                  !strstr(body, "aspect_ratio"));
        else
            CHECK(strstr(body, "\"mode\":\"text_to_video\"") && strstr(body, d->portrait ? "9:16" : "16:9"));
        CHECK(strstr(body, d->scene_seconds == 15U ? "\"duration\":15" : "\"duration\":5"));
        /* Mutating the caller's draft cannot change a paid, reviewed request. */
        memset(d, 0, sizeof(*d));
        CHECK(UmiHeyGenExecuteWithTransport(plan, true, "fixture-key", NULL, Exchange, &f, r) == execute);
        CHECK(f.calls == 1U && r->creation_may_exist && r->http_status == f.http);
        CHECK(r->creation_confirmed == (execute == UMI_STATUS_OK));
        if (execute == UMI_STATUS_OK)
            CHECK(strcmp(r->resource_id, "scene-1") == 0 && strcmp(r->state, "pending") == 0);
        else
            CHECK(r->resource_id[0] == 0 && r->video_url[0] == 0);
        CHECK(UmiHeyGenExecuteWithTransport(plan, true, "fixture-key", NULL, Exchange, &f, r) ==
                  UMI_STATUS_INVALID_STATE &&
              f.calls == 1U);
    }
    UmiHeyGenDestroy(plan);
    free(d);
    free(r);
    return 0;
}
