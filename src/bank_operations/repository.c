/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/repository.c
 *
 * PURPOSE:
 *   Persist and replay banking events through the authoritative Data Server transaction boundary.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* This typed repository is the only banking layer that touches the Data Server.
 * No SQL, SQLite handle or filesystem operation leaks into the financial engine
 * or Bank application. Events and their high-water mark commit together. */
static UmiStatus ReadRevision(UmiDataServer *server, uint64_t *out)
{
    char text[32];
    UmiStatus status = umi_data_server_get(server, BANK_REVISION_KEY, text, sizeof text);
    uint64_t value = 0U;
    *out = 0U;
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    if (status != UMI_STATUS_OK) return status;
    if (strncmp(text, "UBANK1:", 7U) != 0 || text[7] == '\0') return UMI_STATUS_PARSE_ERROR;
    if (text[7] == '0' && text[8] != '\0') return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 7U; text[i] != '\0'; ++i) {
        if (text[i] < '0' || text[i] > '9') return UMI_STATUS_PARSE_ERROR;
        value = value * 10U + (uint64_t)(text[i] - '0');
        if (value > UMI_BANK_EVENT_CAPACITY) return UMI_STATUS_PARSE_ERROR;
    }
    *out = value;
    return UMI_STATUS_OK;
}

static void EventKey(uint64_t revision, char key[80])
{
    (void)snprintf(key, 80U, BANK_EVENT_KEY_PREFIX "%020" PRIu64, revision);
}

/* Never hide an uncertain rollback behind the original error. A poisoned handle
 * exposes no cached balances and accepts no subsequent mutations. */
static UmiStatus Rollback(UmiBankOperations *operations, UmiStatus original)
{
    UmiStatus status = umi_data_server_rollback(operations->server);
    if (status != UMI_STATUS_OK) { operations->poisoned = true; return status; }
    return original;
}

typedef struct BankInventory {
    size_t count;
    uint64_t revision;
} BankInventory;

/* Enumeration runs under the Data Server lock. This callback must not call
 * back into the server. Every event key is canonical and within the high-water
 * mark; orphaned or extra events fail closed rather than being ignored. */
static UmiStatus CountEvent(const char *key, const char *value, void *userData)
{
    BankInventory *inventory = userData;
    const size_t prefix = sizeof BANK_EVENT_KEY_PREFIX - 1U;
    uint64_t revision = 0U;
    char expected[80];
    (void)value;
    if (strncmp(key, BANK_EVENT_KEY_PREFIX, prefix) != 0) return UMI_STATUS_OK;
    if (strlen(key) != prefix + 20U) return UMI_STATUS_PARSE_ERROR;
    for (size_t i = prefix; i < prefix + 20U; ++i) {
        if (key[i] < '0' || key[i] > '9') return UMI_STATUS_PARSE_ERROR;
        /* An event number above capacity is invalid before multiplication can overflow. */
        if (revision > UMI_BANK_EVENT_CAPACITY) return UMI_STATUS_PARSE_ERROR;
        revision = revision * 10U + (uint64_t)(key[i] - '0');
    }
    if (revision == 0U || revision > inventory->revision) return UMI_STATUS_PARSE_ERROR;
    EventKey(revision, expected);
    if (strcmp(key, expected) != 0) return UMI_STATUS_PARSE_ERROR;
    inventory->count++;
    return UMI_STATUS_OK;
}

UmiStatus BankRepositoryLoad(UmiBankOperations *operations, BankState **outState)
{
    BankState *loaded;
    BankInventory inventory = {0U, 0U};
    UmiStatus status;
    if (operations == NULL || outState == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outState = NULL;
    loaded = calloc(1U, sizeof *loaded);
    if (loaded == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_data_server_begin(operations->server);
    if (status != UMI_STATUS_OK) { free(loaded); return status; }
    status = ReadRevision(operations->server, &inventory.revision);
    if (status == UMI_STATUS_OK)
        status = umi_data_server_visit(operations->server, CountEvent, &inventory);
    if (status == UMI_STATUS_OK && inventory.count != (size_t)inventory.revision)
        status = UMI_STATUS_PARSE_ERROR;
    for (uint64_t revision = 1U; status == UMI_STATUS_OK && revision <= inventory.revision; ++revision) {
        char key[80];
        char text[BANK_RECORD_TEXT_CAPACITY];
        UmiBankAuditEvent event;
        EventKey(revision, key);
        status = umi_data_server_get(operations->server, key, text, sizeof text);
        if (status == UMI_STATUS_OK) status = BankDecode(text, &event);
        if (status == UMI_STATUS_OK && event.revision != revision) status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK) status = BankApply(loaded, &event.actor, &event.command);
        /* A missing or semantically impossible committed event is damaged input,
         * not a user command that can be skipped or automatically repaired. */
        if (status != UMI_STATUS_OK && status != UMI_STATUS_IO_ERROR && status != UMI_STATUS_OUT_OF_MEMORY)
            status = UMI_STATUS_PARSE_ERROR;
    }
    status = Rollback(operations, status); /* Read-only transaction; no writes to retain. */
    if (status != UMI_STATUS_OK) { free(loaded); return status; }
    loaded->counts.durable = umi_data_server_backend(operations->server) == UMI_DATA_BACKEND_SQLITE;
    *outState = loaded;
    return UMI_STATUS_OK;
}

UmiStatus BankRepositoryCommit(UmiBankOperations *operations,
    const UmiBankAuditEvent *event, uint64_t expectedRevision)
{
    char key[80], text[BANK_RECORD_TEXT_CAPACITY], revisionText[32], existing[2];
    uint64_t current = 0U;
    UmiStatus status;
    if (operations == NULL || event == NULL || event->revision != expectedRevision + 1U)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = BankEncode(event, text, sizeof text);
    if (status != UMI_STATUS_OK) return status;
    status = umi_data_server_begin(operations->server);
    if (status != UMI_STATUS_OK) return status;
    status = ReadRevision(operations->server, &current);
    if (status == UMI_STATUS_OK && current != expectedRevision) status = UMI_STATUS_BUSY;
    EventKey(event->revision, key);
    if (status == UMI_STATUS_OK) {
        UmiStatus probe = umi_data_server_get(operations->server, key, existing, sizeof existing);
        if (probe != UMI_STATUS_NOT_FOUND)
            status = probe == UMI_STATUS_OK || probe == UMI_STATUS_CAPACITY_EXCEEDED ?
                UMI_STATUS_PARSE_ERROR : probe;
    }
    if (status == UMI_STATUS_OK) status = umi_data_server_set(operations->server, key, text);
    (void)snprintf(revisionText, sizeof revisionText, "UBANK1:%" PRIu64, event->revision);
    if (status == UMI_STATUS_OK) status = umi_data_server_set(operations->server, BANK_REVISION_KEY, revisionText);
    if (status == UMI_STATUS_OK) status = umi_data_server_commit(operations->server);
    return status == UMI_STATUS_OK ? status : Rollback(operations, status);
}
