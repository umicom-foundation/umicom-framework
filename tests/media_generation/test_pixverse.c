/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_pixverse.c
 * PURPOSE: Exercise video receipts, hostile responses and one-attempt submission without network access.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/media_generation/pixverse.h"
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

typedef struct Fixture
{
    const char *reply;
    size_t length;
    unsigned calls, http;
    UmiStatus status;
    bool creating, race;
    UmiCancellationToken *cancel;
} Fixture;
static UmiStatus Exchange(void *context, const char *url, const char *body, const char *trace,
                          const char *key, const UmiCancellationToken *cancel, char *reply, size_t capacity,
                          size_t *length, unsigned *http)
{
    Fixture *f = context;
    ++f->calls;
    (void)cancel;
    CHECK(!strcmp(key, "fixture-key") && !strcmp(trace, "93cb2fa6-e5da-4c69-9588-94ca9839f238"));
    CHECK(!strcmp(url, f->creating ? "https://app-api.pixverse.ai/openapi/v2/video/text/generate"
                                   : "https://app-api.pixverse.ai/openapi/v2/video/result/42"));
    CHECK(f->creating ? body && strstr(body, "A quiet sea") : body == NULL);
    *http = f->http;
    if (f->length >= capacity)
    {
        *length = capacity;
        return UMI_STATUS_OK;
    }
    memcpy(reply, f->reply, f->length);
    *length = f->length;
    if (f->race)
        umi_cancellation_token_request(f->cancel);
    return f->status;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *test = argv[1];
    UmiPixVerseDraft *draft = calloc(1, sizeof(*draft));
    UmiPixVerseResult *result = calloc(1, sizeof(*result));
    CHECK(draft && result);
    draft->operation = UMI_PIXVERSE_CREATE_VIDEO;
    strcpy(draft->trace_id, "93cb2fa6-e5da-4c69-9588-94ca9839f238");
    strcpy(draft->prompt, "A quiet sea");
    strcpy(draft->aspect_ratio, "16:9");
    draft->seconds = 5;
    draft->quality = 720;
    if (!strcmp(test, "settings"))
    {
        draft->seconds = 15;
        draft->quality = 1080;
        strcpy(draft->aspect_ratio, "9:16");
        draft->audio = true;
        draft->multiple_shots = true;
    }
    bool reading = !strncmp(test, "status", 6);
    if (reading)
    {
        draft->operation = UMI_PIXVERSE_GET_VIDEO;
        draft->video_id = 42;
        draft->prompt[0] = 0;
        draft->aspect_ratio[0] = 0;
        draft->seconds = 0;
        draft->quality = 0;
    }
    if (!strcmp(test, "bad-trace"))
        draft->trace_id[5] = '\n';
    if (!strcmp(test, "bad-ratio"))
        strcpy(draft->aspect_ratio, "2:9");
    if (!strcmp(test, "bad-quality"))
        draft->quality = 800;
    if (!strcmp(test, "bad-duration"))
        draft->seconds = 16;
    if (!strcmp(test, "empty"))
        draft->prompt[0] = 0;
    if (!strcmp(test, "utf8"))
        draft->prompt[0] = (char)0xff;
    if (!strcmp(test, "unterminated"))
        memset(draft->prompt, 'x', sizeof(draft->prompt));
    if (!strcmp(test, "status-fields"))
        strcpy(draft->prompt, "must not leave the machine");
    if (!strcmp(test, "status-id"))
        draft->video_id = 0;
    UmiPixVersePlan *plan = NULL;
    UmiStatus status = UmiPixVersePrepare(draft, &plan);
    bool invalid = !strncmp(test, "bad-", 4) || !strcmp(test, "empty") || !strcmp(test, "utf8") ||
                   !strcmp(test, "unterminated") || !strcmp(test, "status-fields") ||
                   !strcmp(test, "status-id");
    if (invalid)
    {
        CHECK(status != UMI_STATUS_OK && !plan);
        free(draft);
        free(result);
        return 0;
    }
    CHECK(status == UMI_STATUS_OK && plan);
    const char *url = NULL, *body = NULL;
    bool creates = false;
    CHECK(UmiPixVerseReview(plan, &url, &body, &creates) == UMI_STATUS_OK && creates == !reading);
    if (!strcmp(test, "settings"))
    {
        CHECK(strstr(body, "\"duration\":15") && strstr(body, "\"quality\":\"1080p\""));
        CHECK(strstr(body, "\"aspect_ratio\":\"9:16\"") && strstr(body, "\"generate_audio_switch\":true"));
        CHECK(strstr(body, "\"generate_multi_clip_switch\":true"));
    }
    strcpy(draft->prompt, "edited after review");
    if (!reading)
        CHECK(strstr(body, "A quiet sea") && !strstr(body, "edited"));
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    Fixture f = {
        "{\"ErrCode\":0,\"Resp\":{\"video_id\":42}}", 0, 0, 200, UMI_STATUS_OK, !reading, false, cancel};
    if (reading)
        f.reply = "{\"ErrCode\":0,\"Resp\":{\"id\":42,\"status\":1,\"url\":\"https://video.example/"
                  "clip.mp4?token=private\"}}";
    if (!strcmp(test, "status-pending"))
        f.reply = "{\"ErrCode\":0,\"Resp\":{\"id\":42,\"status\":5,\"url\":null}}";
    if (!strcmp(test, "status-mismatch"))
        f.reply = "{\"ErrCode\":0,\"Resp\":{\"id\":43,\"status\":5}}";
    if (!strcmp(test, "status-unknown"))
        f.reply = "{\"ErrCode\":0,\"Resp\":{\"id\":42,\"status\":9}}";
    if (!strcmp(test, "status-url"))
        f.reply = "{\"ErrCode\":0,\"Resp\":{\"id\":42,\"status\":1,\"url\":\"http://unsafe.example/a\"}}";
    if (!strcmp(test, "duplicate"))
        f.reply = "{\"ErrCode\":0,\"Resp\":{\"video_id\":42,\"video_\\u0069d\":43}}";
    if (!strcmp(test, "provider-error"))
        f.reply = "{\"ErrCode\":500044,\"Resp\":{\"video_id\":42}}";
    if (!strcmp(test, "wrong-type"))
        f.reply = "{\"ErrCode\":0,\"Resp\":{\"video_id\":\"42\"}}";
    if (!strcmp(test, "trailing"))
        f.reply = "{\"ErrCode\":0,\"Resp\":{\"video_id\":42}}x";
    if (!strcmp(test, "status-deleted"))
        f.reply = "{\"ErrCode\":0,\"Resp\":{\"id\":42,\"status\":6}}";
    if (!strcmp(test, "status-moderated"))
        f.reply = "{\"ErrCode\":0,\"Resp\":{\"id\":42,\"status\":7}}";
    if (!strcmp(test, "status-failed"))
        f.reply = "{\"ErrCode\":0,\"Resp\":{\"id\":42,\"status\":8}}";
    f.length = strlen(f.reply);
    if (!strcmp(test, "overflow"))
        f.length = UMI_PIXVERSE_BODY_CAPACITY;
    if (!strcmp(test, "timeout"))
        f.status = UMI_STATUS_TIMEOUT;
    if (!strcmp(test, "redirect"))
        f.http = 302;
    if (!strcmp(test, "rate-limit"))
        f.http = 429;
    if (!strcmp(test, "cancel-before"))
        umi_cancellation_token_request(cancel);
    if (!strcmp(test, "cancel-receipt"))
        f.race = true;
    const char *key = !strcmp(test, "key-injection") ? "key\r\nX: injected" : "fixture-key";
    status =
        UmiPixVerseExecuteWithTransport(plan, strcmp(test, "denied") != 0, key, cancel, Exchange, &f, result);
    bool success = !strcmp(test, "create") || !strcmp(test, "immutable") || !strcmp(test, "cancel-receipt") ||
                   !strcmp(test, "status") || !strcmp(test, "status-pending") || !strcmp(test, "status-once");
    success = success || !strcmp(test, "settings") || !strcmp(test, "status-deleted") ||
              !strcmp(test, "status-moderated") || !strcmp(test, "status-failed");
    CHECK(success ? status == UMI_STATUS_OK : status != UMI_STATUS_OK);
    bool before = !strcmp(test, "denied") || !strcmp(test, "key-injection") || !strcmp(test, "cancel-before");
    CHECK(f.calls == (before ? 0U : 1U));
    if (before)
        CHECK(!result->creation_may_exist);
    else
    {
        CHECK(result->creation_may_exist == !reading);
        CHECK(success ? result->video_id == 42 : result->video_id == 0);
        CHECK(UmiPixVerseExecuteWithTransport(plan, true, "fixture-key", NULL, Exchange, &f, result) ==
              UMI_STATUS_INVALID_STATE);
        CHECK(f.calls == 1U && result->creation_may_exist == !reading);
    }
    umi_cancellation_token_destroy(cancel);
    UmiPixVerseDestroy(plan);
    free(draft);
    free(result);
    return 0;
}
