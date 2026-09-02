/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/workbench/history.h
 *
 * PURPOSE:
 *   Retain a bounded, newest-first execution history without storing secrets or
 *   full payloads. Entries provide status, duration and transfer evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_WORKBENCH_HISTORY_H
#define UMICOM_WEB_WORKBENCH_HISTORY_H

#include "umicom/web/workbench/request.h"
#include "umicom/web/workbench/response.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the web workbench history entry data shared with callers of this public
 * contract.
 */
typedef struct UmiWebWorkbenchHistoryEntry {
    uint64_t execution_id;
    char request_id[UMI_WEB_WORKBENCH_ID_CAPACITY];
    char request_name[UMI_WEB_WORKBENCH_NAME_CAPACITY];
    UmiHttpMethod method;
    char url[UMI_WEB_WORKBENCH_URL_CAPACITY];
    UmiWebWorkbenchPhase phase;
    UmiStatus status;
    int http_status;
    uint64_t duration_us;
    size_t sent_bytes;
    size_t received_bytes;
    bool tls_verified;
} UmiWebWorkbenchHistoryEntry;

/**
 * Represent the web workbench history data shared with callers of this public contract.
 */
typedef struct UmiWebWorkbenchHistory {
    UmiWebWorkbenchHistoryEntry entries[UMI_WEB_WORKBENCH_MAX_HISTORY];
    size_t entry_count;
    uint64_t next_execution_id;
    uint64_t revision;
} UmiWebWorkbenchHistory;

/**
 * Initialise web workbench history from caller-provided values so later operations receive
 * a known state.
 */
void umi_web_workbench_history_init(UmiWebWorkbenchHistory *history);
/**
 * Provide the web workbench history begin operation used by this module and its client
 * applications.
 */
uint64_t umi_web_workbench_history_begin(
    UmiWebWorkbenchHistory *history,
    const UmiWebWorkbenchRequest *request);
/**
 * Provide the web workbench history complete operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_workbench_history_complete(
    UmiWebWorkbenchHistory *history,
    uint64_t execution_id,
    const UmiWebWorkbenchResponse *response);
/**
 * Find web workbench history while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiWebWorkbenchHistoryEntry *umi_web_workbench_history_at(
    const UmiWebWorkbenchHistory *history,
    size_t newest_first_index);
/**
 * Provide the web workbench history query operation used by this module and its client
 * applications.
 */
size_t umi_web_workbench_history_query(
    const UmiWebWorkbenchHistory *history,
    const char *text,
    int minimum_http_status,
    UmiWebWorkbenchHistoryEntry *out_entries,
    size_t capacity);
/**
 * Release or reset state held by web workbench history so the same storage can be reused
 * safely.
 */
void umi_web_workbench_history_clear(UmiWebWorkbenchHistory *history);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_WEB_WORKBENCH_HISTORY_H */
