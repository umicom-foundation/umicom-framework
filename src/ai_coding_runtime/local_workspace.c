/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/local_workspace.c
 *
 * PURPOSE:
 *   Implement root-confined local file read/write/remove/exists operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/local_workspace.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiAiCodingLocalWorkspace {
    char root[UMI_AI_CODING_RUNTIME_PATH_CAPACITY];
};

/* Provide the full path operation used by this module and its client applications. */
static UmiStatus full_path(
    UmiAiCodingLocalWorkspace *workspace,
    const char *relative_path,
    char *out_path,
    size_t capacity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    return umi_ai_coding_runtime_path_join(
        workspace->root,
        relative_path,
        out_path,
        capacity);
}

/* Read local into validated module state and return a status when input cannot be used. */
static UmiStatus local_read(
    void *user_data,
    const char *relative_path,
    char *out_text,
    size_t capacity,
    size_t *out_length)
{
    UmiAiCodingLocalWorkspace *workspace =
        (UmiAiCodingLocalWorkspace *)user_data;
    char path[UMI_AI_CODING_RUNTIME_PATH_CAPACITY * 2U];
    FILE *stream;
    size_t read_count;
    int extra;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_text == NULL || capacity == 0U || out_length == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_length = 0U;
    out_text[0] = '\0';

    status = full_path(workspace, relative_path, path, sizeof(path));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    stream = fopen(path, "rb");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stream == NULL) {
        return errno == ENOENT ? UMI_STATUS_NOT_FOUND : UMI_STATUS_IO_ERROR;
    }

    read_count = fread(out_text, 1U, capacity - 1U, stream);
    extra = fgetc(stream);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (ferror(stream)) {
        (void)fclose(stream);
        return UMI_STATUS_IO_ERROR;
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (fclose(stream) != 0) return UMI_STATUS_IO_ERROR;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (extra != EOF) return UMI_STATUS_CAPACITY_EXCEEDED;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (memchr(out_text, '\0', read_count) != NULL) {
        return UMI_STATUS_PARSE_ERROR;
    }

    out_text[read_count] = '\0';
    *out_length = read_count;
    return UMI_STATUS_OK;
}

/*
 * Write local in its stable representation and report capacity or input failures to the
 * caller.
 */
static UmiStatus local_write(
    void *user_data,
    const char *relative_path,
    const char *text,
    size_t length)
{
    UmiAiCodingLocalWorkspace *workspace =
        (UmiAiCodingLocalWorkspace *)user_data;
    char path[UMI_AI_CODING_RUNTIME_PATH_CAPACITY * 2U];
    FILE *stream;
    size_t written;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL && length > 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = full_path(workspace, relative_path, path, sizeof(path));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    stream = fopen(path, "wb");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stream == NULL) return UMI_STATUS_IO_ERROR;

    written = length > 0U ? fwrite(text, 1U, length, stream) : 0U;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written != length || fflush(stream) != 0) {
        (void)fclose(stream);
        return UMI_STATUS_IO_ERROR;
    }

    return fclose(stream) == 0 ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
}

/* Remove local while keeping the remaining records in a valid and discoverable state. */
static UmiStatus local_remove(
    void *user_data,
    const char *relative_path)
{
    UmiAiCodingLocalWorkspace *workspace =
        (UmiAiCodingLocalWorkspace *)user_data;
    char path[UMI_AI_CODING_RUNTIME_PATH_CAPACITY * 2U];
    UmiStatus status;

    status = full_path(workspace, relative_path, path, sizeof(path));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (remove(path) == 0) return UMI_STATUS_OK;
    return errno == ENOENT ? UMI_STATUS_NOT_FOUND : UMI_STATUS_IO_ERROR;
}

/* Provide the local exists operation used by this module and its client applications. */
static UmiStatus local_exists(
    void *user_data,
    const char *relative_path,
    int *out_exists)
{
    UmiAiCodingLocalWorkspace *workspace =
        (UmiAiCodingLocalWorkspace *)user_data;
    char path[UMI_AI_CODING_RUNTIME_PATH_CAPACITY * 2U];
    FILE *stream;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_exists == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_exists = 0;

    status = full_path(workspace, relative_path, path, sizeof(path));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    stream = fopen(path, "rb");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stream == NULL) {
        return errno == ENOENT ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
    }

    *out_exists = 1;
    return fclose(stream) == 0 ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
}

/*
 * Initialise ai coding local workspace from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_coding_local_workspace_create(
    const char *root,
    UmiAiCodingLocalWorkspace **out_workspace)
{
    UmiAiCodingLocalWorkspace *workspace;
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (root == NULL || root[0] == '\0' || out_workspace == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(root);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= UMI_AI_CODING_RUNTIME_PATH_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    *out_workspace = NULL;

    workspace = (UmiAiCodingLocalWorkspace *)calloc(
        1U, sizeof(*workspace));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    (void)memcpy(workspace->root, root, length + 1U);
    *out_workspace = workspace;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ai coding local workspace so the same storage can be
 * reused safely.
 */
void umi_ai_coding_local_workspace_destroy(
    UmiAiCodingLocalWorkspace *workspace)
{
    free(workspace);
}

/*
 * Provide the ai coding local workspace adapter operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_local_workspace_adapter(
    UmiAiCodingLocalWorkspace *workspace,
    UmiAiCodingWorkspaceAdapter *out_adapter)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || out_adapter == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_adapter, 0, sizeof(*out_adapter));
    out_adapter->structure_size = (uint32_t)sizeof(*out_adapter);
    out_adapter->api_version = UMI_AI_CODING_RUNTIME_API_VERSION;
    out_adapter->read = local_read;
    out_adapter->write = local_write;
    out_adapter->remove = local_remove;
    out_adapter->exists = local_exists;
    out_adapter->user_data = workspace;
    return UMI_STATUS_OK;
}

/*
 * Provide the ai coding local workspace root operation used by this module and its client
 * applications.
 */
const char *umi_ai_coding_local_workspace_root(
    const UmiAiCodingLocalWorkspace *workspace)
{
    return workspace != NULL ? workspace->root : NULL;
}
