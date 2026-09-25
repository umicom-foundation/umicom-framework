/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance_operations/repository.c
 *
 * PURPOSE:
 *   Persist and replay complete commands through the sole Data Server authority with explicit stale-writer and corruption checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static UmiStatus EventKey(uint64_t revision, char *key, size_t capacity)
{
    int written = snprintf(key, capacity, FINANCE_EVENT_PREFIX "%020" PRIu64, revision);
    return written < 0 || (size_t)written >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

static UmiStatus ReadRevision(const UmiDataServer *server, uint64_t *out)
{
    char value[64], expected[64];
    uint64_t revision = 0U;
    UmiStatus status = umi_data_server_get(server, FINANCE_REVISION_KEY, value, sizeof(value));
    if (status == UMI_STATUS_NOT_FOUND) { *out = 0U; return UMI_STATUS_OK; }
    if (status != UMI_STATUS_OK) return status;
    if (strncmp(value, "UFIN1:", 6U) != 0 || value[6] == '\0') return UMI_STATUS_PARSE_ERROR;
    for (size_t index = 6U; value[index] != '\0'; ++index) {
        if (value[index] < '0' || value[index] > '9') return UMI_STATUS_PARSE_ERROR;
        revision = revision * 10U + (uint64_t)(value[index] - '0');
        if (revision > UMI_FINANCE_OPERATIONS_EVENTS) return UMI_STATUS_PARSE_ERROR;
    }
    (void)snprintf(expected, sizeof(expected), "UFIN1:%" PRIu64, revision);
    if (strcmp(value, expected) != 0) return UMI_STATUS_PARSE_ERROR;
    *out = revision;
    return UMI_STATUS_OK;
}

typedef struct EventScan { uint64_t revision; size_t count; } EventScan;
static UmiStatus ScanEvent(const char *key, const char *value, void *context)
{
    EventScan *scan = context;
    const size_t prefixLength = sizeof(FINANCE_EVENT_PREFIX) - 1U;
    uint64_t revision = 0U;
    char expected[96];
    (void)value;
    if (strncmp(key, FINANCE_EVENT_PREFIX, prefixLength) != 0) return UMI_STATUS_OK;
    if (strlen(key + prefixLength) != 20U) return UMI_STATUS_PARSE_ERROR;
    for (size_t index = prefixLength; key[index] != '\0'; ++index) {
        if (key[index] < '0' || key[index] > '9') return UMI_STATUS_PARSE_ERROR;
        revision = revision * 10U + (uint64_t)(key[index] - '0');
        if (revision > UMI_FINANCE_OPERATIONS_EVENTS) return UMI_STATUS_PARSE_ERROR;
    }
    if (revision == 0U || revision > scan->revision || EventKey(revision, expected, sizeof(expected)) != UMI_STATUS_OK ||
        strcmp(key, expected) != 0) return UMI_STATUS_PARSE_ERROR;
    ++scan->count;
    return UMI_STATUS_OK;
}

static UmiStatus Rollback(UmiFinanceOperations *operations, UmiStatus previous)
{
    UmiStatus status = umi_data_server_rollback(operations->server);
    if (status != UMI_STATUS_OK) { operations->poisoned = true; return status; }
    return previous;
}

UmiStatus FinanceRepositoryLoad(UmiFinanceOperations *operations, FinanceState **out)
{
    FinanceState *state;
    uint64_t revision = 0U;
    EventScan scan = {0U, 0U};
    UmiStatus status;
    *out = NULL;
    if (umi_data_server_in_transaction(operations->server)) return UMI_STATUS_BUSY;
    status = umi_data_server_begin(operations->server);
    if (status != UMI_STATUS_OK) return status;
    state = calloc(1U, sizeof(*state));
    if (state == NULL) return Rollback(operations, UMI_STATUS_OUT_OF_MEMORY);
    state->counts.durable = umi_data_server_backend(operations->server) == UMI_DATA_BACKEND_SQLITE;
    status = ReadRevision(operations->server, &revision);
    scan.revision = revision;
    if (status == UMI_STATUS_OK) status = umi_data_server_visit(operations->server, ScanEvent, &scan);
    if (status == UMI_STATUS_OK && scan.count != (size_t)revision) status = UMI_STATUS_PARSE_ERROR;
    for (uint64_t index = 1U; status == UMI_STATUS_OK && index <= revision; ++index) {
        UmiFinanceOperationCommand command;
        char key[96], value[FINANCE_RECORD_CAPACITY];
        status = EventKey(index, key, sizeof(key));
        if (status == UMI_STATUS_OK) status = umi_data_server_get(operations->server, key, value, sizeof(value));
        if (status == UMI_STATUS_OK) status = FinanceDecode(value, &command);
        if (status == UMI_STATUS_OK && command.expectedRevision != state->counts.revision) status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK) status = FinanceExecute(state, &command);
        if (status != UMI_STATUS_OK && status != UMI_STATUS_IO_ERROR && status != UMI_STATUS_OUT_OF_MEMORY)
            status = UMI_STATUS_PARSE_ERROR;
    }
    /* Ending the read transaction is part of success. A rollback failure is
     * not ignored, because ownership of the connection would then be unclear. */
    status = Rollback(operations, status);
    if (status != UMI_STATUS_OK) { free(state); return status; }
    *out = state;
    return UMI_STATUS_OK;
}

UmiStatus FinanceRepositoryCommit(UmiFinanceOperations *operations,
    const UmiFinanceOperationCommand *command)
{
    uint64_t actualRevision = 0U;
    char key[96], value[FINANCE_RECORD_CAPACITY], previous[FINANCE_RECORD_CAPACITY], revisionText[64];
    UmiStatus status;
    if (umi_data_server_in_transaction(operations->server)) return UMI_STATUS_BUSY;
    status = FinanceEncode(command, value, sizeof(value));
    if (status != UMI_STATUS_OK) return status;
    status = umi_data_server_begin(operations->server);
    if (status != UMI_STATUS_OK) return status;
    status = ReadRevision(operations->server, &actualRevision);
    if (status == UMI_STATUS_OK && actualRevision != operations->state->counts.revision) status = UMI_STATUS_BUSY;
    if (status == UMI_STATUS_OK) status = EventKey(actualRevision + 1U, key, sizeof(key));
    if (status == UMI_STATUS_OK) {
        status = umi_data_server_get(operations->server, key, previous, sizeof(previous));
        if (status == UMI_STATUS_NOT_FOUND) status = UMI_STATUS_OK;
        else if (status == UMI_STATUS_OK) status = UMI_STATUS_PARSE_ERROR;
    }
    if (status == UMI_STATUS_OK) status = umi_data_server_set(operations->server, key, value);
    (void)snprintf(revisionText, sizeof(revisionText), "UFIN1:%" PRIu64, actualRevision + 1U);
    if (status == UMI_STATUS_OK) status = umi_data_server_set(operations->server, FINANCE_REVISION_KEY, revisionText);
    if (status == UMI_STATUS_OK) status = umi_data_server_commit(operations->server);
    if (status != UMI_STATUS_OK) return Rollback(operations, status);
    return UMI_STATUS_OK;
}
