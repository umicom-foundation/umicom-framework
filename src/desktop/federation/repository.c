/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/federation/repository.c
 *
 * PURPOSE:
 *   Persist saved workspaces atomically through the canonical Data Server only.
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

#define DF_META "desk-federation/meta"
/* One member per key keeps every record well below the Data Server value
 * limit. No raw struct, host path, executable or live process token is stored. */
static void WorkspaceKey(size_t index, char *key, size_t capacity)
{
    (void)snprintf(key, capacity, "desk-federation/workspace/%zu", index);
}
static void MemberKey(size_t index, size_t member, char *key, size_t capacity)
{
    (void)snprintf(key, capacity, "desk-federation/workspace/%zu/member/%zu", index, member);
}
static UmiStatus ReadMeta(UmiDataServer *server, uint64_t *revision, size_t *count)
{
    char text[96], *separator;
    uint64_t size;
    UmiStatus status = umi_data_server_get(server, DF_META, text, sizeof text);
    if (status == UMI_STATUS_NOT_FOUND) { *revision = 0U; *count = 0U; return UMI_STATUS_OK; }
    if (status != UMI_STATUS_OK) return status == UMI_STATUS_CAPACITY_EXCEEDED ? UMI_STATUS_PARSE_ERROR : status;
    if (strncmp(text, "DF1|", 4U) != 0) return UMI_STATUS_PARSE_ERROR;
    separator = strchr(text + 4U, '|');
    if (separator == NULL) return UMI_STATUS_PARSE_ERROR;
    *separator++ = '\0';
    if (DfParseUnsigned(text + 4U, revision) != UMI_STATUS_OK ||
        DfParseUnsigned(separator, &size) != UMI_STATUS_OK || size > UMI_DESK_FEDERATION_MAX_WORKSPACES)
        return UMI_STATUS_PARSE_ERROR;
    *count = (size_t)size; return UMI_STATUS_OK;
}
static UmiStatus Rollback(UmiDeskFederation *service, UmiStatus cause)
{
    if (umi_data_server_rollback(service->server) != UMI_STATUS_OK) {
        /* After a failed rollback, no later mutation may claim a clean boundary.
         * Dispose this service and its connection instead of silently retrying. */
        service->poisoned = true; return UMI_STATUS_IO_ERROR;
    }
    return cause;
}
static UmiStatus Begin(UmiDeskFederation *service)
{
    if (umi_data_server_in_transaction(service->server)) return UMI_STATUS_BUSY;
    return umi_data_server_begin(service->server);
}
/* Requiring all namespace keys to match the meta/header counts prevents a
 * deleted meta record from turning a damaged saved session into an empty one. */
typedef struct NamespaceCheck { size_t count; } NamespaceCheck;
static UmiStatus CountNamespace(const char *key, const char *value, void *context)
{
    NamespaceCheck *check = context;
    (void)value;
    if (strncmp(key, "desk-federation/", 16U) == 0) ++check->count;
    return UMI_STATUS_OK;
}
static UmiStatus ReadStore(UmiDeskFederation *service, DfStore *outStore)
{
    char key[96], text[640];
    size_t expected = 0U;
    NamespaceCheck check = {0};
    UmiStatus status;
    memset(outStore, 0, sizeof *outStore);
    status = ReadMeta(service->server, &outStore->revision, &outStore->count);
    if (status == UMI_STATUS_OK) {
        UmiStatus metaStatus = umi_data_server_get(service->server, DF_META, text, sizeof text);
        if (metaStatus == UMI_STATUS_OK) expected = 1U;
        else if (metaStatus != UMI_STATUS_NOT_FOUND) status = metaStatus;
    }
    for (size_t index = 0U; status == UMI_STATUS_OK && index < outStore->count; ++index) {
        UmiDeskWorkspace *workspace = &outStore->workspaces[index];
        WorkspaceKey(index, key, sizeof key);
        status = umi_data_server_get(service->server, key, text, sizeof text);
        if (status == UMI_STATUS_OK) status = DfDecodeHeader(text, workspace);
        if (status != UMI_STATUS_OK) break;
        ++expected;
        for (size_t member = 0U; status == UMI_STATUS_OK && member < workspace->memberCount; ++member) {
            MemberKey(index, member, key, sizeof key);
            status = umi_data_server_get(service->server, key, workspace->members[member].value,
                sizeof workspace->members[member].value);
            ++expected;
        }
    }
    if (status == UMI_STATUS_OK) status = DfStoreValidate(outStore);
    if (status == UMI_STATUS_OK) status = umi_data_server_visit(service->server, CountNamespace, &check);
    if (status == UMI_STATUS_OK && check.count != expected) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_NOT_FOUND || status == UMI_STATUS_CAPACITY_EXCEEDED) status = UMI_STATUS_PARSE_ERROR;
    return status;
}
UmiStatus DfRepositoryLoad(UmiDeskFederation *service, DfStore *outStore)
{
    UmiStatus status = Begin(service);
    if (status != UMI_STATUS_OK) return status;
    status = ReadStore(service, outStore);
    return Rollback(service, status); /* Read transaction: commit no state. */
}
/* Metadata is a concurrency guard, not proof of intact contents. Inspect the
 * complete namespace inside the transaction so same-revision damage cannot
 * silently overwrite a saved workspace or authorise an obsolete review. This
 * detects changes; it is not cryptographic authentication of local storage. */
