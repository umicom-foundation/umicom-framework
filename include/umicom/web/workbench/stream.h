/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/workbench/stream.h
 *
 * PURPOSE:
 *   Retain bounded WebSocket and Server-Sent Event sessions for inspection,
 *   filtering, pause/resume and message-direction analysis.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_WORKBENCH_STREAM_H
#define UMICOM_WEB_WORKBENCH_STREAM_H

#include "umicom/web/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the web workbench stream message data shared with callers of this public
 * contract.
 */
typedef struct UmiWebWorkbenchStreamMessage {
    uint64_t sequence;
    uint64_t timestamp_us;
    UmiWebWorkbenchMessageDirection direction;
    char event_name[UMI_WEB_WORKBENCH_ID_CAPACITY];
    char payload[UMI_WEB_WORKBENCH_TEXT_CAPACITY];
    size_t payload_length;
    bool payload_truncated;
} UmiWebWorkbenchStreamMessage;

/**
 * Represent the web workbench stream session data shared with callers of this public
 * contract.
 */
typedef struct UmiWebWorkbenchStreamSession {
    char session_id[UMI_WEB_WORKBENCH_ID_CAPACITY];
    char name[UMI_WEB_WORKBENCH_NAME_CAPACITY];
    char url[UMI_WEB_WORKBENCH_URL_CAPACITY];
    UmiWebWorkbenchStreamKind kind;
    UmiWebWorkbenchPhase phase;
    UmiWebWorkbenchStreamMessage messages[
        UMI_WEB_WORKBENCH_MAX_STREAM_MESSAGES];
    size_t message_count;
    size_t dropped_message_count;
    uint64_t next_sequence;
    uint64_t revision;
    bool paused;
} UmiWebWorkbenchStreamSession;

/**
 * Initialise web workbench stream from caller-provided values so later operations receive
 * a known state.
 */
void umi_web_workbench_stream_init(
    UmiWebWorkbenchStreamSession *session,
    const char *session_id,
    const char *name,
    UmiWebWorkbenchStreamKind kind,
    const char *url);
/**
 * Provide the web workbench stream set phase operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_workbench_stream_set_phase(
    UmiWebWorkbenchStreamSession *session,
    UmiWebWorkbenchPhase phase);
/**
 * Add web workbench stream only after its inputs and available capacity have been checked.
 */
UmiStatus umi_web_workbench_stream_add(
    UmiWebWorkbenchStreamSession *session,
    UmiWebWorkbenchMessageDirection direction,
    const char *event_name,
    const void *payload,
    size_t payload_length,
    uint64_t timestamp_us);
/**
 * Provide the web workbench stream ingest sse operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_workbench_stream_ingest_sse(
    UmiWebWorkbenchStreamSession *session,
    const char *event_text,
    uint64_t timestamp_us);
/**
 * Provide the web workbench stream query operation used by this module and its client
 * applications.
 */
size_t umi_web_workbench_stream_query(
    const UmiWebWorkbenchStreamSession *session,
    const char *text,
    UmiWebWorkbenchMessageDirection direction,
    bool filter_direction,
    const UmiWebWorkbenchStreamMessage **out_messages,
    size_t capacity);
/**
 * Release or reset state held by web workbench stream so the same storage can be reused
 * safely.
 */
void umi_web_workbench_stream_clear(UmiWebWorkbenchStreamSession *session);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_WEB_WORKBENCH_STREAM_H */
