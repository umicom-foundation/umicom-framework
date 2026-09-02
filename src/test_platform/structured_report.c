/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/structured_report.c
 *
 * PURPOSE:
 *   Implement bounded dependency-free JSON output for the test platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/structured_report.h"

#include <stdio.h>
#include <string.h>

/* Provide the escape json operation used by this module and its client applications. */
static UmiStatus escape_json(const char *text,
                             char *out_text,
                             size_t capacity)
{
    const unsigned char *cursor;
    size_t used = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (cursor = (const unsigned char *)text; *cursor != '\0'; ++cursor) {
        const char *replacement = NULL;
        /* Select the behaviour associated with the requested command or state value. */
        switch (*cursor) {
            case '"': replacement = "\\\""; break;
            case '\\': replacement = "\\\\"; break;
            case '\n': replacement = "\\n"; break;
            case '\r': replacement = "\\r"; break;
            case '\t': replacement = "\\t"; break;
            default: break;
        }
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (replacement != NULL) {
            size_t length = strlen(replacement);
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (used + length + 1U > capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            (void)memcpy(out_text + used, replacement, length);
            used += length;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (used + 2U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
            out_text[used++] = (char)*cursor;
        }
    }
    out_text[used] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Provide the test platform report result json operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_platform_report_result_json(
    const UmiTestPlatformResultSnapshot *result,
    char *out_json,
    size_t capacity)
{
    char id[256];
    char session[256];
    char item[256];
    char message[2048];
    char details[4096];
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == NULL || out_json == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (escape_json(result->id, id, sizeof(id)) != UMI_STATUS_OK ||
        escape_json(result->session_id, session, sizeof(session)) !=
            UMI_STATUS_OK ||
        escape_json(result->item_id, item, sizeof(item)) != UMI_STATUS_OK ||
        escape_json(result->message, message, sizeof(message)) !=
            UMI_STATUS_OK ||
        escape_json(result->failure_details, details, sizeof(details)) !=
            UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    written = snprintf(
        out_json, capacity,
        "{\"type\":\"test-result\",\"id\":\"%s\","
        "\"sessionId\":\"%s\",\"itemId\":\"%s\","
        "\"outcome\":\"%s\",\"durationMs\":%.3f,"
        "\"exitCode\":%d,\"flaky\":%s,\"sequence\":%llu,"
        "\"message\":\"%s\",\"failureDetails\":\"%s\"}",
        id, session, item,
        umi_test_platform_outcome_text((UmiTestPlatformOutcome)result->outcome),
        result->duration_ms, result->exit_code,
        result->flaky ? "true" : "false",
        (unsigned long long)result->sequence, message, details);
    return written < 0 || (size_t)written >= capacity
               ? UMI_STATUS_CAPACITY_EXCEEDED
               : UMI_STATUS_OK;
}

/*
 * Provide the test platform report session json operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_platform_report_session_json(
    const UmiTestPlatformRunSessionSnapshot *session,
    char *out_json,
    size_t capacity)
{
    char id[256];
    char profile[256];
    char suite[256];
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_json == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (escape_json(session->id, id, sizeof(id)) != UMI_STATUS_OK ||
        escape_json(session->profile_id, profile, sizeof(profile)) !=
            UMI_STATUS_OK ||
        escape_json(session->suite_id, suite, sizeof(suite)) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    written = snprintf(
        out_json, capacity,
        "{\"type\":\"test-session\",\"id\":\"%s\","
        "\"profileId\":\"%s\",\"suiteId\":\"%s\","
        "\"state\":%d,\"total\":%zu,\"passed\":%zu,"
        "\"failed\":%zu,\"skipped\":%zu,"
        "\"startedAt\":%llu,\"finishedAt\":%llu}",
        id, profile, suite, session->state, session->total, session->passed,
        session->failed, session->skipped,
        (unsigned long long)session->started_at,
        (unsigned long long)session->finished_at);
    return written < 0 || (size_t)written >= capacity
               ? UMI_STATUS_CAPACITY_EXCEEDED
               : UMI_STATUS_OK;
}
