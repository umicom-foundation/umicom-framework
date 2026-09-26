/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/enterprise_workspace/storage.c
 *
 * PURPOSE:
 *   Persist complete reviewed states through one Data Server transaction with stale-writer checks.
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

typedef struct Head { uint64_t revision, hash; size_t bytes, chunks; } Head;
/* FNV-1a detects accidental torn/corrupt content. It is NOT a signature, a
 * password hash or protection against an owner editing their database. */
static uint64_t Fingerprint(const char *text, size_t length)
{
    uint64_t hash = UINT64_C(14695981039346656037);
    for (size_t i = 0U; i < length; ++i) { hash ^= (unsigned char)text[i]; hash *= UINT64_C(1099511628211); }
    return hash;
}
static bool Number(const char **cursor, uint64_t maximum, uint64_t *value)
{
    const char *start = *cursor;
    uint64_t result = 0U;
    while (**cursor != '\0' && **cursor != '\n') {
        unsigned char ch = (unsigned char)*(*cursor)++;
        if (ch < '0' || ch > '9' || (uint64_t)(ch - '0') > maximum || result > (maximum - (ch - '0')) / 10U) return false;
        result = result * 10U + (ch - '0');
    }
    if (*cursor == start || **cursor != '\n' || (*cursor - start > 1 && start[0] == '0')) return false;
    ++*cursor; *value = result; return true;
}
static UmiStatus ParseHead(const char *text, Head *head)
{
    uint64_t schema, bytes, chunks;
    const char *cursor = text;
    if (!Number(&cursor, 1U, &schema) || schema != 1U || !Number(&cursor, UMI_ENTERPRISE_MAX_AUDIT, &head->revision) ||
        !Number(&cursor, EWS_WIRE_CAPACITY - 1U, &bytes) || !Number(&cursor, EWS_WIRE_CAPACITY / EWS_CHUNK_SIZE + 1U, &chunks) ||
        !Number(&cursor, UINT64_MAX, &head->hash) || *cursor != '\0' || bytes == 0U || chunks == 0U ||
        chunks != (bytes + EWS_CHUNK_SIZE - 1U) / EWS_CHUNK_SIZE) return UMI_STATUS_PARSE_ERROR;
    head->bytes = (size_t)bytes; head->chunks = (size_t)chunks;
    return UMI_STATUS_OK;
}
static void ChunkKey(size_t index, char *key, size_t capacity)
{
    (void)snprintf(key, capacity, EWS_PREFIX "chunk/%04zu", index);
}
static UmiStatus Rollback(UmiEnterpriseWorkspace *workspace, UmiStatus result)
{
    UmiStatus rollback = umi_data_server_rollback(workspace->data);
    if (rollback != UMI_STATUS_OK) { workspace->storageFault = true; return rollback; }
    return result;
}
typedef struct Inventory { size_t chunks, count; bool hasHead; } Inventory;
static UmiStatus VisitOwned(const char *key, const char *value, void *context)
{
    Inventory *inventory = context;
    char expected[96];
    (void)value;
    if (strncmp(key, EWS_PREFIX, sizeof(EWS_PREFIX) - 1U) != 0) return UMI_STATUS_OK;
    ++inventory->count;
    if (!inventory->hasHead) return UMI_STATUS_PARSE_ERROR;
    if (strcmp(key, EWS_HEAD) == 0) return UMI_STATUS_OK;
    for (size_t i = 0U; i < inventory->chunks; ++i) {
        ChunkKey(i, expected, sizeof(expected));
        if (strcmp(key, expected) == 0) return UMI_STATUS_OK;
    }
    return UMI_STATUS_PARSE_ERROR;
}
static UmiStatus CheckInventory(UmiDataServer *data, size_t chunks, bool hasHead)
{
    Inventory inventory = {chunks, 0U, hasHead};
    UmiStatus status = umi_data_server_visit(data, VisitOwned, &inventory);
    if (status != UMI_STATUS_OK) return status;
    return inventory.count == (hasHead ? chunks + 1U : 0U) ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}

