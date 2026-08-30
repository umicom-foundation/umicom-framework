/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/workspace_test_support.h
 *
 * PURPOSE:
 *   Provide a deterministic in-memory coding workspace for runtime tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_WORKSPACE_TEST_SUPPORT_H
#define UMICOM_AI_CODING_RUNTIME_WORKSPACE_TEST_SUPPORT_H

#include "umicom/ai_coding_runtime/workspace.h"

#define TEST_WORKSPACE_FILE_CAPACITY 16U

typedef struct TestWorkspaceFile {
    char path[UMI_AI_TEXT_CAPACITY];
    char text[UMI_AI_CODING_PATCH_CONTENT_CAPACITY];
    size_t length;
    int exists;
} TestWorkspaceFile;

typedef struct TestWorkspace {
    TestWorkspaceFile files[TEST_WORKSPACE_FILE_CAPACITY];
    size_t count;
} TestWorkspace;

void test_workspace_init(TestWorkspace *workspace);
UmiStatus test_workspace_add(
    TestWorkspace *workspace,
    const char *path,
    const char *text);
UmiStatus test_workspace_adapter(
    TestWorkspace *workspace,
    UmiAiCodingWorkspaceAdapter *out_adapter);

#endif
