/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_local_files.c
 * PURPOSE: Exercise actual local coding adapters, root capture and complete text publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/ai_coding_runtime/local_workspace.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/rooted_files.h"

static void ChangeDirectory(const char *path)
{
#ifdef _WIN32
    wchar_t native[UMI_PATH_CAPACITY];
    CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, native, (int)UMI_PATH_CAPACITY) > 0);
    CHECK(SetCurrentDirectoryW(native));
#else
    CHECK(chdir(path) == 0);
#endif
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    char root[UMI_PATH_CAPACITY], source[UMI_PATH_CAPACITY], current[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(source, root, "src");
    CHECK(umi_fs_make_directories(source) == UMI_STATUS_OK);
    CHECK(umi_fs_current_directory(current, sizeof(current)) == UMI_STATUS_OK);
    if (strcmp(mode, "capture") == 0)
        ChangeDirectory(root);
    UmiAiCodingLocalWorkspace *workspace = NULL;
    CHECK(umi_ai_coding_local_workspace_create(strcmp(mode, "capture") == 0 ? "." : root, &workspace) ==
          UMI_STATUS_OK);
    UmiAiCodingWorkspaceAdapter adapter;
    CHECK(umi_ai_coding_local_workspace_adapter(workspace, &adapter) == UMI_STATUS_OK);
    CHECK(umi_ai_coding_workspace_adapter_validate(&adapter) == UMI_STATUS_OK);
    CHECK(umi_path_is_absolute(umi_ai_coding_local_workspace_root(workspace)));
    if (strcmp(mode, "capture") == 0)
        ChangeDirectory(current);
    char output[128] = "previous";
    size_t length = 44U;
    static const char program[] = "int main(void) { return 0; }\n";
    const char *leaf = strcmp(mode, "unicode") == 0 ? "src/caf\xc3\xa9-\xe6\x96\x87.c" : "src/main.c";
    if (strcmp(mode, "roundtrip") == 0 || strcmp(mode, "unicode") == 0 || strcmp(mode, "capture") == 0)
    {
        CHECK(adapter.write(adapter.user_data, leaf, program, sizeof(program) - 1U) == UMI_STATUS_OK);
        CHECK(adapter.read(adapter.user_data, leaf, output, sizeof(output), &length) == UMI_STATUS_OK);
        CHECK(length == sizeof(program) - 1U && strcmp(output, "int main(void) { return 0; }\n") == 0);
        char file[UMI_PATH_CAPACITY];
        FixturePath(file, root, leaf);
        size_t actualSize;
        unsigned char *actual = FixtureRead(file, &actualSize);
        CHECK(actualSize == length && memcmp(actual, output, length) == 0);
        free(actual);
    }
    else if (strcmp(mode, "binary") == 0)
    {
        const char value[] = {'a', '\0', 'b'};
        CHECK(UmiRootedFileWrite(root, leaf, value, sizeof(value)) == UMI_STATUS_OK);
        CHECK(adapter.read(adapter.user_data, leaf, output, sizeof(output), &length) ==
              UMI_STATUS_PARSE_ERROR);
        CHECK(output[0] == '\0' && length == 0U);
    }
    else if (strcmp(mode, "capacity") == 0)
    {
        CHECK(adapter.write(adapter.user_data, leaf, "abcdef", 6U) == UMI_STATUS_OK);
        CHECK(adapter.read(adapter.user_data, leaf, output, 6U, &length) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(output[0] == '\0' && length == 0U);
    }
    else if (strcmp(mode, "empty") == 0)
    {
        CHECK(adapter.write(adapter.user_data, leaf, NULL, 0U) == UMI_STATUS_OK);
        CHECK(adapter.read(adapter.user_data, leaf, output, 1U, &length) == UMI_STATUS_OK);
        CHECK(output[0] == '\0' && length == 0U);
    }
    else if (strcmp(mode, "missing") == 0)
    {
        int exists = 1;
        CHECK(adapter.exists(adapter.user_data, leaf, &exists) == UMI_STATUS_OK && !exists);
        CHECK(adapter.read(adapter.user_data, leaf, output, sizeof(output), &length) == UMI_STATUS_NOT_FOUND);
        CHECK(output[0] == '\0' && length == 0U);
        CHECK(adapter.remove(adapter.user_data, leaf) == UMI_STATUS_NOT_FOUND);
        CHECK(adapter.write(adapter.user_data, "absent/main.c", "x", 1U) == UMI_STATUS_NOT_FOUND);
    }
    else if (strcmp(mode, "remove") == 0)
    {
        CHECK(adapter.write(adapter.user_data, leaf, "x", 1U) == UMI_STATUS_OK);
        CHECK(adapter.remove(adapter.user_data, leaf) == UMI_STATUS_OK);
        int exists = 1;
        CHECK(adapter.exists(adapter.user_data, leaf, &exists) == UMI_STATUS_OK && !exists);
    }
    else if (strcmp(mode, "invalid") == 0)
    {
        CHECK(adapter.write(adapter.user_data, "../outside.c", "x", 1U) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(adapter.write(adapter.user_data, ".git/config", "x", 1U) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(adapter.write(adapter.user_data, "NUL.txt", "x", 1U) != UMI_STATUS_OK);
        CHECK(UmiAiCodingWorkspaceReadFile(NULL, leaf, output, sizeof(output), &length) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(output[0] == '\0' && length == 0U);
    }
    else if (strcmp(mode, "resolve") == 0)
    {
        strcpy(output, "unchanged");
        CHECK(UmiAiCodingWorkspaceRootResolve(root, output, 2U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(output, "unchanged") == 0);
        CHECK(UmiAiCodingWorkspaceRootResolve("", output, sizeof(output)) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(output, "unchanged") == 0);
#ifdef _WIN32
        CHECK(UmiAiCodingWorkspaceRootResolve("C:relative", output, sizeof(output)) ==
              UMI_STATUS_INVALID_ARGUMENT);
#else
        CHECK(UmiAiCodingWorkspaceRootResolve("a\\b", output, sizeof(output)) == UMI_STATUS_INVALID_ARGUMENT);
#endif
    }
    else
    {
        umi_ai_coding_local_workspace_destroy(workspace);
        return 2;
    }
    umi_ai_coding_local_workspace_destroy(workspace);
    return 0;
}
