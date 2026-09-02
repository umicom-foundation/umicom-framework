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

/**
 * Represent the test workspace file data shared with callers of this public contract.
 */
typedef struct TestWorkspaceFile {
    char path[UMI_AI_TEXT_CAPACITY];
    char text[UMI_AI_CODING_PATCH_CONTENT_CAPACITY];
    size_t length;
    int exists;
} TestWorkspaceFile;

/**
 * Represent the test workspace data shared with callers of this public contract.
 */
typedef struct TestWorkspace {
    TestWorkspaceFile files[TEST_WORKSPACE_FILE_CAPACITY];
    size_t count;
} TestWorkspace;

/**
 * Initialise test workspace from caller-provided values so later operations receive a
 * known state.
 */
void test_workspace_init(TestWorkspace *workspace);
/**
 * Add test workspace only after its inputs and available capacity have been checked.
 */
UmiStatus test_workspace_add(
    TestWorkspace *workspace,
    const char *path,
    const char *text);
/**
 * Exercise test workspace adapter and return a clear result when the behaviour no longer
 * matches its contract.
 */
UmiStatus test_workspace_adapter(
    TestWorkspace *workspace,
    UmiAiCodingWorkspaceAdapter *out_adapter);

#endif
