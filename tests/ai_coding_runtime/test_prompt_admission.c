/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_prompt_admission.c
 *
 * PURPOSE:
 *   Verify prompt input provenance, byte feedback and failure-preserving provider request construction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai_coding_runtime/prompt.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); goto cleanup; \
} } while (0)

typedef struct PromptFixture {
    UmiAiCodingTaskPlan plan;
    UmiAiCodingMaterializedContext context;
    UmiAiCodingRuntimeConfig config;
    UmiAiRequest output, before;
} PromptFixture;

static void set_text(UmiAiCodingMaterializedFile *file, const char *text)
{
    strcpy(file->text, text); file->length = strlen(text);
    file->hash = umi_ai_coding_text_hash(file->text, file->length);
}
static void seed(PromptFixture *f, size_t count)
{
    memset(f, 0, sizeof *f);
    umi_ai_coding_request_init(&f->plan.request, UMI_AI_CODING_TASK_REFACTOR);
    strcpy(f->plan.request.request_id, "request.example");
    strcpy(f->plan.request.session_id, "session.example");
    strcpy(f->plan.request.runtime_id, "runtime.example");
    strcpy(f->plan.request.workspace_root, "memory");
    strcpy(f->plan.request.instruction, "Explain a small improvement");
    strcpy(f->plan.request.active_path, "src/file0.c");
    f->plan.ready = 1; f->plan.revision = 19U;
    f->plan.repository_context.file_count = count;
    f->context.revision = 19U; f->context.file_count = count;
    f->context.estimated_tokens = (uint32_t)count * 4U;
    for (size_t i = 0U; i < count; ++i) {
        UmiAiCodingContextFile *selected = &f->plan.repository_context.files[i];
        UmiAiCodingMaterializedFile *file = &f->context.files[i];
        (void)snprintf(selected->path, sizeof selected->path, "src/file%zu.c", i);
        strcpy(selected->language_id, "c"); selected->priority = 1U;
        selected->estimated_tokens = 4U; selected->classification = UMI_AI_DATA_INTERNAL;
        selected->enabled = 1;
        strcpy(file->path, selected->path); strcpy(file->language_id, "c");
        file->estimated_tokens = 4U; file->active = i == 0U;
        set_text(file, "alpha\nbeta\n");
    }
    umi_ai_coding_runtime_config_init(&f->config);
    strcpy(f->config.provider_id, "local.fixture"); strcpy(f->config.model_id, "model.fixture");
    memset(&f->output, 0x5a, sizeof f->output); f->before = f->output;
}

static int invalid_case(const char *mode)
{
    PromptFixture *f = calloc(1U, sizeof *f);
    const UmiAiCodingTaskPlan *plan;
    const UmiAiCodingMaterializedContext *context;
    const UmiAiCodingRuntimeConfig *config;
    UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
    int result = 1;
    REQUIRE(f != NULL); seed(f, 2U);
    plan = &f->plan; context = &f->context; config = &f->config;
    if (strcmp(mode, "null-plan") == 0) plan = NULL;
    else if (strcmp(mode, "null-context") == 0) context = NULL;
    else if (strcmp(mode, "null-config") == 0) config = NULL;
    else if (strcmp(mode, "not-ready") == 0) f->plan.ready = 0;
    else if (strcmp(mode, "request-id") == 0) memset(f->plan.request.request_id, 'x', sizeof f->plan.request.request_id);
    else if (strcmp(mode, "instruction") == 0) memset(f->plan.request.instruction, 'x', sizeof f->plan.request.instruction);
    else if (strcmp(mode, "task") == 0) f->plan.request.task = (UmiAiCodingTaskKind)-1;
    else if (strcmp(mode, "active-path") == 0) strcpy(f->plan.request.active_path, "../outside.c");
    else if (strcmp(mode, "selection") == 0) { f->plan.request.selection_start_line = 4U; f->plan.request.selection_end_line = 2U; }
    else if (strcmp(mode, "plan-count") == 0) f->plan.repository_context.file_count = SIZE_MAX;
    else if (strcmp(mode, "priority") == 0) f->plan.repository_context.files[1].priority = 0U;
    else if (strcmp(mode, "plan-duplicate") == 0) f->plan.repository_context.files[1] = f->plan.repository_context.files[0];
    else if (strcmp(mode, "temperature") == 0) f->config.temperature = NAN;
    else {
        expected = UMI_STATUS_INVALID_STATE;
        if (strcmp(mode, "context-count") == 0) f->context.file_count = SIZE_MAX;
        else if (strcmp(mode, "context-length") == 0) f->context.files[1].length = SIZE_MAX;
        else if (strcmp(mode, "context-hash") == 0) f->context.files[1].hash ^= 1U;
        else if (strcmp(mode, "revision") == 0) ++f->context.revision;
        else if (strcmp(mode, "unselected-path") == 0) strcpy(f->context.files[1].path, "src/unselected.c");
        else if (strcmp(mode, "tokens") == 0) { ++f->context.files[1].estimated_tokens; ++f->context.estimated_tokens; }
        else if (strcmp(mode, "language") == 0) strcpy(f->context.files[1].language_id, "java");
        else if (strcmp(mode, "active") == 0) f->context.files[1].active = 1;
        else if (strcmp(mode, "unaccounted-skip") == 0) { f->context.file_count = 1U; f->context.estimated_tokens = 4U; }
        else { result = 2; goto cleanup; }
    }
    REQUIRE(UmiAiCodingBuildProviderRequestWithFeedback(plan, context, config, NULL, 0U, &f->output) == expected);
    REQUIRE(memcmp(&f->output, &f->before, sizeof f->output) == 0);
    REQUIRE(umi_ai_coding_build_provider_request(plan, context, config, NULL, &f->output) == expected);
    REQUIRE(memcmp(&f->output, &f->before, sizeof f->output) == 0);
    result = 0;
cleanup:
    free(f); return result;
}

