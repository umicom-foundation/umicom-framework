/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/workspace_test_support.c
 *
 * PURPOSE:
 *   Implement a deterministic in-memory coding workspace.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "workspace_test_support.h"

#include <stdio.h>
#include <string.h>

static size_t find_file(TestWorkspace *workspace, const char *path)
{
    size_t index;

    for (index = 0U; index < workspace->count; ++index) {
        if (strcmp(workspace->files[index].path, path) == 0) {
            return index;
        }
    }

    return workspace->count;
}

void test_workspace_init(TestWorkspace *workspace)
{
    if (workspace != NULL) {
        (void)memset(workspace, 0, sizeof(*workspace));
    }
}

UmiStatus test_workspace_add(
    TestWorkspace *workspace,
    const char *path,
    const char *text)
{
    TestWorkspaceFile *file;
    size_t path_length;
    size_t text_length;

    if (workspace == NULL || path == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (workspace->count >= TEST_WORKSPACE_FILE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    path_length = strlen(path);
    text_length = strlen(text);

    if (path_length >= sizeof(workspace->files[0].path) ||
        text_length >= sizeof(workspace->files[0].text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    file = &workspace->files[workspace->count++];
    (void)memset(file, 0, sizeof(*file));
    (void)memcpy(file->path, path, path_length + 1U);
    (void)memcpy(file->text, text, text_length + 1U);
    file->length = text_length;
    file->exists = 1;
    return UMI_STATUS_OK;
}

static UmiStatus read_file(
    void *user_data,
    const char *relative_path,
    char *out_text,
    size_t capacity,
    size_t *out_length)
{
    TestWorkspace *workspace = (TestWorkspace *)user_data;
    size_t index;

    if (workspace == NULL || relative_path == NULL ||
        out_text == NULL || capacity == 0U || out_length == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_file(workspace, relative_path);
    if (index == workspace->count ||
        !workspace->files[index].exists) {
        return UMI_STATUS_NOT_FOUND;
    }

    if (workspace->files[index].length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(
        out_text,
        workspace->files[index].text,
        workspace->files[index].length + 1U);
    *out_length = workspace->files[index].length;
    return UMI_STATUS_OK;
}

static UmiStatus write_file(
    void *user_data,
    const char *relative_path,
    const char *text,
    size_t length)
{
    TestWorkspace *workspace = (TestWorkspace *)user_data;
    size_t index;

    if (workspace == NULL || relative_path == NULL ||
        (text == NULL && length > 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (length >= sizeof(workspace->files[0].text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    index = find_file(workspace, relative_path);

    if (index == workspace->count) {
        UmiStatus status;

        if (length >= UMI_AI_CODING_PATCH_CONTENT_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        {
            char temporary[UMI_AI_CODING_PATCH_CONTENT_CAPACITY];

            if (length > 0U) (void)memcpy(temporary, text, length);
            temporary[length] = '\0';

            status = test_workspace_add(
                workspace,
                relative_path,
                temporary);
        }

        return status;
    }

    if (length > 0U) {
        (void)memcpy(workspace->files[index].text, text, length);
    }
    workspace->files[index].text[length] = '\0';
    workspace->files[index].length = length;
    workspace->files[index].exists = 1;
    return UMI_STATUS_OK;
}

static UmiStatus remove_file(void *user_data, const char *relative_path)
{
    TestWorkspace *workspace = (TestWorkspace *)user_data;
    size_t index;

    if (workspace == NULL || relative_path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_file(workspace, relative_path);
    if (index == workspace->count ||
        !workspace->files[index].exists) {
        return UMI_STATUS_NOT_FOUND;
    }

    workspace->files[index].exists = 0;
    return UMI_STATUS_OK;
}

static UmiStatus exists_file(
    void *user_data,
    const char *relative_path,
    int *out_exists)
{
    TestWorkspace *workspace = (TestWorkspace *)user_data;
    size_t index;

    if (workspace == NULL || relative_path == NULL ||
        out_exists == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_file(workspace, relative_path);
    *out_exists =
        index < workspace->count &&
        workspace->files[index].exists;
    return UMI_STATUS_OK;
}

UmiStatus test_workspace_adapter(
    TestWorkspace *workspace,
    UmiAiCodingWorkspaceAdapter *out_adapter)
{
    if (workspace == NULL || out_adapter == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_adapter, 0, sizeof(*out_adapter));
    out_adapter->structure_size = (uint32_t)sizeof(*out_adapter);
    out_adapter->api_version = UMI_AI_CODING_RUNTIME_API_VERSION;
    out_adapter->read = read_file;
    out_adapter->write = write_file;
    out_adapter->remove = remove_file;
    out_adapter->exists = exists_file;
    out_adapter->user_data = workspace;
    return UMI_STATUS_OK;
}
