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

typedef struct UmiWebWorkbenchHistory {
    UmiWebWorkbenchHistoryEntry entries[UMI_WEB_WORKBENCH_MAX_HISTORY];
    size_t entry_count;
    uint64_t next_execution_id;
    uint64_t revision;
} UmiWebWorkbenchHistory;

void umi_web_workbench_history_init(UmiWebWorkbenchHistory *history);
uint64_t umi_web_workbench_history_begin(
    UmiWebWorkbenchHistory *history,
    const UmiWebWorkbenchRequest *request);
UmiStatus umi_web_workbench_history_complete(
    UmiWebWorkbenchHistory *history,
    uint64_t execution_id,
    const UmiWebWorkbenchResponse *response);
const UmiWebWorkbenchHistoryEntry *umi_web_workbench_history_at(
    const UmiWebWorkbenchHistory *history,
    size_t newest_first_index);
size_t umi_web_workbench_history_query(
    const UmiWebWorkbenchHistory *history,
    const char *text,
    int minimum_http_status,
    UmiWebWorkbenchHistoryEntry *out_entries,
    size_t capacity);
void umi_web_workbench_history_clear(UmiWebWorkbenchHistory *history);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_WEB_WORKBENCH_HISTORY_H */
