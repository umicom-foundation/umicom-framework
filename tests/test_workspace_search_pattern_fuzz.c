/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_search_pattern_fuzz.c
 *
 * PURPOSE:
 *   Exercise the portable workspace-search pattern compiler and ordered NFA
 *   with deterministic malformed and valid expression combinations under the
 *   normal sanitizer-enabled Framework test configuration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdint.h>

#include "umicom/editor/workspace_search_pattern.h"

#define FUZZ_ITERATION_COUNT 20000U
#define FUZZ_PATTERN_CAPACITY 80U
#define FUZZ_TEXT_CAPACITY 160U

static uint64_t generator_state = UINT64_C(0x9e3779b97f4a7c15);

/*
 * Exercise next random and return a clear result when the behaviour no longer matches its
 * contract.
 */
static uint32_t next_random(void)
{
    generator_state ^= generator_state << 7U;
    generator_state ^= generator_state >> 9U;
    generator_state ^= generator_state << 8U;
    return (uint32_t)generator_state;
}

/*
 * Exercise compile status expected and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int compile_status_expected(UmiStatus status)
{
    return status == UMI_STATUS_OK ||
           status == UMI_STATUS_PARSE_ERROR ||
           status == UMI_STATUS_CAPACITY_EXCEEDED;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    static const char pattern_alphabet[] =
        "abc012.*+?{}[]()|^$\\dwsB:-,";
    static const char text_alphabet[] = "abc012 _-\n";
    UmiEditorWorkspaceSearchPattern *pattern = NULL;
    UmiEditorWorkspaceSearchPatternRequest request;
    UmiEditorWorkspaceSearchPatternDiagnostic diagnostic;
    UmiEditorWorkspaceSearchPatternMatch match;
    char expression[FUZZ_PATTERN_CAPACITY];
    char text[FUZZ_TEXT_CAPACITY];
    uint32_t iteration;

    assert(umi_editor_workspace_search_pattern_create(&pattern) ==
           UMI_STATUS_OK);

    /* Visit each bounded item once so every record receives the same rule. */
    for (iteration = 0U; iteration < FUZZ_ITERATION_COUNT; ++iteration) {
        size_t expression_length = 1U +
            (size_t)(next_random() % (FUZZ_PATTERN_CAPACITY - 2U));
        size_t text_length =
            (size_t)(next_random() % (FUZZ_TEXT_CAPACITY - 1U));
        size_t position;
        UmiStatus status;

        /* Visit each bounded item once so every record receives the same rule. */
        for (position = 0U; position < expression_length; ++position) {
            expression[position] = pattern_alphabet[
                next_random() % (sizeof(pattern_alphabet) - 1U)];
        }
        expression[expression_length] = '\0';

        /* Visit each bounded item once so every record receives the same rule. */
        for (position = 0U; position < text_length; ++position) {
            text[position] = text_alphabet[
                next_random() % (sizeof(text_alphabet) - 1U)];
        }
        text[text_length] = '\0';

        umi_editor_workspace_search_pattern_request_init(&request,
                                                          expression);
        request.mode = UMI_EDITOR_WORKSPACE_SEARCH_PATTERN_REGEX;
        request.maximum_program_instructions = 4096U;
        request.maximum_execution_steps = UINT64_C(200000);
        request.multiline = (int)(next_random() & 1U);
        request.dot_matches_newline = (int)(next_random() & 1U);
        request.whole_word = (int)(next_random() & 1U);

        status = umi_editor_workspace_search_pattern_compile(
            pattern, &request, &diagnostic);
        assert(compile_status_expected(status));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            size_t start = text_length == 0U
                ? 0U
                : (size_t)(next_random() % (text_length + 1U));

            status = umi_editor_workspace_search_pattern_find(
                pattern, text, text_length, start, &match);
            assert(status == UMI_STATUS_OK ||
                   status == UMI_STATUS_NOT_FOUND ||
                   status == UMI_STATUS_TIMEOUT);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK) {
                assert(match.start_byte_offset <= match.end_byte_offset);
                assert(match.end_byte_offset <= text_length);
                assert(match.start_byte_offset >= start);
                assert(match.capture_count <=
                       UMI_EDITOR_WORKSPACE_SEARCH_MAX_CAPTURES);
            }
        }
    }

    umi_editor_workspace_search_pattern_destroy(pattern);
    return 0;
}
