/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/provider_text/test_provider_text.c
 *
 * PURPOSE:
 *   Verify malformed UTF-8 status compatibility and output preservation at provider codec boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../../src/provider_chat/internal.h"
#include "../../src/media_generation/heygen_internal.h"
#include "umicom/ai/mcp/json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); return 1; \
} } while (0)

/* Each sequence names a different UTF-8 rejection boundary. A terminating
 * zero is storage metadata, not part of the malformed sequence. */
static const char *const Invalid[] = {
    "\xc0\xaf", "\xe0\x80\xaf", "\xed\xa0\x80", "\xf4\x90\x80\x80",
    "\x80", "\xe2\x82", "\xf0\x9f\x92", "\xf5\x80\x80\x80",
    "\xc2" "A"
};
static const char Valid[] = "Caf\xc3\xa9 \xe6\xb0\xb4 \xf0\x9f\x8c\x8d \"quoted\"\nline";

static void Chat(UmiProviderChatPlan *plan)
{
    memset(plan, 0, sizeof(*plan));
    strcpy(plan->connection.connection.model, "fixture-model");
    plan->output_limit = 128U;
}
static void Video(UmiHeyGenDraft *draft)
{
    memset(draft, 0, sizeof(*draft));
    draft->operation = UMI_HEYGEN_CREATE_VIDEO;
    strcpy(draft->resource_id, "look-1");
    strcpy(draft->voice_id, "voice-1");
    strcpy(draft->request_key, "fixture-request");
    strcpy(draft->script, "Hello");
    strcpy(draft->name, "Lesson");
}
/* Compare copied object representations of the same initialized object, not
 * independently initialized structures with possibly different padding. */
static int RejectChat(bool context)
{
    UmiProviderChatPlan *plan = malloc(sizeof(*plan));
    unsigned char *before = malloc(sizeof(*plan));
    CHECK(plan != NULL && before != NULL);
    for (size_t i = 0U; i < sizeof(Invalid) / sizeof(Invalid[0]); ++i) {
        Chat(plan);
        strcpy(plan->input, "Preserve previous input");
        strcpy(plan->body, "Preserve previous body");
        memcpy(before, plan, sizeof(*plan));
        CHECK(UmiProviderChatEncode(plan, context ? "Question" : Invalid[i],
                                   context ? Invalid[i] : "") == UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(before, plan, sizeof(*plan)) == 0);
        char escaped[128];
        CHECK(umi_ai_mcp_json_escape_string(Invalid[i], escaped, sizeof(escaped)) ==
              UMI_STATUS_INVALID_ARGUMENT);
    }
    free(before);
    free(plan);
    return 0;
}
static int RejectVideo(const char *field)
{
    UmiHeyGenDraft *draft = malloc(sizeof(*draft));
    UmiHeyGenPlan *plan = malloc(sizeof(*plan));
    unsigned char *before = malloc(sizeof(*plan));
    CHECK(draft != NULL && plan != NULL && before != NULL);
    for (size_t i = 0U; i < sizeof(Invalid) / sizeof(Invalid[0]); ++i) {
        Video(draft);
        char *text = strcmp(field, "script") == 0 ? draft->script :
                     strcmp(field, "name") == 0 ? draft->name : draft->motion;
        strcpy(text, Invalid[i]);
        memset(plan, 0, sizeof(*plan));
        strcpy(plan->url, "Previous URL");
        strcpy(plan->body, "Previous body");
        memcpy(before, plan, sizeof(*plan));
        CHECK(UmiHeyGenEncode(draft, plan) == UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(before, plan, sizeof(*plan)) == 0);
    }
    free(before);
    free(plan);
    free(draft);
    return 0;
}
static int ValidText(void)
{
    UmiProviderChatPlan *chat = malloc(sizeof(*chat));
    UmiHeyGenDraft *draft = malloc(sizeof(*draft));
    UmiHeyGenPlan *video = calloc(1U, sizeof(*video));
    CHECK(chat != NULL && draft != NULL && video != NULL);
    for (unsigned responses = 0U; responses < 2U; ++responses) {
        Chat(chat);
        chat->responses = responses != 0U;
        CHECK(UmiProviderChatEncode(chat, Valid, Valid) == UMI_STATUS_OK);
        CHECK(strstr(chat->body, "Caf\xc3\xa9") != NULL);
        CHECK(strstr(chat->body, "\xe6\xb0\xb4") != NULL);
        CHECK(strstr(chat->body, "\xf0\x9f\x8c\x8d") != NULL);
        CHECK(strstr(chat->body, "\\\"quoted\\\"\\nline") != NULL);
    }
    Video(draft);
    strcpy(draft->script, Valid);
    strcpy(draft->name, Valid);
    strcpy(draft->motion, Valid);
    CHECK(UmiHeyGenEncode(draft, video) == UMI_STATUS_OK);
    CHECK(strstr(video->body, "Caf\xc3\xa9") != NULL);
    CHECK(strstr(video->body, "\\\"quoted\\\"\\nline") != NULL);
    free(video);
    free(draft);
    free(chat);
    return 0;
}
static int InvalidFields(void)
{
    UmiProviderChatPlan *chat = calloc(1U, sizeof(*chat));
    UmiHeyGenDraft *draft = malloc(sizeof(*draft));
    UmiHeyGenPlan *video = calloc(1U, sizeof(*video));
    char *unterminated = malloc(UMI_PROVIDER_CHAT_PROMPT_CAPACITY);
    CHECK(chat != NULL && draft != NULL && video != NULL && unterminated != NULL);
    memset(unterminated, 'x', UMI_PROVIDER_CHAT_PROMPT_CAPACITY);
    CHECK(UmiProviderChatEncode(chat, unterminated, "") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiProviderChatEncode(chat, "", "") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiProviderChatEncode(chat, "ok", "\x01") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiProviderChatEncode(NULL, "ok", "") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiProviderChatEncode(chat, NULL, "") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiProviderChatEncode(chat, "ok", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    Video(draft);
    memset(draft->script, 'x', sizeof(draft->script));
    CHECK(UmiHeyGenEncode(draft, video) == UMI_STATUS_INVALID_ARGUMENT);
    Video(draft);
    draft->script[0] = '\0';
    CHECK(UmiHeyGenEncode(draft, video) == UMI_STATUS_INVALID_ARGUMENT);
    Video(draft);
    strcpy(draft->motion, "\x01");
    CHECK(UmiHeyGenEncode(draft, video) == UMI_STATUS_INVALID_ARGUMENT);
    free(unterminated);
    free(video);
    free(draft);
    free(chat);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (strcmp(argv[1], "chat-prompt") == 0) return RejectChat(false);
    if (strcmp(argv[1], "chat-context") == 0) return RejectChat(true);
    if (strcmp(argv[1], "script") == 0 || strcmp(argv[1], "name") == 0 ||
        strcmp(argv[1], "motion") == 0) return RejectVideo(argv[1]);
    if (strcmp(argv[1], "valid") == 0) return ValidText();
    if (strcmp(argv[1], "fields") == 0) return InvalidFields();
    return 2;
}
