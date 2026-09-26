/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/federation/workspaces.c
 *
 * PURPOSE:
 *   Save named groups and running-application sessions with optimistic concurrency.
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

UmiStatus DfSave(UmiDeskFederation *service, const UmiDeskWorkspace *workspace)
{
    DfStore *candidate;
    size_t index;
    UmiStatus status = DfWorkspaceValidate(workspace);
    if (status != UMI_STATUS_OK) return status;
    for (size_t member = 0U; member < workspace->memberCount; ++member)
        if (strcmp(workspace->members[member].value, service->hostId) == 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (service->store->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    index = DfWorkspaceIndex(service->store, workspace->kind, workspace->id);
    if (index == service->store->count && index == UMI_DESK_FEDERATION_MAX_WORKSPACES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    candidate = malloc(sizeof *candidate);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *candidate = *service->store;
    candidate->workspaces[index] = *workspace;
    if (index == candidate->count) ++candidate->count;
    ++candidate->revision;
    status = DfRepositoryCommit(service, candidate);
    if (status == UMI_STATUS_OK) { DfStore *old = service->store; service->store = candidate; candidate = old; }
    free(candidate); return status;
}
UmiStatus UmiDeskFederationSaveWorkspace(UmiDeskFederation *service, const UmiDeskWorkspace *workspace)
{
    UmiStatus status = DfEnter(service, true);
    if (status != UMI_STATUS_OK) return status;
    service->plan.token = 0U;
    status = DfSave(service, workspace);
    service->busy = false; return status;
}
UmiStatus UmiDeskFederationRemoveWorkspace(UmiDeskFederation *service, UmiDeskWorkspaceKind kind, const char *id)
{
    char safeId[UMI_DESK_FEDERATION_WORKSPACE_ID_CAPACITY];
    DfStore *candidate;
    size_t index;
    UmiStatus status = DfEnter(service, true);
    if (status != UMI_STATUS_OK) return status;
    service->plan.token = 0U;
    if (DfCopy(safeId, sizeof safeId, id) != UMI_STATUS_OK || !DfIdValid(safeId, sizeof safeId) ||
        (kind != UMI_DESK_WORKSPACE_GROUP && kind != UMI_DESK_WORKSPACE_SESSION)) status = UMI_STATUS_INVALID_ARGUMENT;
    else {
        index = DfWorkspaceIndex(service->store, kind, safeId);
        if (index == service->store->count) status = UMI_STATUS_NOT_FOUND;
        else if (service->store->revision == UINT64_MAX) status = UMI_STATUS_CAPACITY_EXCEEDED;
        else {
            candidate = malloc(sizeof *candidate);
            if (candidate == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
            else {
                *candidate = *service->store;
                for (size_t next = index + 1U; next < candidate->count; ++next)
                    candidate->workspaces[next - 1U] = candidate->workspaces[next];
                --candidate->count; ++candidate->revision;
                memset(&candidate->workspaces[candidate->count], 0, sizeof candidate->workspaces[0]);
                status = DfRepositoryCommit(service, candidate);
                if (status == UMI_STATUS_OK) { DfStore *old = service->store; service->store = candidate; candidate = old; }
                free(candidate);
            }
        }
    }
    service->busy = false; return status;
}
UmiStatus UmiDeskFederationCaptureSession(UmiDeskFederation *service, const char *id, const char *title)
{
    UmiDeskFederationCatalogue catalogue;
    UmiDeskWorkspace workspace = {0};
    UmiStatus status = DfEnter(service, true);
    if (status != UMI_STATUS_OK) return status;
    service->plan.token = 0U; workspace.kind = UMI_DESK_WORKSPACE_SESSION;
    status = DfCopy(workspace.id, sizeof workspace.id, id);
    if (status == UMI_STATUS_OK) status = DfCopy(workspace.title, sizeof workspace.title, title);
    if (status == UMI_STATUS_OK) status = DfCatalogueRead(service, &catalogue);
    if (status == UMI_STATUS_OK) {
        for (size_t index = 0U; index < catalogue.count; ++index) {
            const UmiDeskFederationApp *app = &catalogue.apps[index];
            if (app->running && app->state != UMI_DESK_FEDERATION_STARTING && app->state != UMI_DESK_FEDERATION_STOPPING &&
                strcmp(app->id.value, service->hostId) != 0)
                workspace.members[workspace.memberCount++] = app->id;
        }
        status = workspace.memberCount == 0U ? UMI_STATUS_INVALID_STATE : DfSave(service, &workspace);
    }
    service->busy = false; return status;
}
