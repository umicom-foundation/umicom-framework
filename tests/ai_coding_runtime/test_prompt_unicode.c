/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_prompt_unicode.c
 *
 * PURPOSE:
 *   Verify complete UTF-8 scalar chunks, byte reassembly and failure-preserving request text admission.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai_coding_runtime/prompt.h"
#include "umicom/base/utf8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); goto cleanup; \
} } while (0)

typedef struct UnicodeFixture {
    UmiAiCodingTaskPlan plan;
    UmiAiCodingMaterializedContext context;
    UmiAiCodingRuntimeConfig config;
    UmiAiRequest output, before;
    char bytes[UMI_AI_CODING_PATCH_CONTENT_CAPACITY];
} UnicodeFixture;

static void seed(UnicodeFixture *f)
{
    memset(f, 0, sizeof *f);
    umi_ai_coding_request_init(&f->plan.request, UMI_AI_CODING_TASK_EXPLAIN);
    strcpy(f->plan.request.request_id, "request.unicode");
    strcpy(f->plan.request.session_id, "session.unicode");
    strcpy(f->plan.request.runtime_id, "runtime.unicode");
    strcpy(f->plan.request.workspace_root, "memory");
    strcpy(f->plan.request.instruction, "Explain this file");
    strcpy(f->plan.request.active_path, "src/main.c");
    f->plan.ready = 1; f->plan.revision = 3U;
    f->plan.repository_context.file_count = 1U;
    UmiAiCodingContextFile *selected = &f->plan.repository_context.files[0];
    strcpy(selected->path, "src/main.c"); strcpy(selected->language_id, "c");
    selected->priority = 1U; selected->estimated_tokens = 4U;
    selected->classification = UMI_AI_DATA_INTERNAL; selected->enabled = 1;
    f->context.revision = 3U; f->context.file_count = 1U; f->context.estimated_tokens = 4U;
    UmiAiCodingMaterializedFile *file = &f->context.files[0];
    strcpy(file->path, "src/main.c"); strcpy(file->language_id, "c");
    strcpy(file->text, "x"); file->length = 1U;
    file->hash = umi_ai_coding_text_hash(file->text, file->length);
    file->estimated_tokens = 4U; file->active = 1;
    umi_ai_coding_runtime_config_init(&f->config);
    strcpy(f->config.provider_id, "local.fixture"); strcpy(f->config.model_id, "model.fixture");
    memset(&f->output, 0x5a, sizeof f->output); f->before = f->output;
}
static UmiStatus construct(UnicodeFixture *f, const char *field, size_t length)
{
    if (strcmp(field, "source") == 0) {
        UmiAiCodingMaterializedFile *file = &f->context.files[0];
        memcpy(file->text, f->bytes, length); file->text[length] = '\0';
        file->length = length; file->hash = umi_ai_coding_text_hash(file->text, length);
        return UmiAiCodingBuildProviderRequestWithFeedback(&f->plan, &f->context, &f->config, NULL, 0U, &f->output);
    }
    return UmiAiCodingBuildProviderRequestWithFeedback(&f->plan, &f->context, &f->config, f->bytes, length, &f->output);
}
static int messages_valid(const UmiAiRequest *request)
{
    if (!UmiUtf8Validate((const unsigned char *)request->request_id, strlen(request->request_id), NULL) ||
        !UmiUtf8Validate((const unsigned char *)request->model_id, strlen(request->model_id), NULL)) return 0;
    for (size_t i = 0U; i < request->message_count; ++i)
        if (!UmiUtf8Validate((const unsigned char *)request->messages[i].text, strlen(request->messages[i].text), NULL) ||
            !UmiUtf8Validate((const unsigned char *)request->messages[i].name, strlen(request->messages[i].name), NULL)) return 0;
    return 1;
}

static int boundary_case(const char *field, size_t width, size_t overlap)
{
    static const unsigned char scalars[3][4] = {{0xc2,0xa2,0,0}, {0xe2,0x82,0xac,0}, {0xf0,0x9f,0x98,0x80}};
    UnicodeFixture *f = calloc(1U, sizeof *f);
    const size_t limit = UMI_AI_TEXT_CAPACITY - 1U;
    size_t start, length, reconstructed = 0U, chunks = 0U;
    int result = 1;
    REQUIRE(f != NULL && width >= 2U && width <= 4U && overlap <= width);
    REQUIRE(strcmp(field, "source") == 0 || strcmp(field, "feedback") == 0);
    seed(f); start = limit - overlap; length = start + width + 5U;
    memset(f->bytes, 'a', length); memcpy(f->bytes + start, scalars[width - 2U], width);
    REQUIRE(construct(f, field, length) == UMI_STATUS_OK);
    REQUIRE(messages_valid(&f->output));
    const char *name = strcmp(field, "source") == 0 ? "src/main.c" : "validation-feedback";
    for (size_t i = 0U; i < f->output.message_count; ++i) {
        const UmiAiMessage *message = &f->output.messages[i];
        if (strcmp(message->name, name) != 0) continue;
        size_t part = strlen(message->text);
        REQUIRE(part <= limit && part <= length - reconstructed);
        REQUIRE(memcmp(message->text, f->bytes + reconstructed, part) == 0);
        reconstructed += part; ++chunks;
    }
    REQUIRE(chunks == 2U && reconstructed == length);
    result = 0;
cleanup:
    free(f); return result;
}

