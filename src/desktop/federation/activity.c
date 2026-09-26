/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/federation/activity.c
 *
 * PURPOSE:
 *   Expose bounded, honest Desk activity without fabricating process readiness.
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

void DfActivityPush(UmiDeskFederation *service, const UmiDeskFederationApp *app,
    UmiDeskFederationActivityKind kind, UmiStatus status)
{
    UmiDeskFederationActivity entry = {0};
    size_t slot;
    if (service->activitySequence == UINT64_MAX) {
        if (service->droppedActivity < UINT64_MAX) ++service->droppedActivity;
        return;
    }
    entry.sequence = ++service->activitySequence; entry.application = app->id;
    entry.kind = kind; entry.state = app->state; entry.status = status;
    if (service->activityCount == UMI_DESK_FEDERATION_MAX_ACTIVITY) {
        service->activityStart = (service->activityStart + 1U) % UMI_DESK_FEDERATION_MAX_ACTIVITY;
        --service->activityCount;
        if (service->droppedActivity < UINT64_MAX) ++service->droppedActivity;
    }
    slot = (service->activityStart + service->activityCount++) % UMI_DESK_FEDERATION_MAX_ACTIVITY;
    service->activity[slot] = entry;
}
UmiStatus UmiDeskFederationPoll(UmiDeskFederation *service)
{
    UmiDeskFederationCatalogue catalogue;
    UmiStatus status = DfEnter(service, false);
    if (status != UMI_STATUS_OK) return status;
    status = DfCatalogueRead(service, &catalogue);
    if (status == UMI_STATUS_OK) {
        if (service->observed) {
            for (size_t index = 0U; index < catalogue.count; ++index) {
                const UmiDeskFederationApp *app = &catalogue.apps[index];
                const UmiDeskFederationApp *old = DfAppFind(&service->baseline, app->id.value);
                if (strcmp(app->id.value, service->hostId) != 0 && (old == NULL || !DfAppSame(old, app)))
                    DfActivityPush(service, app, UMI_DESK_ACTIVITY_OBSERVED, UMI_STATUS_OK);
            }
            for (size_t index = 0U; index < service->baseline.count; ++index) {
                const UmiDeskFederationApp *old = &service->baseline.apps[index];
                if (strcmp(old->id.value, service->hostId) != 0 && DfAppFind(&catalogue, old->id.value) == NULL) {
                    UmiDeskFederationApp missing = *old;
                    missing.state = UMI_DESK_FEDERATION_UNKNOWN; missing.running = false;
                    DfActivityPush(service, &missing, UMI_DESK_ACTIVITY_OBSERVED, UMI_STATUS_NOT_FOUND);
                }
            }
        }
        service->baseline = catalogue; service->observed = true;
    }
    service->busy = false; return status;
}
UmiStatus UmiDeskFederationActivityAt(const UmiDeskFederation *service, size_t index, UmiDeskFederationActivity *outActivity)
{
    if (service == NULL || outActivity == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= service->activityCount) return UMI_STATUS_NOT_FOUND;
    /* Most recent first; array slots are never exposed to callers. */
    *outActivity = service->activity[(service->activityStart + service->activityCount - 1U - index) % UMI_DESK_FEDERATION_MAX_ACTIVITY];
    return UMI_STATUS_OK;
}
UmiStatus UmiDeskFederationAcknowledgeAll(UmiDeskFederation *service)
{
    UmiStatus status = DfEnter(service, false);
    if (status != UMI_STATUS_OK) return status;
    for (size_t index = 0U; index < service->activityCount; ++index)
        service->activity[(service->activityStart + index) % UMI_DESK_FEDERATION_MAX_ACTIVITY].read = true;
    service->busy = false; return UMI_STATUS_OK;
}
