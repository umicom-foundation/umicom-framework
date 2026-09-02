/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/workbench/stream.c
 *
 * PURPOSE:
 *   Implement bounded WebSocket/SSE message inspection and filtering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/workbench/stream.h"

#include <ctype.h>
#include <string.h>

/*
 * Provide the contains case insensitive operation used by this module and its client
 * applications.
 */
static int contains_case_insensitive(const char *text, const char *query)
{
    size_t length;
    const char *cursor;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL || query[0] == '\0') return 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0;
    length = strlen(query);
    /* Visit each bounded item once so every record receives the same rule. */
    for (cursor = text; *cursor != '\0'; ++cursor) {
        size_t index;
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < length; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (cursor[index] == '\0' ||
                tolower((unsigned char)cursor[index]) !=
                tolower((unsigned char)query[index])) break;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index == length) return 1;
    }
    return 0;
}

/*
 * Initialise web workbench stream from caller-provided values so later operations receive
 * a known state.
 */
void umi_web_workbench_stream_init(
    UmiWebWorkbenchStreamSession *session,
    const char *session_id,
    const char *name,
    UmiWebWorkbenchStreamKind kind,
    const char *url)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    memset(session, 0, sizeof(*session));
    (void)umi_web_workbench_copy_text(session->session_id,
        sizeof(session->session_id), session_id != NULL ? session_id : "stream");
    (void)umi_web_workbench_copy_text(session->name,
        sizeof(session->name), name != NULL ? name : "Stream");
    (void)umi_web_workbench_copy_text(session->url,
        sizeof(session->url), url != NULL ? url : "");
    session->kind = kind;
    session->phase = UMI_WEB_WORKBENCH_IDLE;
    session->next_sequence = 1U;
    session->revision = 1U;
}

/*
 * Provide the web workbench stream set phase operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_workbench_stream_set_phase(
    UmiWebWorkbenchStreamSession *session,
    UmiWebWorkbenchPhase phase)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || phase > UMI_WEB_WORKBENCH_FAILED) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    session->phase = phase;
    session->revision++;
    return UMI_STATUS_OK;
}

/* Add web workbench stream only after its inputs and available capacity have been checked. */
UmiStatus umi_web_workbench_stream_add(
    UmiWebWorkbenchStreamSession *session,
    UmiWebWorkbenchMessageDirection direction,
    const char *event_name,
    const void *payload,
    size_t payload_length,
    uint64_t timestamp_us)
{
    UmiWebWorkbenchStreamMessage *message;
    size_t copied;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || (payload == NULL && payload_length > 0U) ||
        direction > UMI_WEB_WORKBENCH_MESSAGE_OUTBOUND) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (session->paused) return UMI_STATUS_BUSY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->message_count == UMI_WEB_WORKBENCH_MAX_STREAM_MESSAGES) {
        memmove(&session->messages[0], &session->messages[1],
            (session->message_count - 1U) * sizeof(session->messages[0]));
        session->message_count--;
        session->dropped_message_count++;
    }
    message = &session->messages[session->message_count++];
    memset(message, 0, sizeof(*message));
    message->sequence = session->next_sequence++;
    message->timestamp_us = timestamp_us;
    message->direction = direction;
    (void)umi_web_workbench_copy_text(message->event_name,
        sizeof(message->event_name), event_name != NULL ? event_name : "message");
    copied = payload_length < sizeof(message->payload) - 1U
        ? payload_length : sizeof(message->payload) - 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (copied > 0U) memcpy(message->payload, payload, copied);
    message->payload[copied] = '\0';
    message->payload_length = copied;
    message->payload_truncated = copied < payload_length;
    session->revision++;
    return UMI_STATUS_OK;
}

/*
 * Provide the web workbench stream ingest sse operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_workbench_stream_ingest_sse(
    UmiWebWorkbenchStreamSession *session,
    const char *event_text,
    uint64_t timestamp_us)
{
    char event_name[UMI_WEB_WORKBENCH_ID_CAPACITY] = "message";
    char payload[UMI_WEB_WORKBENCH_TEXT_CAPACITY];
    size_t payload_length = 0U;
    const char *cursor;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || event_text == NULL ||
        session->kind != UMI_WEB_WORKBENCH_STREAM_SSE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    payload[0] = '\0';
    cursor = event_text;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        const char *end = strchr(cursor, '\n');
        size_t line_length = end != NULL ? (size_t)(end - cursor) : strlen(cursor);
        /* Apply this branch only when its contract condition is satisfied. */
        if (line_length > 0U && cursor[line_length - 1U] == '\r') line_length--;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (line_length > 6U && strncmp(cursor, "event:", 6U) == 0) {
            const char *value = cursor + 6U;
            size_t value_length;
            /*
             * Continue only while work remains available; the loop body advances the state on each
             * pass.
             */
            while (*value == ' ' && (size_t)(value - cursor) < line_length) ++value;
            value_length = line_length - (size_t)(value - cursor);
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (value_length >= sizeof(event_name)) return UMI_STATUS_CAPACITY_EXCEEDED;
            memcpy(event_name, value, value_length);
            event_name[value_length] = '\0';
        } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (line_length >= 5U && strncmp(cursor, "data:", 5U) == 0) {
            const char *value = cursor + 5U;
            size_t value_length;
            /*
             * Continue only while work remains available; the loop body advances the state on each
             * pass.
             */
            while (*value == ' ' && (size_t)(value - cursor) < line_length) ++value;
            value_length = line_length - (size_t)(value - cursor);
            /* Apply this branch only when its contract condition is satisfied. */
            if (payload_length > 0U) {
                /* Keep the operation inside its valid bounds before reading, writing or adding data. */
                if (payload_length + 1U >= sizeof(payload)) return UMI_STATUS_CAPACITY_EXCEEDED;
                payload[payload_length++] = '\n';
            }
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (payload_length + value_length >= sizeof(payload)) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            memcpy(payload + payload_length, value, value_length);
            payload_length += value_length;
            payload[payload_length] = '\0';
        }
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (end == NULL) break;
        cursor = end + 1;
    }
    return umi_web_workbench_stream_add(session,
        UMI_WEB_WORKBENCH_MESSAGE_INBOUND, event_name, payload,
        payload_length, timestamp_us);
}

/*
 * Provide the web workbench stream query operation used by this module and its client
 * applications.
 */
size_t umi_web_workbench_stream_query(
    const UmiWebWorkbenchStreamSession *session,
    const char *text,
    UmiWebWorkbenchMessageDirection direction,
    bool filter_direction,
    const UmiWebWorkbenchStreamMessage **out_messages,
    size_t capacity)
{
    size_t offset;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || (out_messages == NULL && capacity > 0U)) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (offset = 0U; offset < session->message_count && count < capacity; ++offset) {
        const UmiWebWorkbenchStreamMessage *message =
            &session->messages[session->message_count - offset - 1U];
        /* Apply this branch only when its contract condition is satisfied. */
        if (filter_direction && message->direction != direction) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!contains_case_insensitive(message->event_name, text) &&
            !contains_case_insensitive(message->payload, text)) continue;
        out_messages[count++] = message;
    }
    return count;
}

/*
 * Release or reset state held by web workbench stream so the same storage can be reused
 * safely.
 */
void umi_web_workbench_stream_clear(UmiWebWorkbenchStreamSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    memset(session->messages, 0, sizeof(session->messages));
    session->message_count = 0U;
    session->dropped_message_count = 0U;
    session->revision++;
}
