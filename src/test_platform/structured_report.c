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

/* Preserve the previous streaming export for review. The replacement validates
 * bounded text and numbers, escapes every JSON control and publishes only a
 * complete record, so export failure cannot replace previously retained evidence. */
#if 0
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
#endif

#include "../base/utf8_text_private.h"
#include <locale.h>
#include <math.h>
#include <stdlib.h>

/* Six bytes cover the longest escape for one input byte. Fixed snapshot
 * fields bound all arithmetic and allocation; text never crosses its member. */
static UmiStatus EscapeField(const char *text, size_t bound, char *out, size_t capacity)
{
    const char *end = memchr(text, '\0', bound);
    if (end == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t bytes = (size_t)(end - text), used = 0U;
    if (!UmiUtf8TextValid(text, bytes)) return UMI_STATUS_INVALID_ARGUMENT;
    static const char hex[] = "0123456789abcdef";
    for (size_t i = 0U; i < bytes; ++i) {
        unsigned char c = (unsigned char)text[i];
        const char *replacement = NULL;
        char escaped[6]; size_t count = 1U;
        if (c == '"') replacement = "\\\"";
        else if (c == '\\') replacement = "\\\\";
        else if (c == '\n') replacement = "\\n";
        else if (c == '\r') replacement = "\\r";
        else if (c == '\t') replacement = "\\t";
        if (replacement != NULL) count = 2U;
        else if (c < 0x20U) {
            escaped[0]='\\'; escaped[1]='u'; escaped[2]='0'; escaped[3]='0';
            escaped[4]=hex[c >> 4U]; escaped[5]=hex[c & 15U];
            replacement=escaped; count=sizeof(escaped);
        } else replacement=text+i;
        if (used >= capacity || count >= capacity-used) return UMI_STATUS_CAPACITY_EXCEEDED;
        memcpy(out+used,replacement,count); used+=count;
    }
    out[used]='\0'; return UMI_STATUS_OK;
}

/* Preserve the existing three decimal places without changing the process
 * locale. The owner must not call setlocale concurrently with report export. */
static UmiStatus Duration(double value, char *out, size_t capacity)
{
    if (!isfinite(value) || value < 0.0) return UMI_STATUS_INVALID_ARGUMENT;
    int count=snprintf(out,capacity,"%.3f",value);
    if (count<0 || (size_t)count>=capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    const struct lconv *locale=localeconv();
    if (locale==NULL || locale->decimal_point==NULL || locale->decimal_point[0]=='\0')
        return UMI_STATUS_INVALID_STATE;
    if (strcmp(locale->decimal_point,".")!=0) {
        char *point=strstr(out,locale->decimal_point);
        if (point==NULL) return UMI_STATUS_INVALID_STATE;
        size_t width=strlen(locale->decimal_point);
        memmove(point+1,point+width,strlen(point+width)+1U); *point='.';
    }
    return UMI_STATUS_OK;
}

static UmiStatus Publish(const char *record, int length, size_t staging_capacity,
    char *output, size_t capacity)
{
    if (length<0) return UMI_STATUS_INTERNAL_ERROR;
    size_t size=(size_t)length;
    if (size>=staging_capacity || size>=capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(output,record,size+1U); return UMI_STATUS_OK;
}

typedef struct ResultStorage {
    char id[6U*128U], session[6U*128U], item[6U*128U];
    char message[6U*1024U], details[6U*2048U], duration[512];
    char record[UMI_TEST_PLATFORM_RESULT_JSON_CAPACITY];
} ResultStorage;
UmiStatus umi_test_platform_report_result_json(const UmiTestPlatformResultSnapshot *result,
    char *out_json, size_t capacity)
{
    if (result==NULL || out_json==NULL || capacity==0U) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=umi_test_platform_result_snapshot_validate(result,NULL);
    if (status!=UMI_STATUS_OK) return status;
    if (result->outcome<UMI_TEST_PLATFORM_OUTCOME_NOT_RUN || result->outcome>UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT)
        return UMI_STATUS_INVALID_ARGUMENT;
    ResultStorage *stage=malloc(sizeof(*stage));
    if (stage==NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status=Duration(result->duration_ms,stage->duration,sizeof(stage->duration));
#define REPORT_FIELD(member,destination) \
    if (status==UMI_STATUS_OK) status=EscapeField(result->member,sizeof(result->member),stage->destination,sizeof(stage->destination))
    REPORT_FIELD(id,id); REPORT_FIELD(session_id,session); REPORT_FIELD(item_id,item);
    REPORT_FIELD(message,message); REPORT_FIELD(failure_details,details);
#undef REPORT_FIELD
    if (status==UMI_STATUS_OK) {
        int count=snprintf(stage->record,sizeof(stage->record),
            "{\"type\":\"test-result\",\"id\":\"%s\",\"sessionId\":\"%s\",\"itemId\":\"%s\","
            "\"outcome\":\"%s\",\"durationMs\":%s,\"exitCode\":%d,\"flaky\":%s,\"sequence\":%llu,"
            "\"message\":\"%s\",\"failureDetails\":\"%s\"}",
            stage->id,stage->session,stage->item,umi_test_platform_outcome_text((UmiTestPlatformOutcome)result->outcome),
            stage->duration,result->exit_code,result->flaky?"true":"false",(unsigned long long)result->sequence,
            stage->message,stage->details);
        status=Publish(stage->record,count,sizeof(stage->record),out_json,capacity);
    }
    free(stage);return status;
}

typedef struct SessionStorage {
    char id[6U*128U], profile[6U*128U], suite[6U*128U];
    char record[UMI_TEST_PLATFORM_SESSION_JSON_CAPACITY];
} SessionStorage;
UmiStatus umi_test_platform_report_session_json(const UmiTestPlatformRunSessionSnapshot *session,
    char *out_json,size_t capacity)
{
    if (session==NULL || out_json==NULL || capacity==0U) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=umi_test_platform_run_session_snapshot_validate(session,NULL);
    if (status!=UMI_STATUS_OK) return status;
    SessionStorage *stage=malloc(sizeof(*stage));
    if (stage==NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status=EscapeField(session->id,sizeof(session->id),stage->id,sizeof(stage->id));
    if (status==UMI_STATUS_OK) status=EscapeField(session->profile_id,sizeof(session->profile_id),stage->profile,sizeof(stage->profile));
    if (status==UMI_STATUS_OK) status=EscapeField(session->suite_id,sizeof(session->suite_id),stage->suite,sizeof(stage->suite));
    if (status==UMI_STATUS_OK) {
        int count=snprintf(stage->record,sizeof(stage->record),
            "{\"type\":\"test-session\",\"id\":\"%s\",\"profileId\":\"%s\",\"suiteId\":\"%s\","
            "\"state\":%d,\"total\":%zu,\"passed\":%zu,\"failed\":%zu,\"skipped\":%zu,"
            "\"startedAt\":%llu,\"finishedAt\":%llu}",
            stage->id,stage->profile,stage->suite,session->state,session->total,session->passed,session->failed,session->skipped,
            (unsigned long long)session->started_at,(unsigned long long)session->finished_at);
        status=Publish(stage->record,count,sizeof(stage->record),out_json,capacity);
    }
    free(stage);return status;
}
