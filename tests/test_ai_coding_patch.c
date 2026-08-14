/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_coding_patch.c
 *
 * PURPOSE:
 *   Verify reviewed patch application, hash-conflict rejection, transactional
 *   rollback and controlled reversion through a host-owned file adapter.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/ai/coding_patch.h"

typedef struct MemoryFileSystem {
    char path[3][128];
    char text[3][1024];
    int present[3];
    int fail_write_at;
    int writes;
} MemoryFileSystem;

static int find_file(MemoryFileSystem *fs, const char *path)
{
    int index;
    for (index = 0; index < 3; ++index) {
        if (strcmp(fs->path[index], path) == 0) return index;
    }
    return -1;
}

static UmiStatus read_file(void *user_data, const char *path, char *out_text,
                           size_t capacity, size_t *out_length)
{
    MemoryFileSystem *fs = (MemoryFileSystem *)user_data;
    int index = find_file(fs, path);
    size_t length;
    if (index < 0 || !fs->present[index]) return UMI_STATUS_NOT_FOUND;
    length = strlen(fs->text[index]);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out_text, fs->text[index], length + 1U);
    *out_length = length;
    return UMI_STATUS_OK;
}

static UmiStatus write_file(void *user_data, const char *path,
                            const char *text, size_t length)
{
    MemoryFileSystem *fs = (MemoryFileSystem *)user_data;
    int index = find_file(fs, path);
    ++fs->writes;
    if (fs->fail_write_at > 0 && fs->writes == fs->fail_write_at) {
        return UMI_STATUS_IO_ERROR;
    }
    if (index < 0 || length >= sizeof(fs->text[index])) return UMI_STATUS_IO_ERROR;
    (void)memcpy(fs->text[index], text, length);
    fs->text[index][length] = '\0';
    fs->present[index] = 1;
    return UMI_STATUS_OK;
}

static UmiStatus remove_file(void *user_data, const char *path)
{
    MemoryFileSystem *fs = (MemoryFileSystem *)user_data;
    int index = find_file(fs, path);
    if (index < 0 || !fs->present[index]) return UMI_STATUS_NOT_FOUND;
    fs->present[index] = 0;
    fs->text[index][0] = '\0';
    return UMI_STATUS_OK;
}

int main(void)
{
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
