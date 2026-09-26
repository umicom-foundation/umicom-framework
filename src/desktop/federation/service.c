/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/federation/service.c
 *
 * PURPOSE:
 *   Own workspace state and lifecycle without taking over the existing launcher.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <stdlib.h>
#include <string.h>

UmiStatus UmiDeskFederationCreate(UmiDataServer *server, const UmiDeskFederationPorts *ports,
    const char *hostApplicationId, UmiDeskFederation **outService)
{
    UmiDeskFederation *service;
    char host[UMI_DESK_FEDERATION_APP_ID_CAPACITY] = {0};
    if (outService == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outService = NULL;
    if (server == NULL || ports == NULL || ports->read == NULL || ports->request == NULL ||
        DfCopy(host, sizeof host, hostApplicationId) != UMI_STATUS_OK || !DfIdValid(host, sizeof host))
        return UMI_STATUS_INVALID_ARGUMENT;
    service = calloc(1U, sizeof *service);
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    service->store = calloc(1U, sizeof *service->store);
    if (service->store == NULL) { free(service); return UMI_STATUS_OUT_OF_MEMORY; }
    service->server = server; service->ports = *ports;
    memcpy(service->hostId, host, sizeof host);
    *outService = service; return UMI_STATUS_OK;
}
void UmiDeskFederationDestroy(UmiDeskFederation *service)
{
    if (service == NULL) return;
    free(service->store); free(service);
}
UmiStatus UmiDeskFederationReload(UmiDeskFederation *service)
{
    DfStore *candidate;
    UmiStatus status = DfEnter(service, false);
    if (status != UMI_STATUS_OK) return status;
    service->plan.token = 0U;
    candidate = calloc(1U, sizeof *candidate);
    if (candidate == NULL) { service->busy = false; return UMI_STATUS_OUT_OF_MEMORY; }
    status = DfRepositoryLoad(service, candidate);
    if (status == UMI_STATUS_OK) {
        DfStore *old = service->store; service->store = candidate; candidate = old; service->loaded = true;
    }
    free(candidate); service->busy = false; return status;
}
UmiStatus UmiDeskFederationWorkspaceAt(const UmiDeskFederation *service, size_t index, UmiDeskWorkspace *outWorkspace)
{
    if (service == NULL || outWorkspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!service->loaded) return UMI_STATUS_INVALID_STATE;
    if (index >= service->store->count) return UMI_STATUS_NOT_FOUND;
    *outWorkspace = service->store->workspaces[index]; return UMI_STATUS_OK;
}
UmiStatus UmiDeskFederationWorkspaceFind(const UmiDeskFederation *service, UmiDeskWorkspaceKind kind,
    const char *id, UmiDeskWorkspace *outWorkspace)
{
    char safeId[UMI_DESK_FEDERATION_WORKSPACE_ID_CAPACITY];
    size_t index;
    if (service == NULL || outWorkspace == NULL || DfCopy(safeId, sizeof safeId, id) != UMI_STATUS_OK ||
        !DfIdValid(safeId, sizeof safeId) || (kind != UMI_DESK_WORKSPACE_GROUP && kind != UMI_DESK_WORKSPACE_SESSION))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!service->loaded) return UMI_STATUS_INVALID_STATE;
    index = DfWorkspaceIndex(service->store, kind, safeId);
    return UmiDeskFederationWorkspaceAt(service, index, outWorkspace);
}
UmiStatus UmiDeskFederationSnapshotGet(const UmiDeskFederation *service, UmiDeskFederationSnapshot *outSnapshot)
{
    UmiDeskFederationSnapshot snapshot = {0};
    if (service == NULL || outSnapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    snapshot.loaded = service->loaded; snapshot.revision = service->store->revision;
    snapshot.workspaceCount = service->store->count; snapshot.previewToken = service->plan.token;
    snapshot.activityCount = service->activityCount; snapshot.droppedActivity = service->droppedActivity;
    for (size_t index = 0U; index < service->store->count; ++index) {
        if (service->store->workspaces[index].kind == UMI_DESK_WORKSPACE_GROUP) ++snapshot.groupCount;
        else ++snapshot.sessionCount;
    }
    for (size_t index = 0U; index < service->activityCount; ++index)
        if (!service->activity[(service->activityStart + index) % UMI_DESK_FEDERATION_MAX_ACTIVITY].read) ++snapshot.unreadCount;
    *outSnapshot = snapshot; return UMI_STATUS_OK;
}
UmiStatus UmiDeskFederationCancelPreview(UmiDeskFederation *service)
{
    UmiStatus status = DfEnter(service, false);
    if (status != UMI_STATUS_OK) return status;
    service->plan.token = 0U; service->busy = false; return UMI_STATUS_OK;
}