UmiStatus EwsLoad(UmiEnterpriseWorkspace *workspace, EwsState *state,
    char outHead[EWS_HEAD_CAPACITY])
{
    char textHead[EWS_HEAD_CAPACITY], chunk[EWS_CHUNK_SIZE + 1U], key[96];
    Head head = {0};
    char *text = NULL;
    size_t used = 0U;
    UmiStatus status;
    if (umi_data_server_in_transaction(workspace->data)) return UMI_STATUS_BUSY;
    status = umi_data_server_begin(workspace->data);
    if (status != UMI_STATUS_OK) return status;
    status = umi_data_server_get(workspace->data, EWS_HEAD, textHead, sizeof(textHead));
    if (status == UMI_STATUS_NOT_FOUND) {
        status = CheckInventory(workspace->data, 0U, false);
        status = Rollback(workspace, status);
        if (status == UMI_STATUS_OK) { (void)memset(state, 0, sizeof(*state)); outHead[0] = '\0'; }
        return status;
    }
    if (status == UMI_STATUS_OK) status = ParseHead(textHead, &head);
    if (status == UMI_STATUS_OK) status = CheckInventory(workspace->data, head.chunks, true);
    if (status == UMI_STATUS_OK) {
        text = malloc(head.bytes + 1U);
        if (text == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    for (size_t i = 0U; status == UMI_STATUS_OK && i < head.chunks; ++i) {
        size_t expected = head.bytes - used;
        if (expected > EWS_CHUNK_SIZE) expected = EWS_CHUNK_SIZE;
        ChunkKey(i, key, sizeof(key));
        status = umi_data_server_get(workspace->data, key, chunk, sizeof(chunk));
        if (status == UMI_STATUS_OK) {
            if (strlen(chunk) != expected) status = UMI_STATUS_PARSE_ERROR;
            else { (void)memcpy(text + used, chunk, expected); used += expected; }
        }
    }
    if (status == UMI_STATUS_OK) {
        text[used] = '\0';
        if (used != head.bytes || Fingerprint(text, used) != head.hash) status = UMI_STATUS_PARSE_ERROR;
        else status = EwsDecode(text, used, state);
        if (status == UMI_STATUS_OK && state->revision != head.revision) status = UMI_STATUS_PARSE_ERROR;
    }
    free(text);
    status = Rollback(workspace, status);
    if (status == UMI_STATUS_OK) (void)EwsCopy(outHead, EWS_HEAD_CAPACITY, textHead);
    return status;
}
/* Used even for idempotent no-ops: a stale view must not falsely report that
 * another process's pause or recipe setting already has the requested value. */
UmiStatus EwsCurrent(UmiEnterpriseWorkspace *workspace)
{
    char head[EWS_HEAD_CAPACITY];
    UmiStatus status;
    if (workspace->storageFault) return UMI_STATUS_INVALID_STATE;
    if (umi_data_server_in_transaction(workspace->data)) return UMI_STATUS_BUSY;
    status = umi_data_server_begin(workspace->data);
    if (status != UMI_STATUS_OK) return status;
    status = umi_data_server_get(workspace->data, EWS_HEAD, head, sizeof(head));
    if (status == UMI_STATUS_NOT_FOUND && workspace->head[0] == '\0') status = CheckInventory(workspace->data, 0U, false);
    else if (status == UMI_STATUS_NOT_FOUND) status = UMI_STATUS_BUSY;
    else if (status == UMI_STATUS_OK && strcmp(head, workspace->head) != 0) status = UMI_STATUS_BUSY;
    return Rollback(workspace, status);
}
UmiStatus EwsSave(UmiEnterpriseWorkspace *workspace, const EwsState *state)
{
    char current[EWS_HEAD_CAPACITY], nextHead[EWS_HEAD_CAPACITY], key[96], chunk[EWS_CHUNK_SIZE + 1U];
    char *text = NULL;
    Head old = {0};
    size_t bytes = 0U, chunks, used = 0U;
    UmiStatus status;
    int n;
    status = EwsEncode(state, &text, &bytes);
    if (status != UMI_STATUS_OK) return status;
    chunks = (bytes + EWS_CHUNK_SIZE - 1U) / EWS_CHUNK_SIZE;
    n = snprintf(nextHead, sizeof(nextHead), "1\n%" PRIu64 "\n%zu\n%zu\n%" PRIu64 "\n",
        state->revision, bytes, chunks, Fingerprint(text, bytes));
    if (n < 0 || (size_t)n >= sizeof(nextHead)) { free(text); return UMI_STATUS_CAPACITY_EXCEEDED; }
    if (umi_data_server_in_transaction(workspace->data)) { free(text); return UMI_STATUS_BUSY; }
    status = umi_data_server_begin(workspace->data);
    if (status != UMI_STATUS_OK) { free(text); return status; }
    status = umi_data_server_get(workspace->data, EWS_HEAD, current, sizeof(current));
    if (status == UMI_STATUS_NOT_FOUND && workspace->head[0] == '\0') status = CheckInventory(workspace->data, 0U, false);
    else if (status == UMI_STATUS_NOT_FOUND) status = UMI_STATUS_BUSY;
    else if (status == UMI_STATUS_OK) {
        if (strcmp(current, workspace->head) != 0) status = UMI_STATUS_BUSY;
        else status = ParseHead(current, &old);
    }
    for (size_t i = 0U; status == UMI_STATUS_OK && i < chunks; ++i) {
        size_t length = bytes - used;
        if (length > EWS_CHUNK_SIZE) length = EWS_CHUNK_SIZE;
        (void)memcpy(chunk, text + used, length); chunk[length] = '\0'; used += length;
        ChunkKey(i, key, sizeof(key));
        status = umi_data_server_set(workspace->data, key, chunk);
    }
    for (size_t i = chunks; status == UMI_STATUS_OK && i < old.chunks; ++i) {
        ChunkKey(i, key, sizeof(key)); status = umi_data_server_delete(workspace->data, key);
    }
    if (status == UMI_STATUS_OK) status = umi_data_server_set(workspace->data, EWS_HEAD, nextHead);
    if (status == UMI_STATUS_OK) status = umi_data_server_commit(workspace->data);
    if (status != UMI_STATUS_OK) status = Rollback(workspace, status);
    else (void)EwsCopy(workspace->head, sizeof(workspace->head), nextHead);
    free(text);
    return status;
}
