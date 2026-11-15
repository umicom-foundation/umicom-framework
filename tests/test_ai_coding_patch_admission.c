/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_coding_patch_admission.c
 *
 * PURPOSE:
 *   Verify bounded patch metadata, unchanged rejected mutations and callback admission before apply or revert.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/coding_patch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Keep large patch values on the heap and evaluate checks in release builds.
 * Every failure goes through cleanup, independently of assert/NDEBUG. */
#define REQUIRE(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); goto cleanup; \
} } while (0)
#define CASE(name) (strcmp(mode, (name)) == 0)

typedef struct PatchMemory {
    char text[UMI_AI_CODING_PATCH_CONTENT_CAPACITY];
    int present, reads, writes, removes;
    int fail_read, oversized_read, fail_write;
} PatchMemory;

static UmiStatus memory_read(void *user, const char *path, char *out,
    size_t capacity, size_t *length)
{
    PatchMemory *memory = user;
    (void)path;
    ++memory->reads;
    if (memory->fail_read) return UMI_STATUS_IO_ERROR;
    if (!memory->present) return UMI_STATUS_NOT_FOUND;
    if (memory->oversized_read) {
        *length = memory->oversized_read == 1 ? capacity : SIZE_MAX;
        return UMI_STATUS_OK;
    }
    *length = strlen(memory->text);
    if (*length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Initialize only bytes promised by the callback contract, no terminator. */
    memcpy(out, memory->text, *length);
    return UMI_STATUS_OK;
}

static UmiStatus memory_write(void *user, const char *path, const char *text, size_t length)
{
    PatchMemory *memory = user;
    (void)path;
    ++memory->writes;
    if (memory->fail_write && memory->writes == 1) return UMI_STATUS_IO_ERROR;
    if (length >= sizeof memory->text) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(memory->text, text, length);
    memory->text[length] = '\0'; memory->present = 1;
    return UMI_STATUS_OK;
}

static UmiStatus memory_remove(void *user, const char *path)
{
    PatchMemory *memory = user;
    (void)path;
    ++memory->removes;
    if (!memory->present) return UMI_STATUS_NOT_FOUND;
    memory->present = 0; memory->text[0] = '\0';
    return UMI_STATUS_OK;
}

static UmiStatus make_patch(UmiAiCodingPatch *patch)
{
    UmiStatus status = umi_ai_coding_patch_init(patch, "patch.example", "request.example",
        "Update calculation", "Use the reviewed calculation");
    if (status == UMI_STATUS_OK)
        status = umi_ai_coding_patch_add_file(patch, "src/example.c",
            UMI_AI_CODING_PATCH_MODIFY, "before\n", "after\n");
    return status;
}

static int integrity_case(const char *mode)
{
    int result = 1;
    UmiAiCodingPatch *patch = malloc(sizeof *patch);
    UmiAiCodingPatch *before = malloc(sizeof *before);
    UmiAiCodingPatchPolicy policy = umi_ai_coding_patch_policy_default();
    UmiStatus expected = UMI_STATUS_INVALID_STATE, actual;
    int use_policy = 0;
    REQUIRE(patch != NULL && before != NULL);
    REQUIRE(make_patch(patch) == UMI_STATUS_OK);
    if (CASE("valid")) expected = UMI_STATUS_OK;
    else if (CASE("empty-draft")) {
        REQUIRE(umi_ai_coding_patch_init(patch, "p", "r", "t", "why") == UMI_STATUS_OK);
        expected = UMI_STATUS_OK;
    } else if (CASE("empty-policy")) {
        REQUIRE(umi_ai_coding_patch_init(patch, "p", "r", "t", "why") == UMI_STATUS_OK);
        use_policy = 1; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (CASE("size")) patch->structure_size = 0U;
    else if (CASE("abi")) patch->abi_version = 0U;
    else if (CASE("count")) patch->file_count = UMI_AI_CODING_PATCH_FILE_MAX + 1U;
    else if (CASE("count-size-max")) patch->file_count = SIZE_MAX;
    else if (CASE("revision-zero")) patch->revision = 0U;
    else if (CASE("revision-final")) { patch->revision = UINT64_MAX; expected = UMI_STATUS_OK; }
    else if (CASE("state-low")) patch->state = (UmiAiCodingPatchState)0;
    else if (CASE("state-high")) patch->state = (UmiAiCodingPatchState)99;
    else if (CASE("approval-empty")) patch->state = UMI_AI_CODING_PATCH_APPROVED;
    else if (CASE("id-bound")) memset(patch->patch_id, 'x', sizeof patch->patch_id);
    else if (CASE("request-bound")) memset(patch->request_id, 'x', sizeof patch->request_id);
    else if (CASE("title-bound")) memset(patch->title, 'x', sizeof patch->title);
    else if (CASE("rationale-bound")) memset(patch->rationale, 'x', sizeof patch->rationale);
    else if (CASE("approver-bound")) memset(patch->approved_by, 'x', sizeof patch->approved_by);
    else if (CASE("path-bound")) memset(patch->files[0].path, 'x', sizeof patch->files[0].path);
    else if (CASE("unsafe-path")) {
        strcpy(patch->files[0].path, "../outside.c"); expected = UMI_STATUS_PERMISSION_DENIED;
    } else if (CASE("operation-low")) patch->files[0].operation = (UmiAiCodingPatchOperation)0;
    else if (CASE("operation-high")) patch->files[0].operation = (UmiAiCodingPatchOperation)99;
    else if (CASE("before-length")) patch->files[0].before_length = SIZE_MAX;
    else if (CASE("after-length")) patch->files[0].after_length = UMI_AI_CODING_PATCH_CONTENT_CAPACITY;
    else if (CASE("before-bound")) memset(patch->files[0].before_text, 'x', sizeof patch->files[0].before_text);
    else if (CASE("after-bound")) memset(patch->files[0].after_text, 'x', sizeof patch->files[0].after_text);
    else if (CASE("early-terminator")) patch->files[0].before_text[1] = '\0';
    else if (CASE("before-hash")) ++patch->files[0].before_hash;
    else if (CASE("after-hash")) ++patch->files[0].after_hash;
    else if (CASE("removed-lines")) ++patch->files[0].removed_lines;
    else if (CASE("added-lines")) ++patch->files[0].added_lines;
    else if (CASE("changed-lines")) ++patch->changed_lines;
    else if (CASE("create-before")) patch->files[0].operation = UMI_AI_CODING_PATCH_CREATE;
    else if (CASE("delete-after")) patch->files[0].operation = UMI_AI_CODING_PATCH_DELETE;
    else if (CASE("duplicate")) {
        patch->files[1] = patch->files[0]; patch->file_count = 2U;
        patch->changed_lines *= 2U; expected = UMI_STATUS_ALREADY_EXISTS;
    } else if (CASE("later-path-bound")) {
        patch->files[1] = patch->files[0]; patch->file_count = 2U;
        patch->changed_lines *= 2U;
        memset(patch->files[1].path, 'x', sizeof patch->files[1].path);
    } else if (CASE("policy-files-zero")) {
        policy.maximum_files = 0U; use_policy = 1; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (CASE("policy-files-excess")) {
        policy.maximum_files = SIZE_MAX; use_policy = 1; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (CASE("policy-lines-zero")) {
        policy.maximum_changed_lines = 0U; use_policy = 1; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (CASE("policy-lines-exact")) {
        policy.maximum_changed_lines = patch->changed_lines; use_policy = 1; expected = UMI_STATUS_OK;
    } else if (CASE("policy-lines-insufficient")) {
        policy.maximum_changed_lines = 1U; use_policy = 1; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (CASE("policy-negative-flags")) {
        policy.allow_create = policy.allow_delete = policy.require_approval = -1;
        use_policy = 1; expected = UMI_STATUS_OK;
    } else if (CASE("null-inputs")) {
        REQUIRE(UmiAiCodingPatchCheckIntegrity(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiAiCodingPatchPolicyValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(umi_ai_coding_patch_validate(patch, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        result = 0; goto cleanup;
    } else { fprintf(stderr, "unknown integrity case: %s\n", mode); goto cleanup; }
    memcpy(before, patch, sizeof *before);
    actual = use_policy ? umi_ai_coding_patch_validate(patch, &policy) :
        UmiAiCodingPatchCheckIntegrity(patch);
    REQUIRE(actual == expected);
    REQUIRE(memcmp(before, patch, sizeof *before) == 0);
    result = 0;
cleanup:
    free(before); free(patch); return result;
}

static int mutation_case(const char *mode)
{
    int result = 1;
    UmiAiCodingPatch *patch = malloc(sizeof *patch);
    UmiAiCodingPatch *before = malloc(sizeof *before);
    char oversized[UMI_AI_CODING_PATCH_CONTENT_CAPACITY];
    UmiStatus status, expected = UMI_STATUS_CAPACITY_EXCEEDED;
    REQUIRE(patch != NULL && before != NULL);
    REQUIRE(make_patch(patch) == UMI_STATUS_OK);
    memset(oversized, 'x', sizeof oversized);
    if (CASE("capacity")) {
        for (size_t i = 1U; i < UMI_AI_CODING_PATCH_FILE_MAX; ++i) {
            char path[64]; snprintf(path, sizeof path, "src/file%zu.c", i);
            REQUIRE(umi_ai_coding_patch_add_file(patch, path, UMI_AI_CODING_PATCH_MODIFY, "a", "b") == UMI_STATUS_OK);
        }
    }
    if (CASE("max-add") || CASE("max-approve") || CASE("max-reject")) patch->revision = UINT64_MAX;
    if (CASE("corrupt-add") || CASE("corrupt-approve") || CASE("corrupt-reject")) {
        patch->file_count = SIZE_MAX; expected = UMI_STATUS_INVALID_STATE;
    }
    if (CASE("last-add") || CASE("last-approve") || CASE("last-reject")) patch->revision = UINT64_MAX - 1U;
    memcpy(before, patch, sizeof *before);
    if (CASE("max-approve") || CASE("corrupt-approve") || CASE("last-approve"))
        status = umi_ai_coding_patch_approve(patch, "reviewer");
    else if (CASE("max-reject") || CASE("corrupt-reject") || CASE("last-reject"))
        status = umi_ai_coding_patch_reject(patch);
    else if (CASE("approver-bound")) status = umi_ai_coding_patch_approve(patch, oversized);
    else if (CASE("path-bound")) status = umi_ai_coding_patch_add_file(patch, oversized, UMI_AI_CODING_PATCH_MODIFY, "a", "b");
    else if (CASE("before-bound")) status = umi_ai_coding_patch_add_file(patch, "src/other.c", UMI_AI_CODING_PATCH_MODIFY, oversized, "b");
    else if (CASE("after-bound")) status = umi_ai_coding_patch_add_file(patch, "src/other.c", UMI_AI_CODING_PATCH_MODIFY, "a", oversized);
    else if (CASE("duplicate")) {
        expected = UMI_STATUS_ALREADY_EXISTS;
        status = umi_ai_coding_patch_add_file(patch, "src/example.c", UMI_AI_CODING_PATCH_MODIFY, "a", "b");
    } else if (CASE("borrow-slot")) {
        strcpy(patch->files[1].path, "src/borrowed.c");
        strcpy(patch->files[1].before_text, "borrowed before");
        strcpy(patch->files[1].after_text, "borrowed after");
        REQUIRE(umi_ai_coding_patch_add_file(patch, patch->files[1].path, UMI_AI_CODING_PATCH_MODIFY,
            patch->files[1].before_text, patch->files[1].after_text) == UMI_STATUS_OK);
        REQUIRE(strcmp(patch->files[1].after_text, "borrowed after") == 0);
        REQUIRE(UmiAiCodingPatchCheckIntegrity(patch) == UMI_STATUS_OK);
        result = 0; goto cleanup;
    } else if (CASE("borrow-approver")) {
        strcpy(patch->approved_by, "prior reviewer");
        REQUIRE(umi_ai_coding_patch_approve(patch, patch->approved_by + 6) == UMI_STATUS_OK);
        REQUIRE(strcmp(patch->approved_by, "reviewer") == 0);
        result = 0; goto cleanup;
    } else if (CASE("create-before") || CASE("delete-after")) {
        expected = UMI_STATUS_INVALID_ARGUMENT;
        status = umi_ai_coding_patch_add_file(patch, "src/other.c",
            CASE("create-before") ? UMI_AI_CODING_PATCH_CREATE : UMI_AI_CODING_PATCH_DELETE, "a", "b");
    } else if (CASE("max-add") || CASE("last-add") || CASE("corrupt-add") || CASE("capacity"))
        status = umi_ai_coding_patch_add_file(patch, "src/other.c", UMI_AI_CODING_PATCH_MODIFY, "a", "b");
    else { fprintf(stderr, "unknown mutation case: %s\n", mode); goto cleanup; }
    if (CASE("last-add") || CASE("last-approve") || CASE("last-reject")) {
        REQUIRE(status == UMI_STATUS_OK && patch->revision == UINT64_MAX);
        REQUIRE(UmiAiCodingPatchCheckIntegrity(patch) == UMI_STATUS_OK);
    } else {
        REQUIRE(status == expected); REQUIRE(memcmp(before, patch, sizeof *before) == 0);
    }
    result = 0;
cleanup:
    free(before); free(patch); return result;
}

static int adapter_case(const char *mode)
{
    int result = 1, reverting = strncmp(mode, "revert-", 7U) == 0;
    UmiAiCodingPatch *patch = malloc(sizeof *patch);
    UmiAiCodingPatch *before = malloc(sizeof *before);
    PatchMemory memory = {.text = "before\n", .present = 1};
    UmiAiCodingFileAdapter adapter = {sizeof adapter, UMI_AI_CODING_ABI_VERSION,
        memory_read, memory_write, memory_remove, &memory};
    UmiAiCodingPatchPolicy policy = umi_ai_coding_patch_policy_default();
    UmiStatus status, expected = UMI_STATUS_CAPACITY_EXCEEDED;
    REQUIRE(patch != NULL && before != NULL);
    REQUIRE(make_patch(patch) == UMI_STATUS_OK);
    REQUIRE(umi_ai_coding_patch_approve(patch, "reviewer") == UMI_STATUS_OK);
    if (reverting) {
        REQUIRE(umi_ai_coding_patch_apply(patch, &policy, &adapter) == UMI_STATUS_OK);
        memory.reads = memory.writes = memory.removes = 0;
    }
    if (CASE("apply-bound") || CASE("revert-bound")) memory.oversized_read = 1;
    else if (CASE("apply-size-max") || CASE("revert-size-max")) memory.oversized_read = 2;
    else if (CASE("apply-max") || CASE("revert-max")) patch->revision = UINT64_MAX;
    else if (CASE("apply-last") || CASE("revert-last")) {
        patch->revision = UINT64_MAX - 1U; expected = UMI_STATUS_OK;
    } else if (CASE("apply-corrupt-count") || CASE("revert-corrupt-count")) {
        patch->file_count = SIZE_MAX; expected = UMI_STATUS_INVALID_STATE;
    } else if (CASE("apply-read-error") || CASE("revert-read-error")) {
        memory.fail_read = 1; expected = UMI_STATUS_IO_ERROR;
    } else if (CASE("apply-conflict") || CASE("revert-conflict")) {
        strcpy(memory.text, "someone else changed this"); expected = UMI_STATUS_INVALID_STATE;
    } else if (CASE("apply-valid") || CASE("revert-valid")) expected = UMI_STATUS_OK;
    else if (CASE("apply-write-error") || CASE("revert-write-error")) {
        memory.fail_write = 1; expected = UMI_STATUS_IO_ERROR;
    } else if (CASE("apply-negative-approval")) {
        policy.require_approval = -1; patch->state = UMI_AI_CODING_PATCH_DRAFT;
        expected = UMI_STATUS_PERMISSION_DENIED;
    } else { fprintf(stderr, "unknown adapter case: %s\n", mode); goto cleanup; }
    memcpy(before, patch, sizeof *before);
    status = reverting ? umi_ai_coding_patch_revert(patch, &adapter) :
        umi_ai_coding_patch_apply(patch, &policy, &adapter);
    REQUIRE(status == expected);
    if (expected == UMI_STATUS_OK) {
        REQUIRE(memory.reads == 1 && memory.writes == 1 && memory.removes == 0);
        REQUIRE(strcmp(memory.text, reverting ? "before\n" : "after\n") == 0);
        REQUIRE(patch->revision == before->revision + 1U);
        REQUIRE(patch->state == (reverting ? UMI_AI_CODING_PATCH_REVERTED : UMI_AI_CODING_PATCH_APPLIED));
        REQUIRE(UmiAiCodingPatchCheckIntegrity(patch) == UMI_STATUS_OK);
    } else {
/* Transaction failure now records attempted mutation in the patch revision and performs a restoration readback. The earlier no-revision expectation is retained for review. The former implementation is retained for owner review. */
#if 0
        REQUIRE(memcmp(before, patch, sizeof *before) == 0);
        if (memory.fail_write) {
            REQUIRE(memory.writes == 2 && memory.removes == 0);
            REQUIRE(strcmp(memory.text, reverting ? "after\n" : "before\n") == 0);
        } else REQUIRE(memory.writes == 0 && memory.removes == 0);
#endif
        if (memory.fail_write) {
            /* An attempted mutation advances the observation revision even
             * when compensation restores the original bytes. */
            ++before->revision;
            REQUIRE(memcmp(before, patch, sizeof *before) == 0);
            REQUIRE(memory.writes == 2 && memory.removes == 0 && memory.reads == 2);
            REQUIRE(strcmp(memory.text, reverting ? "after\n" : "before\n") == 0);
        } else {
            REQUIRE(memcmp(before, patch, sizeof *before) == 0);
            REQUIRE(memory.writes == 0 && memory.removes == 0);
        }
        if (CASE("apply-max") || CASE("revert-max") ||
            CASE("apply-corrupt-count") || CASE("revert-corrupt-count") ||
            CASE("apply-negative-approval")) REQUIRE(memory.reads == 0);
    }
    result = 0;
cleanup:
    free(before); free(patch); return result;
}

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    if (strcmp(argv[1], "integrity") == 0) return integrity_case(argv[2]);
    if (strcmp(argv[1], "mutation") == 0) return mutation_case(argv[2]);
    if (strcmp(argv[1], "adapter") == 0) return adapter_case(argv[2]);
    return 2;
}
