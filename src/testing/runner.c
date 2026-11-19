/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/runner.c
 *
 * PURPOSE:
 *   Execute test processes, capture output and aggregate suite results.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/runner.h"

#include <stdio.h>
#include <string.h>

#include "umicom/platform/process.h"

/* The former splitter could return a valid prefix when the full command did
 * not fit or a quote was incomplete. The checked splitter below keeps this
 * runner's text format and admits the whole command before launch. Retain the
 * former behavior here for review. */
#if 0
/* Provide the split arguments operation used by this module and its client applications. */
static size_t split_arguments(char *text,
                              const char **arguments,
                              size_t capacity)
{
    size_t count = 0U;
    char *cursor = text;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (cursor != NULL && *cursor != '\0' && count < capacity) {
        char *start;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (*cursor == ' ' || *cursor == '\t') {
            cursor += 1;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == '\0') {
            break;
        }
        start = cursor;
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == '"') {
            start = ++cursor;
            /*
             * Continue only while work remains available; the loop body advances the state on each
             * pass.
             */
            while (*cursor != '\0' && *cursor != '"') {
                cursor += 1;
            }
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            /*
             * Continue only while work remains available; the loop body advances the state on each
             * pass.
             */
            while (*cursor != '\0' && *cursor != ' ' && *cursor != '\t') {
                cursor += 1;
            }
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor != '\0') {
            *cursor++ = '\0';
        }
        arguments[count++] = start;
    }
    return count;
}
#endif

/* Keep test argument text compatible with existing records. Quotes at the
 * start of an argument group its bytes, and backslashes remain literal. The
 * higher-level developer task parser has a different grammar and capacity. */
static UmiStatus split_arguments_checked(char *text, const char **arguments,
                                         size_t capacity, size_t *out_count)
{
    size_t count = 0U;
    char *cursor = text;
    for (;;) {
        cursor += strspn(cursor, " \t");
        if (*cursor == '\0') break;
        if (count == capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

        char *start = cursor;
        if (*cursor == '"') {
            start = cursor + 1;
            cursor = strchr(start, '"');
            if (cursor == NULL) return UMI_STATUS_PARSE_ERROR;
        } else {
            cursor += strcspn(cursor, " \t");
        }
        if (*cursor != '\0') *cursor++ = '\0';
        arguments[count++] = start;
    }
    *out_count = count;
    return UMI_STATUS_OK;
}

/*
 * Provide the test runner run case operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runner_run_case(const UmiTestCase *test_case,
                                   UmiCancellationToken *cancellation,
                                   UmiTestResult *out_result)
{
    UmiProcessRequest request;
    UmiProcessResult process_result;
    const char *arguments[UMI_PROCESS_MAX_ARGUMENTS];
    char argument_text[UMI_TEST_COMMAND_CAPACITY];
    size_t argument_count;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (test_case == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Public case records own fixed-width text. Bound identity reads before
     * constructing a result, including when a disabled case is skipped. */
    if (memchr(test_case->test_id, '\0', sizeof(test_case->test_id)) == NULL ||
        memchr(test_case->name, '\0', sizeof(test_case->name)) == NULL) {
        umi_test_result_init(out_result, NULL, NULL);
        umi_test_result_finish(out_result, UMI_STATUS_INVALID_ARGUMENT, -1, 0U);
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_test_result_init(out_result, test_case->test_id, test_case->name);
    /* Apply this operation only while the related capability or state is available. */
    if (!test_case->enabled) {
        out_result->state = UMI_TEST_STATE_SKIPPED;
        return UMI_STATUS_OK;
    }
    /* This former formatted copy could hide an unterminated fixed field, and
     * the old splitter silently discarded trailing arguments. Retain it here
     * for review. The command below is admitted in full before process entry. */
#if 0
    (void)snprintf(argument_text,
                   sizeof(argument_text),
                   "%s",
                   test_case->argument_text);
    argument_count = split_arguments(argument_text,
                                     arguments,
                                     UMI_PROCESS_MAX_ARGUMENTS);
#endif
    if (memchr(test_case->program, '\0', sizeof(test_case->program)) == NULL ||
        memchr(test_case->working_directory, '\0', sizeof(test_case->working_directory)) == NULL ||
        memchr(test_case->argument_text, '\0', sizeof(test_case->argument_text)) == NULL) {
        umi_test_result_finish(out_result, UMI_STATUS_INVALID_ARGUMENT, -1, 0U);
        (void)snprintf(out_result->output, sizeof(out_result->output), "Test command field is unterminated");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memcpy(argument_text, test_case->argument_text, sizeof(argument_text));
    status = split_arguments_checked(argument_text, arguments, UMI_PROCESS_MAX_ARGUMENTS, &argument_count);
    if (status != UMI_STATUS_OK) {
        umi_test_result_finish(out_result, status, -1, 0U);
        (void)snprintf(out_result->output, sizeof(out_result->output), "%s",
            status == UMI_STATUS_PARSE_ERROR ? "Test argument is missing a closing quote" :
            "Test command exceeds the process argument limit");
        return status;
    }
    (void)memset(&request, 0, sizeof(request));
    request.program = test_case->program;
    request.arguments = arguments;
    request.argument_count = argument_count;
    request.working_directory =
        test_case->working_directory[0] != '\0'
            ? test_case->working_directory
            : NULL;
    request.capture_stdout = 1;
    request.capture_stderr = 1;
    request.timeout_ms = test_case->timeout_ms;
    request.cancellation = cancellation;
    out_result->state = UMI_TEST_STATE_RUNNING;
    status = umi_process_execute(&request, &process_result);
    {
        size_t output_length = strlen(process_result.output);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (output_length >= sizeof(out_result->output)) {
            output_length = sizeof(out_result->output) - 1U;
        }
        (void)memcpy(out_result->output,
                     process_result.output,
                     output_length);
        out_result->output[output_length] = '\0';
    }
    umi_test_result_finish(out_result,
                           status,
                           process_result.exit_code,
                           process_result.duration_ms);
    return status;
}

/*
 * Provide the test runner run suite operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runner_run_suite(const UmiTestSuite *suite,
                                    UmiCancellationToken *cancellation,
                                    UmiTestResult *results,
                                    size_t result_capacity,
                                    UmiTestRunSummary *out_summary)
{
    size_t index;
    size_t count;
    UmiStatus first_failure = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (suite == NULL || results == NULL || out_summary == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_summary, 0, sizeof(*out_summary));
    count = umi_test_suite_count(suite);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > result_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        UmiStatus status = umi_test_runner_run_case(
            umi_test_suite_at(suite, index),
            cancellation,
            &results[index]
        );
        out_summary->total += 1U;
        out_summary->duration_ms += results[index].duration_ms;
        /* Select the behaviour associated with the requested command or state value. */
        switch (results[index].state) {
            case UMI_TEST_STATE_PASSED: out_summary->passed += 1U; break;
            case UMI_TEST_STATE_FAILED: out_summary->failed += 1U; break;
            case UMI_TEST_STATE_SKIPPED: out_summary->skipped += 1U; break;
            case UMI_TEST_STATE_CANCELLED:
                out_summary->cancelled += 1U;
                break;
            case UMI_TEST_STATE_TIMED_OUT:
                out_summary->timed_out += 1U;
                break;
            default: break;
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK &&
            first_failure == UMI_STATUS_OK) {
            first_failure = status;
        }
    }
    return first_failure;
}
