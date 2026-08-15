/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/resource_broker.h
 *
 * PURPOSE:
 *   Broker shared and exclusive resource leases through Framework so
 *   applications never exchange device, model, document or service handles
 *   directly.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_RESOURCE_BROKER_H
#define UMICOM_APPLICATION_RESOURCE_BROKER_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/application/definition.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_RESOURCE_BROKER_TEXT_CAPACITY 128U
#define UMI_RESOURCE_BROKER_MAX_LEASES 256U

typedef enum UmiResourceLeaseMode {
    UMI_RESOURCE_LEASE_SHARED_READ = 1,
    UMI_RESOURCE_LEASE_SHARED_WRITE = 2,
    UMI_RESOURCE_LEASE_EXCLUSIVE = 3
} UmiResourceLeaseMode;

typedef struct UmiResourceLeaseRequest {
    const UmiApplicationDefinition *application;
    const char *resource_id;
    const char *capability_id;
    UmiResourceLeaseMode mode;
    uint64_t quota_units;
    uint64_t expires_at;
} UmiResourceLeaseRequest;

typedef struct UmiResourceLease {
    char lease_id[UMI_RESOURCE_BROKER_TEXT_CAPACITY];
    char application_id[UMI_RESOURCE_BROKER_TEXT_CAPACITY];
    char resource_id[UMI_RESOURCE_BROKER_TEXT_CAPACITY];
    char capability_id[UMI_RESOURCE_BROKER_TEXT_CAPACITY];
    UmiResourceLeaseMode mode;
    uint64_t quota_units;
    uint64_t expires_at;
    uint64_t revision;
    int active;
} UmiResourceLease;

typedef struct UmiApplicationResourceBroker UmiApplicationResourceBroker;

UmiStatus umi_application_resource_broker_create(
    UmiApplicationResourceBroker **out_broker);
void umi_application_resource_broker_destroy(
    UmiApplicationResourceBroker *broker);
UmiStatus umi_application_resource_broker_acquire(
    UmiApplicationResourceBroker *broker,
    const UmiResourceLeaseRequest *request,
    UmiResourceLease *out_lease);
UmiStatus umi_application_resource_broker_renew(
    UmiApplicationResourceBroker *broker,
    const char *application_id,
    const char *lease_id,
    uint64_t expires_at,
    UmiResourceLease *out_lease);
UmiStatus umi_application_resource_broker_release(
    UmiApplicationResourceBroker *broker,
    const char *application_id,
    const char *lease_id);
UmiStatus umi_application_resource_broker_find(
    const UmiApplicationResourceBroker *broker,
    const char *lease_id,
    UmiResourceLease *out_lease);
size_t umi_application_resource_broker_active_count(
    const UmiApplicationResourceBroker *broker);

#ifdef __cplusplus
}
#endif

#endif
