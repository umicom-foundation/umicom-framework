/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/workspace.h
 *
 * PURPOSE:
 *   Define a narrow, reusable workspace boundary for repository-aware AI tasks.
 *   The adapter can represent local files, a remote development host, an
 *   in-memory test workspace or another controlled repository implementation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_WORKSPACE_H
#define UMICOM_AI_CODING_RUNTIME_WORKSPACE_H

#include "umicom/ai_coding_runtime/path.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiAiCodingWorkspaceRead)(
    void *user_data,
    const char *relative_path,
    char *out_text,
    size_t capacity,
    size_t *out_length);

typedef UmiStatus (*UmiAiCodingWorkspaceWrite)(
    void *user_data,
    const char *relative_path,
    const char *text,
    size_t length);

typedef UmiStatus (*UmiAiCodingWorkspaceRemove)(
    void *user_data,
    const char *relative_path);

typedef UmiStatus (*UmiAiCodingWorkspaceExists)(
    void *user_data,
    const char *relative_path,
    int *out_exists);

typedef struct UmiAiCodingWorkspaceAdapter {
    uint32_t structure_size;
    uint32_t api_version;
    UmiAiCodingWorkspaceRead read;
    UmiAiCodingWorkspaceWrite write;
    UmiAiCodingWorkspaceRemove remove;
    UmiAiCodingWorkspaceExists exists;
    void *user_data;
} UmiAiCodingWorkspaceAdapter;

UmiStatus umi_ai_coding_workspace_adapter_validate(
    const UmiAiCodingWorkspaceAdapter *adapter);

UmiStatus umi_ai_coding_workspace_file_adapter(
    const UmiAiCodingWorkspaceAdapter *workspace,
    UmiAiCodingFileAdapter *out_adapter);

#ifdef __cplusplus
}
#endif
#endif
