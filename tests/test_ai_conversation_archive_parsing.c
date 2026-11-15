/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_conversation_archive_parsing.c
 *
 * PURPOSE:
 *   Verify complete conversation parsing, ambiguous-input refusal and transactional output including aliased input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "ai/conversation_archive_fixture.h"
#include <stdio.h>

#define FIXTURE_CAPACITY 65536U
static void append_text(char *text, const char *suffix)
{
    size_t used = strlen(text), extra = strlen(suffix);
    assert(used + extra < FIXTURE_CAPACITY);
    memcpy(text + used, suffix, extra + 1U);
}

static void make_text(char *text, size_t count)
{
    char row[256];
    strcpy(text, "format=umicom-ai-session-v1\nsession.id=session.fixture\n"
        "session.provider=provider.local\nsession.model=model.chat\nsession.created=1\n"
        "record.workspace=workspace.fixture\nrecord.title=Archive fixture\n"
        "record.classification=2\nrecord.remote-approved=0\nrecord.persistence-approved=0\n"
        "record.updated=2\nrecord.revision=3\n");
    (void)snprintf(row, sizeof row, "message.count=%zu\n", count);
    append_text(text, row);
    for (size_t index = 0U; index < count; ++index) {
        (void)snprintf(row, sizeof row, "message.%zu.role=2\nmessage.%zu.name=user\n"
            "message.%zu.text=Message %zu\n", index, index, index, index);
        append_text(text, row);
    }
}

/* Fixture editing replaces complete lines, not production parser operations. */
static void change_field(char *text, const char *key, const char *value)
{
    char prefix[128];
    char *line, *end;
    size_t head, tail, replacement;
    (void)snprintf(prefix, sizeof prefix, "%s=", key);
    line = strstr(text, prefix);
    assert(line != NULL && (line == text || line[-1] == '\n'));
    end = strchr(line, '\n');
    assert(end != NULL);
    ++end;
    head = (size_t)(line - text); tail = strlen(end);
    replacement = value != NULL ? strlen(prefix) + strlen(value) + 1U : 0U;
    assert(head + replacement + tail < FIXTURE_CAPACITY);
    memmove(text + head + replacement, end, tail + 1U);
    if (value != NULL) {
        memcpy(line, prefix, strlen(prefix));
        memcpy(line + strlen(prefix), value, strlen(value));
        line[replacement - 1U] = '\n';
    }
}

static void expect_refusal(const void *text, size_t size, UmiStatus expected)
{
    UmiAiConversationRecord *out = archive_record_fixture("session.keep");
    UmiAiConversationRecord *before = malloc(sizeof *before);
    assert(before != NULL);
    memcpy(before, out, sizeof *before);
    assert(UmiAiConversationRecordParse(text, size, out) == expected);
    assert(memcmp(before, out, sizeof *out) == 0);
    free(out); free(before);
}