static UmiStatus CheckStore(UmiDeskFederation *service)
{
    DfStore *current = calloc(1U, sizeof *current);
    UmiStatus status;
    if (current == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = ReadStore(service, current);
    if (status == UMI_STATUS_OK && (current->revision != service->store->revision ||
        current->count != service->store->count)) status = UMI_STATUS_BUSY;
    for (size_t index = 0U; status == UMI_STATUS_OK && index < current->count; ++index) {
        const UmiDeskWorkspace *left = &current->workspaces[index];
        const UmiDeskWorkspace *right = &service->store->workspaces[index];
        if (left->kind != right->kind || strcmp(left->id, right->id) != 0 ||
            strcmp(left->title, right->title) != 0 || left->memberCount != right->memberCount)
            status = UMI_STATUS_PARSE_ERROR;
        for (size_t member = 0U; status == UMI_STATUS_OK && member < left->memberCount; ++member)
            if (strcmp(left->members[member].value, right->members[member].value) != 0)
                status = UMI_STATUS_PARSE_ERROR;
    }
    free(current);
    return status;
}
UmiStatus DfRepositoryCheck(UmiDeskFederation *service)
{
    UmiStatus status = Begin(service);
    if (status != UMI_STATUS_OK) return status;
    status = CheckStore(service);
    return Rollback(service, status);
}
UmiStatus DfRepositoryCommit(UmiDeskFederation *service, const DfStore *candidate)
{
    char key[96], text[640];
    UmiStatus status = Begin(service);
    if (status != UMI_STATUS_OK) return status;
    status = CheckStore(service);
    /* Delete only our old counted keys in this transaction. This removes stale
     * members after shrink/removal while leaving every other namespace alone. */
    for (size_t index = 0U; status == UMI_STATUS_OK && index < service->store->count; ++index) {
        WorkspaceKey(index, key, sizeof key); status = umi_data_server_delete(service->server, key);
        for (size_t member = 0U; status == UMI_STATUS_OK && member < service->store->workspaces[index].memberCount; ++member) {
            MemberKey(index, member, key, sizeof key); status = umi_data_server_delete(service->server, key);
        }
    }
    for (size_t index = 0U; status == UMI_STATUS_OK && index < candidate->count; ++index) {
        const UmiDeskWorkspace *workspace = &candidate->workspaces[index];
        WorkspaceKey(index, key, sizeof key);
        status = DfEncodeHeader(workspace, text, sizeof text);
        if (status == UMI_STATUS_OK) status = umi_data_server_set(service->server, key, text);
        for (size_t member = 0U; status == UMI_STATUS_OK && member < workspace->memberCount; ++member) {
            MemberKey(index, member, key, sizeof key);
            status = umi_data_server_set(service->server, key, workspace->members[member].value);
        }
    }
    if (status == UMI_STATUS_OK) {
        (void)snprintf(text, sizeof text, "DF1|%" PRIu64 "|%zu", candidate->revision, candidate->count);
        status = umi_data_server_set(service->server, DF_META, text);
    }
    if (status == UMI_STATUS_OK) status = umi_data_server_commit(service->server);
    if (status != UMI_STATUS_OK) return Rollback(service, status);
    return UMI_STATUS_OK;
}
