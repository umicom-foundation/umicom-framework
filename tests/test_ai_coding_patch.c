/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_coding_patch.c
 *
 * PURPOSE:
 *   Verify reviewed patch application, hash-conflict rejection, transactional
 *   rollback and controlled reversion through a host-owned file adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "value_archive/construction_cases.h"
#include <string.h>

#include "umicom/ai/coding_patch.h"

typedef struct MemoryFileSystem {
    char path[3][128];
    char text[3][1024];
    int present[3];
    int fail_write_at;
    int writes;
} MemoryFileSystem;

/*
 * Exercise find file and return a clear result when the behaviour no longer matches its
 * contract.
 */
static int find_file(MemoryFileSystem *fs, const char *path)
{
    int index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0; index < 3; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(fs->path[index], path) == 0) return index;
    }
    return -1;
}

/*
 * Exercise read file and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus read_file(void *user_data, const char *path, char *out_text,
                           size_t capacity, size_t *out_length)
{
    MemoryFileSystem *fs = (MemoryFileSystem *)user_data;
    int index = find_file(fs, path);
    size_t length;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index < 0 || !fs->present[index]) return UMI_STATUS_NOT_FOUND;
    length = strlen(fs->text[index]);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out_text, fs->text[index], length + 1U);
    *out_length = length;
    return UMI_STATUS_OK;
}

/*
 * Exercise write file and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus write_file(void *user_data, const char *path,
                            const char *text, size_t length)
{
    MemoryFileSystem *fs = (MemoryFileSystem *)user_data;
    int index = find_file(fs, path);
    ++fs->writes;
    /* Apply this branch only when its contract condition is satisfied. */
    if (fs->fail_write_at > 0 && fs->writes == fs->fail_write_at) {
        return UMI_STATUS_IO_ERROR;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index < 0 || length >= sizeof(fs->text[index])) return UMI_STATUS_IO_ERROR;
    (void)memcpy(fs->text[index], text, length);
    fs->text[index][length] = '\0';
    fs->present[index] = 1;
    return UMI_STATUS_OK;
}

/*
 * Exercise remove file and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus remove_file(void *user_data, const char *path)
{
    MemoryFileSystem *fs = (MemoryFileSystem *)user_data;
    int index = find_file(fs, path);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index < 0 || !fs->present[index]) return UMI_STATUS_NOT_FOUND;
    fs->present[index] = 0;
    fs->text[index][0] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted coding patch value. */
static int UmiAiCodingPatchConstructionCases(void)
{
    UmiAiCodingPatch *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_ai_coding_patch_init(value, "patch.48", "request.48", "Refactor A", "Remove repetition") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->patch_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_coding_patch_init(value, oversized, "request.48", "Refactor A", "Remove repetition") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_coding_patch_init(value, value->patch_id, "request.48", "Refactor A", "Remove repetition") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->patch_id, "patch.48") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->request_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_coding_patch_init(value, "patch.48", oversized, "Refactor A", "Remove repetition") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_coding_patch_init(value, "patch.48", value->request_id, "Refactor A", "Remove repetition") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->request_id, "request.48") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->title) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_coding_patch_init(value, "patch.48", "request.48", oversized, "Remove repetition") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_coding_patch_init(value, "patch.48", "request.48", value->title, "Remove repetition") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->title, "Refactor A") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->rationale) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_coding_patch_init(value, "patch.48", "request.48", "Refactor A", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_coding_patch_init(value, "patch.48", "request.48", "Refactor A", value->rationale) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->rationale, "Remove repetition") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiAiCodingPatchConstructionCases() != 0) return 1;

    MemoryFileSystem fs = {
        .path = {"src/a.c", "src/b.c", "tests/test_a.c"},
        .text = {"old a\n", "old b\n", ""},
        .present = {1, 1, 0}
    };
    UmiAiCodingFileAdapter adapter = {
        .structure_size = sizeof(UmiAiCodingFileAdapter),
        .abi_version = UMI_AI_CODING_ABI_VERSION,
        .read = read_file,
        .write = write_file,
        .remove = remove_file,
        .user_data = &fs
    };
    UmiAiCodingPatchPolicy policy = umi_ai_coding_patch_policy_default();
    UmiAiCodingPatch patch;
    UmiAiCodingPatch rollback_patch;
    UmiAiCodingPatch conflict_patch;

    assert(umi_ai_coding_patch_init(
        &patch, "patch.48", "request.48", "Refactor A", "Remove repetition")
        == UMI_STATUS_OK);
    assert(umi_ai_coding_patch_add_file(
        &patch, "src/a.c", UMI_AI_CODING_PATCH_MODIFY,
        "old a\n", "new a\n") == UMI_STATUS_OK);
    assert(umi_ai_coding_patch_add_file(
        &patch, "tests/test_a.c", UMI_AI_CODING_PATCH_CREATE,
        "", "test a\n") == UMI_STATUS_OK);
    assert(umi_ai_coding_patch_approve(&patch, "developer") == UMI_STATUS_OK);
    assert(umi_ai_coding_patch_apply(&patch, &policy, &adapter) == UMI_STATUS_OK);
    assert(strcmp(fs.text[0], "new a\n") == 0);
    assert(strcmp(fs.text[2], "test a\n") == 0);
    assert(umi_ai_coding_patch_revert(&patch, &adapter) == UMI_STATUS_OK);
    assert(strcmp(fs.text[0], "old a\n") == 0);
    assert(!fs.present[2]);

    assert(umi_ai_coding_patch_init(
        &rollback_patch, "patch.rollback", "request.48", "Two-file change",
        "Prove partial writes are restored") == UMI_STATUS_OK);
    assert(umi_ai_coding_patch_add_file(
        &rollback_patch, "src/a.c", UMI_AI_CODING_PATCH_MODIFY,
        "old a\n", "newer a\n") == UMI_STATUS_OK);
    assert(umi_ai_coding_patch_add_file(
        &rollback_patch, "src/b.c", UMI_AI_CODING_PATCH_MODIFY,
        "old b\n", "newer b\n") == UMI_STATUS_OK);
    assert(umi_ai_coding_patch_approve(&rollback_patch, "developer")
           == UMI_STATUS_OK);
    fs.fail_write_at = fs.writes + 2;
    assert(umi_ai_coding_patch_apply(&rollback_patch, &policy, &adapter)
           == UMI_STATUS_IO_ERROR);
    assert(strcmp(fs.text[0], "old a\n") == 0);
    assert(strcmp(fs.text[1], "old b\n") == 0);
    assert(rollback_patch.state == UMI_AI_CODING_PATCH_APPROVED);
    fs.fail_write_at = 0;

    assert(umi_ai_coding_patch_init(
        &conflict_patch, "patch.conflict", "request.48", "Conflict check",
        "Reject a file changed after review") == UMI_STATUS_OK);
    assert(umi_ai_coding_patch_add_file(
        &conflict_patch, "src/a.c", UMI_AI_CODING_PATCH_MODIFY,
        "old a\n", "reviewed a\n") == UMI_STATUS_OK);
    assert(umi_ai_coding_patch_approve(&conflict_patch, "developer")
           == UMI_STATUS_OK);
    (void)strcpy(fs.text[0], "changed outside review\n");
    assert(umi_ai_coding_patch_apply(&conflict_patch, &policy, &adapter)
           == UMI_STATUS_INVALID_STATE);
    assert(strcmp(fs.text[0], "changed outside review\n") == 0);
    return 0;
}
