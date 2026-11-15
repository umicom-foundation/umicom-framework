/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_materializer_admission.c
 *
 * PURPOSE:
 *   Exercise malicious adapter lengths, metadata admission, stable callback selection and preserved materialized outputs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai_coding_runtime/context_materializer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); goto cleanup; \
} } while (0)

typedef struct Fixture {
    UmiAiCodingTaskPlan plan;
    UmiAiCodingWorkspaceAdapter adapter;
    UmiAiCodingMaterializedContext output, before;
    const char *mode;
    size_t reads;
    int early_publication;
} Fixture;

static UmiStatus read_text(void *user, const char *path, char *buffer, size_t capacity, size_t *length)
{
    Fixture *f = user;
    ++f->reads;
    if (memcmp(&f->output, &f->before, sizeof f->output) != 0) f->early_publication = 1;
    memset(buffer, 'x', capacity);
    *length = 3U;
    if (strcmp(f->mode, "capacity-length") == 0) *length = capacity;
    else if (strcmp(f->mode, "size-max") == 0) *length = SIZE_MAX;
    else if (strcmp(f->mode, "embedded-nul") == 0) buffer[1] = '\0';
    else if (strcmp(f->mode, "error") == 0) return UMI_STATUS_IO_ERROR;
    else if (strcmp(f->mode, "late-error") == 0 && f->reads == 2U) return UMI_STATUS_IO_ERROR;
    else if (strcmp(f->mode, "late-oversize") == 0 && f->reads == 2U) *length = capacity;
    else if (strcmp(f->mode, "empty") == 0) *length = 0U;
    else if (strcmp(f->mode, "exact-max") == 0) *length = capacity - 1U;
    else if (strcmp(f->mode, "skips") == 0) {
        /* Failed callbacks deliberately report nonsense bytes and lengths;
         * only their established skip status is relevant to the result. */
        *length = SIZE_MAX;
        if (f->reads == 1U) return UMI_STATUS_NOT_FOUND;
        if (f->reads == 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        if (f->reads == 3U) return UMI_STATUS_PARSE_ERROR;
        *length = 3U;
    } else if (strcmp(f->mode, "snapshot") == 0 && f->reads == 1U) {
        /* Reentrant edits of caller metadata must not change the in-flight
         * selection or redirect the next callback through a new adapter. */
        memset(&f->plan, 0, sizeof f->plan);
        f->adapter.read = NULL;
    }
    (void)path;
    if (strcmp(f->mode, "without-terminator") != 0 && *length < capacity)
        buffer[*length] = '\0';
    return UMI_STATUS_OK;
}
static UmiStatus write_text(void *user, const char *path, const char *text, size_t length)
{ (void)user; (void)path; (void)text; (void)length; return UMI_STATUS_INVALID_STATE; }
static UmiStatus remove_file(void *user, const char *path)
{ (void)user; (void)path; return UMI_STATUS_INVALID_STATE; }
static UmiStatus exists_file(void *user, const char *path, int *exists)
{ (void)user; (void)path; *exists = 1; return UMI_STATUS_OK; }

static void seed(Fixture *f, const char *mode, size_t count)
{
    memset(f, 0, sizeof *f);
    f->mode = mode;
    f->plan.ready = 1; f->plan.revision = 7U;
    f->plan.repository_context.file_count = count;
    strcpy(f->plan.request.active_path, "src/file0.c");
    for (size_t i = 0U; i < count; ++i) {
        UmiAiCodingContextFile *file = &f->plan.repository_context.files[i];
        (void)snprintf(file->path, sizeof file->path, "src/file%zu.c", i);
        strcpy(file->language_id, "c"); strcpy(file->summary, "Memory source");
        file->classification = UMI_AI_DATA_INTERNAL;
        file->estimated_tokens = 4U; file->priority = 1U; file->enabled = 1;
    }
    f->adapter = (UmiAiCodingWorkspaceAdapter){sizeof f->adapter,
        UMI_AI_CODING_RUNTIME_API_VERSION, read_text, write_text, remove_file, exists_file, f};
    memset(&f->output, 0x5a, sizeof f->output); f->before = f->output;
}

static int plan_case(const char *mode)
{
    Fixture *f = calloc(1U, sizeof *f);
    const UmiAiCodingTaskPlan *input;
    UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
    int result = 1;
    REQUIRE(f != NULL);
    seed(f, mode, 2U); input = &f->plan;
    UmiAiCodingContextFile *file = &f->plan.repository_context.files[1];
    if (strcmp(mode, "null") == 0) input = NULL;
    else if (strcmp(mode, "not-ready") == 0) f->plan.ready = 0;
    else if (strcmp(mode, "count") == 0) f->plan.repository_context.file_count = SIZE_MAX;
    else if (strcmp(mode, "path") == 0) memset(file->path, 'x', sizeof file->path);
    else if (strcmp(mode, "unsafe") == 0) strcpy(file->path, "../outside.c");
    else if (strcmp(mode, "language") == 0) memset(file->language_id, 'x', sizeof file->language_id);
    else if (strcmp(mode, "summary") == 0) memset(file->summary, 'x', sizeof file->summary);
    else if (strcmp(mode, "priority") == 0) file->priority = 0U;
    else if (strcmp(mode, "tokens") == 0) file->estimated_tokens = 0U;
    else if (strcmp(mode, "classification") == 0) file->classification = (UmiAiDataClassification)-1;
    else if (strcmp(mode, "active-path") == 0) memset(f->plan.request.active_path, 'x', sizeof f->plan.request.active_path);
    else if (strcmp(mode, "active-unsafe") == 0) strcpy(f->plan.request.active_path, "../outside.c");
    else if (strcmp(mode, "duplicate") == 0) strcpy(file->path, f->plan.repository_context.files[0].path);
    else if (strcmp(mode, "token-overflow") == 0) { file->estimated_tokens = UINT32_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(mode, "adapter-version") == 0) f->adapter.api_version = 0U;
    else if (strcmp(mode, "adapter-read") == 0) f->adapter.read = NULL;
    else { result = 2; goto cleanup; }
    REQUIRE(umi_ai_coding_materialize_context(input, &f->adapter, &f->output) == expected);
    REQUIRE(f->reads == 0U && memcmp(&f->output, &f->before, sizeof f->output) == 0);
    result = 0;
cleanup:
    free(f); return result;
}

static int read_case(const char *mode)
{
    Fixture *f = calloc(1U, sizeof *f);
    UmiStatus expected = UMI_STATUS_CAPACITY_EXCEEDED;
    int result = 1;
    REQUIRE(f != NULL); seed(f, mode, 2U);
    if (strcmp(mode, "embedded-nul") == 0) expected = UMI_STATUS_PARSE_ERROR;
    else if (strcmp(mode, "error") == 0 || strcmp(mode, "late-error") == 0) expected = UMI_STATUS_IO_ERROR;
    else if (strcmp(mode, "capacity-length") != 0 && strcmp(mode, "size-max") != 0 && strcmp(mode, "late-oversize") != 0) { result = 2; goto cleanup; }
    REQUIRE(umi_ai_coding_materialize_context(&f->plan, &f->adapter, &f->output) == expected);
    REQUIRE(f->reads == (strncmp(mode, "late-", 5U) == 0 ? 2U : 1U));
    REQUIRE(!f->early_publication && memcmp(&f->output, &f->before, sizeof f->output) == 0);
    result = 0;
cleanup:
    free(f); return result;
}

static int success_case(const char *mode)
{
    Fixture *f = calloc(1U, sizeof *f);
    size_t count = 2U, wanted = 2U, skipped = 0U;
    int truncated = 0, result = 1;
    REQUIRE(f != NULL);
    if (strcmp(mode, "skips") == 0) { count = 4U; wanted = 1U; skipped = 3U; truncated = 1; }
    else if (strcmp(mode, "capacity") == 0) { count = UMI_AI_CODING_CONTEXT_PLAN_MAX; wanted = UMI_AI_CODING_RUNTIME_CONTEXT_CAPACITY; skipped = count - wanted; truncated = 1; }
    else if (strcmp(mode, "zero-files") == 0) count = wanted = 0U;
    else if (strcmp(mode, "token-maximum") == 0) count = wanted = 1U;
    else if (strcmp(mode, "normal") != 0 && strcmp(mode, "empty") != 0 && strcmp(mode, "exact-max") != 0 && strcmp(mode, "without-terminator") != 0 && strcmp(mode, "snapshot") != 0) { result = 2; goto cleanup; }
    seed(f, mode, count);
    if (strcmp(mode, "token-maximum") == 0) f->plan.repository_context.files[0].estimated_tokens = UINT32_MAX;
    REQUIRE(umi_ai_coding_materialize_context(&f->plan, &f->adapter, &f->output) == UMI_STATUS_OK);
    REQUIRE(UmiAiCodingMaterializedContextValidate(&f->output) == UMI_STATUS_OK);
    REQUIRE(f->output.revision == 7U && f->output.file_count == wanted);
    REQUIRE(f->output.skipped_count == skipped && f->output.truncated == truncated);
    REQUIRE(f->reads == (strcmp(mode, "capacity") == 0 ? wanted : count) && !f->early_publication);
    REQUIRE(f->output.estimated_tokens == (strcmp(mode, "token-maximum") == 0 ? UINT32_MAX : 4U * wanted));
    for (size_t i = 0U; i < wanted; ++i) {
        UmiAiCodingMaterializedFile found;
        const UmiAiCodingMaterializedFile *file = &f->output.files[i];
        size_t length = strcmp(mode, "empty") == 0 ? 0U : strcmp(mode, "exact-max") == 0 ? sizeof file->text - 1U : 3U;
        REQUIRE(file->length == length && file->text[length] == '\0');
        REQUIRE(file->active == (i == 0U && strcmp(mode, "skips") != 0));
        REQUIRE(umi_ai_coding_materialized_context_find(&f->output, file->path, &found) == UMI_STATUS_OK);
        REQUIRE(memcmp(file, &found, sizeof found) == 0);
    }
    if (strcmp(mode, "snapshot") == 0) {
        REQUIRE(f->adapter.read == NULL && f->plan.ready == 0);
        REQUIRE(strcmp(f->output.files[1].path, "src/file1.c") == 0);
    }
    result = 0;
cleanup:
    free(f); return result;
}

static int value_case(const char *mode)
{
    Fixture *f = calloc(1U, sizeof *f);
    UmiAiCodingMaterializedFile found, before;
    int result = 1;
    REQUIRE(f != NULL); seed(f, "normal", 2U);
    REQUIRE(umi_ai_coding_materialize_context(&f->plan, &f->adapter, &f->output) == UMI_STATUS_OK);
    UmiAiCodingMaterializedFile *file = &f->output.files[1];
    if (strcmp(mode, "count") == 0) f->output.file_count = SIZE_MAX;
    else if (strcmp(mode, "skips") == 0) f->output.skipped_count = SIZE_MAX;
    else if (strcmp(mode, "path") == 0) memset(file->path, 'x', sizeof file->path);
    else if (strcmp(mode, "unsafe") == 0) strcpy(file->path, "../outside.c");
    else if (strcmp(mode, "language") == 0) memset(file->language_id, 'x', sizeof file->language_id);
    else if (strcmp(mode, "length") == 0) file->length = SIZE_MAX;
    else if (strcmp(mode, "nul") == 0) file->text[1] = '\0';
    else if (strcmp(mode, "terminator") == 0) file->text[file->length] = 'x';
    else if (strcmp(mode, "hash") == 0) file->hash ^= 1U;
    else if (strcmp(mode, "tokens") == 0) file->estimated_tokens = 0U;
    else if (strcmp(mode, "aggregate") == 0) ++f->output.estimated_tokens;
    else if (strcmp(mode, "duplicate") == 0) strcpy(file->path, f->output.files[0].path);
    else if (strcmp(mode, "overflow") == 0) file->estimated_tokens = UINT32_MAX;
    else { result = 2; goto cleanup; }
    memset(&found, 0x69, sizeof found); before = found;
    REQUIRE(UmiAiCodingMaterializedContextValidate(&f->output) == UMI_STATUS_INVALID_STATE);
    /* A matching first record must not hide corrupt metadata later in the value. */
    REQUIRE(umi_ai_coding_materialized_context_find(&f->output, "src/file0.c", &found) == UMI_STATUS_INVALID_STATE);
    REQUIRE(memcmp(&found, &before, sizeof found) == 0);
    result = 0;
cleanup:
    free(f); return result;
}

static int find_case(const char *mode)
{
    Fixture *f = calloc(1U, sizeof *f);
    UmiAiCodingMaterializedFile found, before;
    char unterminated[UMI_AI_CODING_RUNTIME_PATH_CAPACITY];
    const char *query = "src/file0.c";
    UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
    int result = 1;
    REQUIRE(f != NULL); seed(f, "normal", 2U);
    REQUIRE(umi_ai_coding_materialize_context(&f->plan, &f->adapter, &f->output) == UMI_STATUS_OK);
    memset(&found, 0x69, sizeof found); before = found;
    memset(unterminated, 'x', sizeof unterminated);
    if (strcmp(mode, "null") == 0) query = NULL;
    else if (strcmp(mode, "unterminated") == 0) query = unterminated;
    else if (strcmp(mode, "unsafe") == 0) query = "../outside.c";
    else if (strcmp(mode, "missing") == 0) { query = "src/missing.c"; expected = UMI_STATUS_NOT_FOUND; }
    else if (strcmp(mode, "present") == 0) expected = UMI_STATUS_OK;
    else if (strcmp(mode, "alias") == 0) {
        REQUIRE(umi_ai_coding_materialized_context_find(&f->output, f->output.files[0].path, &f->output.files[0]) == UMI_STATUS_OK);
        REQUIRE(UmiAiCodingMaterializedContextValidate(&f->output) == UMI_STATUS_OK);
        result = 0; goto cleanup;
    } else { result = 2; goto cleanup; }
    REQUIRE(umi_ai_coding_materialized_context_find(&f->output, query, &found) == expected);
    if (expected == UMI_STATUS_OK) REQUIRE(memcmp(&found, &f->output.files[0], sizeof found) == 0);
    else REQUIRE(memcmp(&found, &before, sizeof found) == 0);
    result = 0;
cleanup:
    free(f); return result;
}

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    if (strcmp(argv[1], "plan") == 0) return plan_case(argv[2]);
    if (strcmp(argv[1], "read") == 0) return read_case(argv[2]);
    if (strcmp(argv[1], "success") == 0) return success_case(argv[2]);
    if (strcmp(argv[1], "value") == 0) return value_case(argv[2]);
    if (strcmp(argv[1], "find") == 0) return find_case(argv[2]);
    return 2;
}
