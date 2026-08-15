/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/resource_broker.c
 *
 * PURPOSE:
 *   Implement bounded Framework-owned resource leasing and conflict checks.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/resource_broker.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/runtime/capability_catalogue.h"

struct UmiApplicationResourceBroker {
    UmiResourceLease leases[UMI_RESOURCE_BROKER_MAX_LEASES];
    uint64_t next_id;
    uint64_t revision;
};

static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL ||
        source[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

static int lease_conflicts(const UmiResourceLease *lease,
                           const UmiResourceLeaseRequest *request)
{
    if (!lease->active || strcmp(lease->resource_id, request->resource_id) != 0)
        return 0;
    return lease->mode == UMI_RESOURCE_LEASE_EXCLUSIVE ||
           request->mode == UMI_RESOURCE_LEASE_EXCLUSIVE;
}

UmiStatus umi_application_resource_broker_create(
    UmiApplicationResourceBroker **out_broker)
{
    if (out_broker == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_broker = (UmiApplicationResourceBroker *)calloc(1U,
                                                         sizeof(**out_broker));
    if (*out_broker == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (*out_broker)->next_id = 1U;
    (*out_broker)->revision = 1U;
    return UMI_STATUS_OK;
}

void umi_application_resource_broker_destroy(
    UmiApplicationResourceBroker *broker)
{
    free(broker);
}

UmiStatus umi_application_resource_broker_acquire(
    UmiApplicationResourceBroker *broker,
    const UmiResourceLeaseRequest *request,
    UmiResourceLease *out_lease)
{
    size_t index;
    UmiResourceLease *slot = NULL;
    UmiStatus status;
    if (broker == NULL || request == NULL || out_lease == NULL ||
        umi_application_definition_validate(request->application) !=
            UMI_STATUS_OK ||
        request->resource_id == NULL || request->capability_id == NULL ||
        request->mode < UMI_RESOURCE_LEASE_SHARED_READ ||
        request->mode > UMI_RESOURCE_LEASE_EXCLUSIVE ||
        request->quota_units == 0U ||
        umi_framework_capability_catalogue_find(request->capability_id) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_application_definition_declares_capability(
            request->application, request->capability_id))
        return UMI_STATUS_PERMISSION_DENIED;
    for (index = 0U; index < UMI_RESOURCE_BROKER_MAX_LEASES; ++index) {
        if (lease_conflicts(&broker->leases[index], request))
            return UMI_STATUS_BUSY;
        if (!broker->leases[index].active && slot == NULL)
            slot = &broker->leases[index];
    }
    if (slot == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memset(slot, 0, sizeof(*slot));
    (void)snprintf(slot->lease_id, sizeof(slot->lease_id), "lease-%llu",
                   (unsigned long long)broker->next_id++);
    status = copy_text(slot->application_id, sizeof(slot->application_id),
                       request->application->application_id);
    if (status == UMI_STATUS_OK)
        status = copy_text(slot->resource_id, sizeof(slot->resource_id),
                           request->resource_id);
    if (status == UMI_STATUS_OK)
        status = copy_text(slot->capability_id, sizeof(slot->capability_id),
                           request->capability_id);
    if (status != UMI_STATUS_OK) {
        (void)memset(slot, 0, sizeof(*slot));
        return status;
    }
    slot->mode = request->mode;
    slot->quota_units = request->quota_units;
    slot->expires_at = request->expires_at;
    slot->revision = ++broker->revision;
    slot->active = 1;
    *out_lease = *slot;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_resource_broker_renew(
    UmiApplicationResourceBroker *broker,
    const char *application_id,
    const char *lease_id,
    uint64_t expires_at,
    UmiResourceLease *out_lease)
{
    size_t index;
    if (broker == NULL || application_id == NULL || lease_id == NULL ||
        out_lease == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < UMI_RESOURCE_BROKER_MAX_LEASES; ++index) {
        UmiResourceLease *lease = &broker->leases[index];
        if (lease->active && strcmp(lease->lease_id, lease_id) == 0) {
            if (strcmp(lease->application_id, application_id) != 0)
                return UMI_STATUS_PERMISSION_DENIED;
            lease->expires_at = expires_at;
            lease->revision = ++broker->revision;
            *out_lease = *lease;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_application_resource_broker_release(
    UmiApplicationResourceBroker *broker,
    const char *application_id,
    const char *lease_id)
{
    size_t index;
    if (broker == NULL || application_id == NULL || lease_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < UMI_RESOURCE_BROKER_MAX_LEASES; ++index) {
        UmiResourceLease *lease = &broker->leases[index];
        if (lease->active && strcmp(lease->lease_id, lease_id) == 0) {
            if (strcmp(lease->application_id, application_id) != 0)
                return UMI_STATUS_PERMISSION_DENIED;
            lease->active = 0;
            lease->revision = ++broker->revision;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_application_resource_broker_find(
    const UmiApplicationResourceBroker *broker,
    const char *lease_id,
    UmiResourceLease *out_lease)
{
    size_t index;
    if (broker == NULL || lease_id == NULL || out_lease == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < UMI_RESOURCE_BROKER_MAX_LEASES; ++index) {
        if (broker->leases[index].active &&
            strcmp(broker->leases[index].lease_id, lease_id) == 0) {
            *out_lease = broker->leases[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

size_t umi_application_resource_broker_active_count(
    const UmiApplicationResourceBroker *broker)
{
    size_t index;
    size_t count = 0U;
    if (broker == NULL) return 0U;
    for (index = 0U; index < UMI_RESOURCE_BROKER_MAX_LEASES; ++index) {
        if (broker->leases[index].active) count += 1U;
    }
    return count;
}
