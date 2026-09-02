/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/service_registry.h
 *
 * PURPOSE:
 *   Provide an explicit, versioned registry for shared service instances and
 *   their ownership so modules receive declared dependencies instead of using
 *   global variables or accessing another module's private state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_SERVICE_REGISTRY_H
#define UMICOM_RUNTIME_SERVICE_REGISTRY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/base/version.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SERVICE_ID_CAPACITY 128U
#define UMI_SERVICE_PROVIDER_CAPACITY 128U
#define UMI_SERVICE_REGISTRY_MAX 512U

/**
 * List the named service flags values accepted by this public contract.
 */
typedef enum UmiServiceFlags {
    UMI_SERVICE_NONE = 0U,
    UMI_SERVICE_SINGLETON = 1U << 0,
    UMI_SERVICE_OWNED = 1U << 1,
    UMI_SERVICE_THREAD_SAFE = 1U << 2,
    UMI_SERVICE_REPLACEABLE = 1U << 3
} UmiServiceFlags;

typedef void (*UmiServiceDestroyFn)(void *service, void *user_data);

/**
 * Represent the service descriptor data shared with callers of this public contract.
 */
typedef struct UmiServiceDescriptor {
    uint32_t structure_size;
    const char *service_id;
    UmiVersion service_version;
    const char *provider_module_id;
    void *service;
    UmiServiceDestroyFn destroy;
    void *destroy_user_data;
    uint32_t flags;
} UmiServiceDescriptor;

/**
 * Represent the service registry data shared with callers of this public contract.
 */
typedef struct UmiServiceRegistry UmiServiceRegistry;

/**
 * Initialise service registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_service_registry_create(UmiServiceRegistry **out_registry);
/**
 * Release or reset state held by service registry so the same storage can be reused
 * safely.
 */
void umi_service_registry_destroy(UmiServiceRegistry *registry);
/**
 * Add service registry only after its inputs and available capacity have been checked.
 */
UmiStatus umi_service_registry_register(UmiServiceRegistry *registry,
                                        const UmiServiceDescriptor *descriptor);
/**
 * Remove service registry while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_service_registry_unregister(UmiServiceRegistry *registry,
                                          const char *service_id,
                                          const char *provider_module_id);
/**
 * Find service registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiServiceDescriptor *umi_service_registry_find(
    const UmiServiceRegistry *registry,
    const char *service_id
);
/**
 * Provide the service registry find provider operation used by this module and its client
 * applications.
 */
const UmiServiceDescriptor *umi_service_registry_find_provider(
    const UmiServiceRegistry *registry,
    const char *service_id,
    const char *provider_module_id
);
/**
 * Return the number of records represented by service registry without changing their
 * state.
 */
size_t umi_service_registry_count(const UmiServiceRegistry *registry);
/**
 * Find service registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiServiceDescriptor *umi_service_registry_at(
    const UmiServiceRegistry *registry,
    size_t index
);

#ifdef __cplusplus
}
#endif

#endif