static int feedback_case(const char *mode)
{
    PromptFixture *f = calloc(1U, sizeof *f);
    char bytes[] = {'r', 'e', 't', 'r', 'y'};
    const char *feedback = bytes;
    char *large = NULL;
    size_t length = sizeof bytes;
    UmiStatus expected = UMI_STATUS_OK;
    int result = 1, legacy = 0;
    REQUIRE(f != NULL); seed(f, 1U);
    if (strcmp(mode, "span") == 0) { /* No terminator is provided or needed. */ }
    else if (strcmp(mode, "null-empty") == 0) { feedback = NULL; length = 0U; }
    else if (strcmp(mode, "null-length") == 0) { feedback = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(mode, "embedded-nul") == 0) { bytes[2] = '\0'; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(mode, "size-max") == 0) { length = SIZE_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(mode, "alias") == 0) {
        strcpy(f->output.messages[0].text, "retry"); f->before = f->output;
        feedback = f->output.messages[0].text;
    } else if (strcmp(mode, "legacy-limit") == 0 || strcmp(mode, "message-limit") == 0) {
        size_t limit = (size_t)UMI_AI_MAX_MESSAGES * (UMI_AI_TEXT_CAPACITY - 1U);
        large = malloc(limit + 2U); REQUIRE(large != NULL);
        memset(large, 'x', limit + 1U); large[limit + 1U] = '\0';
        feedback = large; expected = UMI_STATUS_CAPACITY_EXCEEDED;
        legacy = strcmp(mode, "legacy-limit") == 0;
        length = limit;
    } else { result = 2; goto cleanup; }
    UmiStatus status = legacy ? umi_ai_coding_build_provider_request(&f->plan, &f->context, &f->config, feedback, &f->output) :
        UmiAiCodingBuildProviderRequestWithFeedback(&f->plan, &f->context, &f->config, feedback, length, &f->output);
    REQUIRE(status == expected);
    if (expected != UMI_STATUS_OK) REQUIRE(memcmp(&f->output, &f->before, sizeof f->output) == 0);
    else if (length != 0U) {
        REQUIRE(strcmp(f->output.messages[f->output.message_count - 1U].name, "validation-feedback") == 0);
        REQUIRE(strcmp(f->output.messages[f->output.message_count - 1U].text, "retry") == 0);
    } else REQUIRE(f->output.message_count == 4U);
    result = 0;
cleanup:
    free(large); free(f); return result;
}

