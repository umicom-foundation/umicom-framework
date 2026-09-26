/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/federation/planning.c
 *
 * PURPOSE:
 *   Make immutable, single-use reviews and recheck them before host dispatch.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <string.h>

UmiDeskFederationPlanItem DfClassify(UmiDeskFederation *service,
    const UmiDeskFederationCatalogue *catalogue, const UmiDeskFederationAppId *member)
{
    UmiDeskFederationPlanItem item = {0};
    const UmiDeskFederationApp *app = DfAppFind(catalogue, member->value);
    item.app.id = *member;
    (void)DfCopy(item.app.title, sizeof item.app.title, member->value);
    item.reason = UMI_DESK_FEDERATION_NOT_REGISTERED;
    if (app == NULL) return item;
    item.app = *app;
    if (strcmp(member->value, service->hostId) == 0) item.reason = UMI_DESK_FEDERATION_SELF;
    else if (!app->installed) item.reason = UMI_DESK_FEDERATION_NOT_INSTALLED;
    else if (!app->compatible) item.reason = UMI_DESK_FEDERATION_INCOMPATIBLE;
    else if (!app->enabled) item.reason = UMI_DESK_FEDERATION_DISABLED;
    else if (app->state == UMI_DESK_FEDERATION_STARTING || app->state == UMI_DESK_FEDERATION_STOPPING)
        item.reason = UMI_DESK_FEDERATION_TRANSITIONING;
    else if (app->state == UMI_DESK_FEDERATION_UNKNOWN) item.reason = UMI_DESK_FEDERATION_UNKNOWN_STATE;
    else { item.reason = UMI_DESK_FEDERATION_READY; item.action = app->running ? UMI_DESK_FEDERATION_ACTIVATE : UMI_DESK_FEDERATION_START; }
    return item;
}
UmiStatus DfPrepare(UmiDeskFederation *service, const UmiDeskFederationAppId *members,
    size_t count, UmiDeskFederationPlan *outPlan)
{
    UmiDeskFederationCatalogue catalogue;
    UmiDeskFederationPlan plan = {0};
    UmiStatus status;
    service->plan.token = 0U;
    if (outPlan == NULL || members == NULL || count == 0U || count > UMI_DESK_FEDERATION_MAX_APPS)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (service->nextToken == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t index = 0U; index < count; ++index)
        if (!DfIdValid(members[index].value, sizeof members[index].value)) return UMI_STATUS_INVALID_ARGUMENT;
    status = DfRepositoryCheck(service);
    if (status == UMI_STATUS_OK) status = DfCatalogueRead(service, &catalogue);
    if (status != UMI_STATUS_OK) return status;
    plan.count = count; plan.workspaceRevision = service->store->revision;
    for (size_t index = 0U; index < count; ++index) plan.items[index] = DfClassify(service, &catalogue, &members[index]);
    plan.token = ++service->nextToken; service->plan = plan; *outPlan = plan;
    return UMI_STATUS_OK;
}
UmiStatus UmiDeskFederationPrepareWorkspace(UmiDeskFederation *service, UmiDeskWorkspaceKind kind,
    const char *id, UmiDeskFederationPlan *outPlan)
{
    UmiDeskWorkspace workspace;
    UmiStatus status = DfEnter(service, true);
    if (status != UMI_STATUS_OK) return status;
    service->plan.token = 0U;
    status = UmiDeskFederationWorkspaceFind(service, kind, id, &workspace);
    if (status == UMI_STATUS_OK) status = DfPrepare(service, workspace.members, workspace.memberCount, outPlan);
    service->busy = false; return status;
}
UmiStatus UmiDeskFederationPrepareApplication(UmiDeskFederation *service,
    const char *applicationId, UmiDeskFederationPlan *outPlan)
{
    UmiDeskFederationAppId member = {{0}};
    UmiStatus status = DfEnter(service, true);
    if (status != UMI_STATUS_OK) return status;
    service->plan.token = 0U;
    status = DfCopy(member.value, sizeof member.value, applicationId);
    if (status == UMI_STATUS_OK) status = DfPrepare(service, &member, 1U, outPlan);
    service->busy = false; return status;
}
UmiStatus UmiDeskFederationPrepareLink(UmiDeskFederation *service, const char *text,
    UmiDeskFederationPlan *outPlan)
{
    UmiDeskFederationLink link;
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (service->busy) return UMI_STATUS_BUSY;
    service->plan.token = 0U;
    status = UmiDeskFederationLinkParse(text, &link);
    if (status != UMI_STATUS_OK) return status;
    if (link.kind == UMI_DESK_LINK_APPLICATION) return UmiDeskFederationPrepareApplication(service, link.id, outPlan);
    return UmiDeskFederationPrepareWorkspace(service,
        link.kind == UMI_DESK_LINK_GROUP ? UMI_DESK_WORKSPACE_GROUP : UMI_DESK_WORKSPACE_SESSION, link.id, outPlan);
}
UmiStatus UmiDeskFederationExecute(UmiDeskFederation *service, uint64_t token, UmiDeskFederationReport *outReport)
{
    UmiDeskFederationCatalogue catalogue;
    UmiStatus status, firstFailure = UMI_STATUS_OK;
    if (outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(outReport, 0, sizeof *outReport);
    status = DfEnter(service, true);
    if (status != UMI_STATUS_OK) return status;
    if (token == 0U || token != service->plan.token) { service->busy = false; return UMI_STATUS_INVALID_STATE; }
    service->plan.token = 0U; /* Consume before calling anything outside this service. */
    status = DfRepositoryCheck(service);
    if (status == UMI_STATUS_OK) status = DfCatalogueRead(service, &catalogue);
    for (size_t index = 0U; status == UMI_STATUS_OK && index < service->plan.count; ++index) {
        const UmiDeskFederationPlanItem *old = &service->plan.items[index];
        const UmiDeskFederationPlanItem current = DfClassify(service, &catalogue, &old->app.id);
        if (current.action != old->action || current.reason != old->reason || !DfAppSame(&old->app, &current.app))
            status = UMI_STATUS_BUSY;
    }
    if (status == UMI_STATUS_OK) {
        for (size_t index = 0U; index < service->plan.count; ++index) {
            const UmiDeskFederationPlanItem *item = &service->plan.items[index];
            UmiDeskFederationResult *result = &outReport->results[outReport->count++];
            result->id = item->app.id; result->action = item->action;
            if (item->action == UMI_DESK_FEDERATION_SKIP) {
                result->status = UMI_STATUS_UNAVAILABLE; ++outReport->skippedCount;
                DfActivityPush(service, &item->app, UMI_DESK_ACTIVITY_SKIPPED, result->status);
            } else {
                result->status = service->ports.request(service->ports.context, &item->app, item->action);
                if (result->status == UMI_STATUS_OK) ++outReport->acceptedCount;
                else ++outReport->failedCount;
                DfActivityPush(service, &item->app, result->status == UMI_STATUS_OK ?
                    UMI_DESK_ACTIVITY_REQUEST_ACCEPTED : UMI_DESK_ACTIVITY_REQUEST_FAILED, result->status);
            }
            if (firstFailure == UMI_STATUS_OK && result->status != UMI_STATUS_OK) firstFailure = result->status;
        }
        status = firstFailure;
    }
    service->busy = false; return status;
}