static int invalid_case(const char *field, const char *mode)
{
    UnicodeFixture *f = calloc(1U, sizeof *f);
    size_t length = 1U;
    int result = 1;
    REQUIRE(f != NULL); seed(f);
    REQUIRE(strcmp(field, "source") == 0 || strcmp(field, "feedback") == 0);
    if (strcmp(mode, "continuation") == 0) f->bytes[0] = (char)0x80;
    else if (strcmp(mode, "overlong-two") == 0) { memcpy(f->bytes, "\xc0\x80", 2U); length = 2U; }
    else if (strcmp(mode, "overlong-three") == 0) { memcpy(f->bytes, "\xe0\x9f\xbf", 3U); length = 3U; }
    else if (strcmp(mode, "surrogate") == 0) { memcpy(f->bytes, "\xed\xa0\x80", 3U); length = 3U; }
    else if (strcmp(mode, "above-unicode") == 0) { memcpy(f->bytes, "\xf4\x90\x80\x80", 4U); length = 4U; }
    else if (strcmp(mode, "truncated") == 0) { memcpy(f->bytes, "\xf0\x9f\x98", 3U); length = 3U; }
    else if (strcmp(mode, "bad-continuation") == 0) { memcpy(f->bytes, "\xe2\x41\xac", 3U); length = 3U; }
    else if (strcmp(mode, "late") == 0) { length = UMI_AI_TEXT_CAPACITY; memset(f->bytes, 'x', length); f->bytes[length - 1U] = (char)0xff; }
    else { result = 2; goto cleanup; }
    REQUIRE(construct(f, field, length) == UMI_STATUS_PARSE_ERROR);
    REQUIRE(memcmp(&f->output, &f->before, sizeof f->output) == 0);
    result = 0;
cleanup:
    free(f); return result;
}

static int metadata_case(const char *field)
{
    UnicodeFixture *f = calloc(1U, sizeof *f);
    int result = 1;
    REQUIRE(f != NULL); seed(f);
    if (strcmp(field, "request-id") == 0) strcpy(f->plan.request.request_id, "bad\xff");
    else if (strcmp(field, "model-id") == 0) strcpy(f->config.model_id, "bad\xff");
    else if (strcmp(field, "instruction") == 0) strcpy(f->plan.request.instruction, "bad\xff");
    else if (strcmp(field, "path") == 0) {
        strcpy(f->plan.repository_context.files[0].path, "src/\xff.c");
        strcpy(f->plan.request.active_path, "src/\xff.c");
        strcpy(f->context.files[0].path, "src/\xff.c");
    } else { result = 2; goto cleanup; }
    REQUIRE(umi_ai_coding_build_provider_request(&f->plan, &f->context, &f->config, NULL, &f->output) == UMI_STATUS_PARSE_ERROR);
    REQUIRE(memcmp(&f->output, &f->before, sizeof f->output) == 0);
    result = 0;
cleanup:
    free(f); return result;
}

static int roundtrip_case(void)
{
    UnicodeFixture *f = calloc(1U, sizeof *f);
    int result = 1;
    REQUIRE(f != NULL); seed(f);
    strcpy(f->plan.request.request_id, "request.\xc2\xa2");
    strcpy(f->config.model_id, "model.\xe2\x82\xac");
    strcpy(f->plan.request.instruction, "Explain \xf0\x9f\x98\x80");
    strcpy(f->plan.repository_context.files[0].path, "src/\xe2\x82\xac.c");
    strcpy(f->plan.request.active_path, f->plan.repository_context.files[0].path);
    strcpy(f->context.files[0].path, f->plan.request.active_path);
    REQUIRE(umi_ai_coding_build_provider_request(&f->plan, &f->context, &f->config, NULL, &f->output) == UMI_STATUS_OK);
    REQUIRE(messages_valid(&f->output));
    REQUIRE(strcmp(f->output.request_id, f->plan.request.request_id) == 0);
    REQUIRE(strcmp(f->output.model_id, f->config.model_id) == 0);
    REQUIRE(strcmp(f->output.messages[1].text, f->plan.request.instruction) == 0);
    REQUIRE(strcmp(f->output.messages[2].name, f->context.files[0].path) == 0);
    result = 0;
cleanup:
    free(f); return result;
}

int main(int argc, char **argv)
{
    if (argc == 5 && strcmp(argv[1], "boundary") == 0)
        return boundary_case(argv[2], (size_t)strtoul(argv[3], NULL, 10), (size_t)strtoul(argv[4], NULL, 10));
    if (argc == 4 && strcmp(argv[1], "invalid") == 0) return invalid_case(argv[2], argv[3]);
    if (argc == 3 && strcmp(argv[1], "metadata") == 0) return metadata_case(argv[2]);
    if (argc == 2 && strcmp(argv[1], "roundtrip") == 0) return roundtrip_case();
    return 2;
}