static int success_case(const char *mode)
{
    PromptFixture *f = calloc(1U, sizeof *f);
    size_t expected = 5U;
    int result = 1;
    REQUIRE(f != NULL); seed(f, 2U);
    if (strcmp(mode, "refactor") == 0) { }
    else if (strcmp(mode, "explain") == 0) { f->plan.request.task = UMI_AI_CODING_TASK_EXPLAIN; expected = 4U; }
    else if (strcmp(mode, "selection") == 0) { f->plan.request.selection_start_line = 2U; f->plan.request.selection_end_line = 2U; expected = 6U; }
    else if (strcmp(mode, "empty-file") == 0) set_text(&f->context.files[1], "");
    else if (strcmp(mode, "chunked") == 0) {
        UmiAiCodingMaterializedFile *file = &f->context.files[1];
        file->length = 2U * (UMI_AI_TEXT_CAPACITY - 1U) + 1U;
        memset(file->text, 'z', file->length); file->text[file->length] = '\0';
        file->hash = umi_ai_coding_text_hash(file->text, file->length); expected = 7U;
    } else if (strcmp(mode, "skipped") == 0) {
        f->context.files[0] = f->context.files[1]; f->context.file_count = 1U;
        f->context.skipped_count = 1U; f->context.estimated_tokens = 4U; expected = 4U;
    } else if (strcmp(mode, "zero-files") == 0) { seed(f, 0U); expected = 3U; }
    else { result = 2; goto cleanup; }
    REQUIRE(umi_ai_coding_build_provider_request(&f->plan, &f->context, &f->config, NULL, &f->output) == UMI_STATUS_OK);
    REQUIRE(f->output.message_count == expected);
    REQUIRE(strcmp(f->output.request_id, "request.example") == 0);
    REQUIRE(strcmp(f->output.model_id, "model.fixture") == 0);
    REQUIRE(strcmp(f->output.messages[0].name, "umicom-coding-policy") == 0);
    if (strcmp(mode, "selection") == 0) {
        REQUIRE(strcmp(f->output.messages[3].name, "active-selection") == 0);
        REQUIRE(strcmp(f->output.messages[3].text, "beta\n") == 0);
    }
    if (strcmp(mode, "chunked") == 0) {
        REQUIRE(strlen(f->output.messages[4].text) == UMI_AI_TEXT_CAPACITY - 1U);
        REQUIRE(strlen(f->output.messages[5].text) == UMI_AI_TEXT_CAPACITY - 1U);
        REQUIRE(strcmp(f->output.messages[6].text, "z") == 0);
    }
    if (strcmp(mode, "empty-file") == 0) REQUIRE(f->output.messages[4].text[0] == '\0');
    result = 0;
cleanup:
    free(f); return result;
}

static int capacity_case(const char *mode)
{
    PromptFixture *f = calloc(1U, sizeof *f);
    int result = 1;
    REQUIRE(f != NULL);
    if (strcmp(mode, "messages") == 0) {
        seed(f, UMI_AI_CODING_RUNTIME_CONTEXT_CAPACITY);
        for (size_t i = 0U; i < f->context.file_count; ++i) {
            UmiAiCodingMaterializedFile *file = &f->context.files[i];
            file->length = sizeof file->text - 1U;
            memset(file->text, 'x', file->length); file->text[file->length] = '\0';
            file->hash = umi_ai_coding_text_hash(file->text, file->length);
        }
    } else if (strcmp(mode, "path-name") == 0) {
        seed(f, 2U);
        memset(f->plan.repository_context.files[1].path, 'x', UMI_AI_ID_CAPACITY);
        f->plan.repository_context.files[1].path[UMI_AI_ID_CAPACITY] = '\0';
        strcpy(f->context.files[1].path, f->plan.repository_context.files[1].path);
    } else { result = 2; goto cleanup; }
    REQUIRE(umi_ai_coding_build_provider_request(&f->plan, &f->context, &f->config, NULL, &f->output) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(memcmp(&f->output, &f->before, sizeof f->output) == 0);
    result = 0;
cleanup:
    free(f); return result;
}

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    if (strcmp(argv[1], "invalid") == 0) return invalid_case(argv[2]);
    if (strcmp(argv[1], "feedback") == 0) return feedback_case(argv[2]);
    if (strcmp(argv[1], "success") == 0) return success_case(argv[2]);
    if (strcmp(argv[1], "capacity") == 0) return capacity_case(argv[2]);
    return 2;
}
