/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_search_pattern.c
 *
 * PURPOSE:
 *   Implement the test workspace search pattern behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_search_pattern.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/workspace_search_pattern.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorWorkspaceSearchPattern *pattern = NULL;
    UmiEditorWorkspaceSearchPatternRequest request;
    UmiEditorWorkspaceSearchPatternDiagnostic diagnostic;
    UmiEditorWorkspaceSearchPatternMatch match;
    const char *assignment = "answer = 42;";

    assert(umi_editor_workspace_search_pattern_create(&pattern) ==
           UMI_STATUS_OK);

    umi_editor_workspace_search_pattern_request_init(&request, "alpha");
    assert(umi_editor_workspace_search_pattern_compile(
               pattern, &request, &diagnostic) == UMI_STATUS_OK);
    assert(umi_editor_workspace_search_pattern_find(
               pattern, "xx Alpha yy", strlen("xx Alpha yy"), 0U, &match) ==
           UMI_STATUS_OK);
    assert(match.start_byte_offset == 3U);
    assert(match.end_byte_offset == 8U);

    umi_editor_workspace_search_pattern_request_init(
        &request, "([A-Za-z_][A-Za-z0-9_]*)\\s*=\\s*(\\d+)");
    request.mode = UMI_EDITOR_WORKSPACE_SEARCH_PATTERN_REGEX;
    assert(umi_editor_workspace_search_pattern_compile(
               pattern, &request, &diagnostic) == UMI_STATUS_OK);
    assert(umi_editor_workspace_search_pattern_find(
               pattern, assignment, strlen(assignment), 0U, &match) ==
           UMI_STATUS_OK);
    assert(match.start_byte_offset == 0U);
    assert(match.end_byte_offset == 11U);
    assert(match.capture_count == 3U);
    assert(match.captures[1].matched);
    assert(match.captures[1].start_byte_offset == 0U);
    assert(match.captures[1].end_byte_offset == 6U);
    assert(match.captures[2].matched);
    assert(match.captures[2].start_byte_offset == 9U);
    assert(match.captures[2].end_byte_offset == 11U);

    /* Ordered NFA priority preserves alternation and greedy/lazy intent. */
    umi_editor_workspace_search_pattern_request_init(&request, "a+");
    request.mode = UMI_EDITOR_WORKSPACE_SEARCH_PATTERN_REGEX;
    assert(umi_editor_workspace_search_pattern_compile(
               pattern, &request, &diagnostic) == UMI_STATUS_OK);
    assert(umi_editor_workspace_search_pattern_find(
               pattern, "aaa", strlen("aaa"), 0U, &match) ==
           UMI_STATUS_OK);
    assert(match.start_byte_offset == 0U);
    assert(match.end_byte_offset == 3U);

    umi_editor_workspace_search_pattern_request_init(&request, "a+?");
    request.mode = UMI_EDITOR_WORKSPACE_SEARCH_PATTERN_REGEX;
    assert(umi_editor_workspace_search_pattern_compile(
               pattern, &request, &diagnostic) == UMI_STATUS_OK);
    assert(umi_editor_workspace_search_pattern_find(
               pattern, "aaa", strlen("aaa"), 0U, &match) ==
           UMI_STATUS_OK);
    assert(match.start_byte_offset == 0U);
    assert(match.end_byte_offset == 1U);

    umi_editor_workspace_search_pattern_request_init(&request, "a|ab");
    request.mode = UMI_EDITOR_WORKSPACE_SEARCH_PATTERN_REGEX;
    assert(umi_editor_workspace_search_pattern_compile(
               pattern, &request, &diagnostic) == UMI_STATUS_OK);
    assert(umi_editor_workspace_search_pattern_find(
               pattern, "ab", strlen("ab"), 0U, &match) ==
           UMI_STATUS_OK);
    assert(match.end_byte_offset == 1U);

    umi_editor_workspace_search_pattern_request_init(&request, "ab|a");
    request.mode = UMI_EDITOR_WORKSPACE_SEARCH_PATTERN_REGEX;
    assert(umi_editor_workspace_search_pattern_compile(
               pattern, &request, &diagnostic) == UMI_STATUS_OK);
    assert(umi_editor_workspace_search_pattern_find(
               pattern, "ab", strlen("ab"), 0U, &match) ==
           UMI_STATUS_OK);
    assert(match.end_byte_offset == 2U);

    umi_editor_workspace_search_pattern_request_init(&request, "cat");
    request.whole_word = 1;
    assert(umi_editor_workspace_search_pattern_compile(
               pattern, &request, &diagnostic) == UMI_STATUS_OK);
    assert(umi_editor_workspace_search_pattern_find(
               pattern, "concatenate cat", strlen("concatenate cat"), 0U,
               &match) == UMI_STATUS_OK);
    assert(match.start_byte_offset == 12U);

    umi_editor_workspace_search_pattern_request_init(&request, "(");
    request.mode = UMI_EDITOR_WORKSPACE_SEARCH_PATTERN_REGEX;
    assert(umi_editor_workspace_search_pattern_compile(
               pattern, &request, &diagnostic) == UMI_STATUS_PARSE_ERROR);
    assert(diagnostic.message[0] != '\0');

    umi_editor_workspace_search_pattern_destroy(pattern);
    return 0;
}