int main(int argc, char **argv)
{
    static const char *const modes[] = {
        "valid", "crlf", "reordered", "no-final-newline", "escaped", "empty-messages",
        "maximum-messages", "optional-empty", "max-u64", "zero-revision", "alias",
        "null-input", "null-output", "empty", "size-limit", "embedded-nul", "escaped-nul",
        "invalid-escape", "short-escape", "duplicate-metadata", "duplicate-message",
        "index-alias", "missing-metadata", "missing-message", "orphan-message",
        "missing-separator", "unknown-field", "bad-format", "numeric-sign",
        "numeric-space", "numeric-overflow", "numeric-trailing", "index-sign",
        "index-space", "index-range", "classification-range", "classification-alias",
        "message-count-range", "role-range", "field-overflow"
    };
    char *text = calloc(FIXTURE_CAPACITY, 1U), *scratch = calloc(FIXTURE_CAPACITY, 1U);
    UmiAiConversationRecord *out = archive_record_fixture("session.old");
    UmiStatus expected = UMI_STATUS_PARSE_ERROR;
    size_t size, expected_messages = 1U;
    int known = 0, success = 0;
    assert(argc == 2 && text != NULL && scratch != NULL);
    for (size_t i = 0U; i < sizeof modes / sizeof modes[0]; ++i)
        if (strcmp(argv[1], modes[i]) == 0) known = 1;
    assert(known);
    make_text(text, 1U);
#define CASE(name) (strcmp(argv[1], name) == 0)
    if (CASE("valid")) success = 1;
    else if (CASE("crlf")) {
        size_t used = 0U;
        for (size_t i = 0U; text[i] != '\0'; ++i) {
            if (text[i] == '\n') scratch[used++] = '\r';
            scratch[used++] = text[i];
        }
        scratch[used] = '\0'; strcpy(text, scratch); success = 1;
    } else if (CASE("reordered")) {
        char *lines[256], *cursor = text;
        size_t count = 0U;
        while (*cursor != '\0') {
            char *next = strchr(cursor, '\n');
            assert(next != NULL && count < sizeof lines / sizeof lines[0]);
            *next = '\0'; lines[count++] = cursor; cursor = next + 1U;
        }
        while (count != 0U) { append_text(scratch, lines[--count]); append_text(scratch, "\n"); }
        strcpy(text, scratch); success = 1;
    } else if (CASE("no-final-newline")) {
        text[strlen(text) - 1U] = '\0'; success = 1;
    } else if (CASE("escaped")) {
        change_field(text, "message.0.text", "first%0Asecond%25%3D%0D");
        success = 1;
    } else if (CASE("empty-messages")) {
        make_text(text, 0U); expected_messages = 0U; success = 1;
    } else if (CASE("maximum-messages")) {
        make_text(text, UMI_AI_MAX_MESSAGES); expected_messages = UMI_AI_MAX_MESSAGES; success = 1;
    } else if (CASE("optional-empty")) {
        change_field(text, "message.0.name", ""); change_field(text, "message.0.text", ""); success = 1;
    } else if (CASE("max-u64")) {
        change_field(text, "session.created", "18446744073709551615");
        change_field(text, "record.updated", "18446744073709551615");
        change_field(text, "record.revision", "18446744073709551615"); success = 1;
    } else if (CASE("zero-revision")) {
        change_field(text, "record.revision", "0"); success = 1;
    } else if (CASE("alias")) {
        size = strlen(text);
        assert(size < sizeof out->conversation.messages[0].text);
        memcpy(out->conversation.messages[0].text, text, size);
        assert(UmiAiConversationRecordParse(out->conversation.messages[0].text, size, out) == UMI_STATUS_OK);
        assert(strcmp(out->session.session_id, "session.fixture") == 0 &&
            strcmp(out->conversation.messages[0].text, "Message 0") == 0);
        goto cleanup;
    } else if (CASE("null-input")) {
        expect_refusal(NULL, 1U, UMI_STATUS_INVALID_ARGUMENT); goto cleanup;
    } else if (CASE("null-output")) {
        assert(UmiAiConversationRecordParse(text, strlen(text), NULL) == UMI_STATUS_INVALID_ARGUMENT);
        goto cleanup;
    } else if (CASE("empty")) {
        expect_refusal(text, 0U, UMI_STATUS_PARSE_ERROR); goto cleanup;
    } else if (CASE("size-limit")) {
        const char byte = 'x';
        expect_refusal(&byte, (size_t)UMI_AI_CONVERSATION_FILE_MAX_BYTES + 1U, UMI_STATUS_CAPACITY_EXCEEDED);
        goto cleanup;
    } else if (CASE("embedded-nul")) {
        size = strlen(text); text[size / 2U] = '\0';
        expect_refusal(text, size, UMI_STATUS_PARSE_ERROR); goto cleanup;
    } else if (CASE("escaped-nul")) change_field(text, "message.0.text", "before%00hidden");
    else if (CASE("invalid-escape")) change_field(text, "message.0.text", "%GG");
    else if (CASE("short-escape")) change_field(text, "message.0.text", "%2");
    else if (CASE("duplicate-metadata")) append_text(text, "record.classification=1\n");
    else if (CASE("duplicate-message")) append_text(text, "message.0.text=Overwrite\n");
    else if (CASE("index-alias")) append_text(text, "message.00.text=Overwrite\n");
    else if (CASE("missing-metadata")) {
        static const char *const keys[] = {"format", "session.id", "session.provider",
            "session.model", "session.created", "record.workspace", "record.title",
            "record.classification", "record.remote-approved", "record.persistence-approved",
            "record.updated", "record.revision", "message.count"};
        for (size_t i = 0U; i < sizeof keys / sizeof keys[0]; ++i) {
            make_text(text, 1U); change_field(text, keys[i], NULL);
            expect_refusal(text, strlen(text), UMI_STATUS_PARSE_ERROR);
        }
        goto cleanup;
    } else if (CASE("missing-message")) {
        static const char *const keys[] = {"message.0.role", "message.0.name", "message.0.text"};
        for (size_t i = 0U; i < sizeof keys / sizeof keys[0]; ++i) {
            make_text(text, 1U); change_field(text, keys[i], NULL);
            expect_refusal(text, strlen(text), UMI_STATUS_PARSE_ERROR);
        }
        goto cleanup;
    } else if (CASE("orphan-message")) change_field(text, "message.count", "0");
    else if (CASE("missing-separator")) append_text(text, "unparseable-line\n");
    else if (CASE("unknown-field")) append_text(text, "record.unknown=x\n");
    else if (CASE("bad-format")) change_field(text, "format", "umicom-ai-session-v99");
    else if (CASE("numeric-sign")) {
        const char *values[] = {"-1", "+1"};
        for (size_t i = 0U; i < 2U; ++i) {
            change_field(text, "session.created", values[i]);
            expect_refusal(text, strlen(text), UMI_STATUS_PARSE_ERROR);
        }
        goto cleanup;
    } else if (CASE("numeric-space")) change_field(text, "record.updated", " 2");
    else if (CASE("numeric-overflow")) change_field(text, "record.revision", "18446744073709551616");
    else if (CASE("numeric-trailing")) change_field(text, "record.updated", "2x");
    else if (CASE("index-sign")) {
        change_field(text, "message.0.text", NULL); append_text(text, "message.+0.text=Message 0\n");
    } else if (CASE("index-space")) {
        change_field(text, "message.0.text", NULL); append_text(text, "message. 0.text=Message 0\n");
    } else if (CASE("index-range")) append_text(text, "message.64.text=Outside\n");
    else if (CASE("classification-range")) change_field(text, "record.classification", "5");
    else if (CASE("classification-alias")) change_field(text, "record.classification", "4294967298");
    else if (CASE("message-count-range")) change_field(text, "message.count", "65");
    else if (CASE("role-range")) change_field(text, "message.0.role", "0");
    else if (CASE("field-overflow")) {
        memset(scratch, 'x', UMI_AI_TEXT_CAPACITY); scratch[UMI_AI_TEXT_CAPACITY] = '\0';
        change_field(text, "message.0.text", scratch); expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (success) {
        assert(UmiAiConversationRecordParse(text, strlen(text), out) == UMI_STATUS_OK);
        assert(UmiAiConversationRecordValidate(out) == UMI_STATUS_OK);
        assert(strcmp(out->session.session_id, "session.fixture") == 0);
        assert(strcmp(out->conversation.conversation_id, out->session.session_id) == 0);
        assert(out->conversation.message_count == expected_messages && out->classification == UMI_AI_DATA_INTERNAL);
        if (CASE("escaped")) assert(strcmp(out->conversation.messages[0].text, "first\nsecond%=\r") == 0);
        if (CASE("optional-empty")) assert(out->conversation.messages[0].name[0] == '\0' &&
            out->conversation.messages[0].text[0] == '\0');
        if (CASE("max-u64")) assert(out->revision == UINT64_MAX &&
            out->session.created_at_ns == UINT64_MAX && out->updated_at_ns == UINT64_MAX);
        if (CASE("zero-revision")) assert(out->revision == 0U);
    } else expect_refusal(text, strlen(text), expected);
cleanup:
    free(out); free(text); free(scratch);
    return 0;
#undef CASE
}
