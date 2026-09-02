/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/resource_broker.h
 *
 * PURPOSE:
 *   Broker shared and exclusive resource leases through Framework so
 *   applications never exchange device, model, document or service handles
 *   directly.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * List the named resource lease mode values accepted by this public contract.
 */
typedef enum UmiResourceLeaseMode {
    UMI_RESOURCE_LEASE_SHARED_READ = 1,
    UMI_RESOURCE_LEASE_SHARED_WRITE = 2,
    UMI_RESOURCE_LEASE_EXCLUSIVE = 3
} UmiResourceLeaseMode;

/**
 * Represent the resource lease request data shared with callers of this public contract.
 */
typedef struct UmiResourceLeaseRequest {
    const UmiApplicationDefinition *application;
    const char *resource_id;
    const char *capability_id;
    UmiResourceLeaseMode mode;
    uint64_t quota_units;
    uint64_t expires_at;
} UmiResourceLeaseRequest;

/**
 * Represent the resource lease data shared with callers of this public contract.
 */
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

/**
 * Represent the application resource broker data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationResourceBroker UmiApplicationResourceBroker;

/**
 * Initialise application resource broker from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_application_resource_broker_create(
    UmiApplicationResourceBroker **out_broker);
/**
 * Release or reset state held by application resource broker so the same storage can be
 * reused safely.
 */
void umi_application_resource_broker_destroy(
    UmiApplicationResourceBroker *broker);
/**
 * Provide the application resource broker acquire operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_resource_broker_acquire(
    UmiApplicationResourceBroker *broker,
    const UmiResourceLeaseRequest *request,
    UmiResourceLease *out_lease);
/**
 * Provide the application resource broker renew operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_resource_broker_renew(
    UmiApplicationResourceBroker *broker,
    const char *application_id,
    const char *lease_id,
    uint64_t expires_at,
    UmiResourceLease *out_lease);
/**
 * Release or reset state held by application resource broker so the same storage can be
 * reused safely.
 */
UmiStatus umi_application_resource_broker_release(
    UmiApplicationResourceBroker *broker,
    const char *application_id,
    const char *lease_id);
/**
 * Find application resource broker while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_application_resource_broker_find(
    const UmiApplicationResourceBroker *broker,
    const char *lease_id,
    UmiResourceLease *out_lease);
/**
 * Return the number of records represented by application resource broker active without
 * changing their state.
 */
size_t umi_application_resource_broker_active_count(
    const UmiApplicationResourceBroker *broker);

#ifdef __cplusplus
}
#endif

#endif
