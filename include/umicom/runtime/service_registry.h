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


/* Copied metadata contains no service, callback or registry pointers. It remains
 * readable after registry mutation or release; it does not keep a service alive. */
typedef struct UmiServiceSnapshot {
    char service_id[UMI_SERVICE_ID_CAPACITY];
    char provider_module_id[UMI_SERVICE_PROVIDER_CAPACITY];
    UmiVersion service_version;
    uint32_t flags;
} UmiServiceSnapshot;

/* Copy one entry under the registry mutex. NULL provider selects the first
 * matching provider in registration order. A missing entry leaves output
 * unchanged. Keep registry lifetime valid throughout the call. */
UmiStatus UmiServiceRegistryCopySnapshot(const UmiServiceRegistry *registry,
    const char *service_id, const char *provider_module_id,
    UmiServiceSnapshot *out_snapshot);

/* Copy one consistent metadata list in registration order. out_total receives
 * the entire count, including entries beyond capacity. A short non-NULL output
 * receives a valid prefix and CAPACITY_EXCEEDED. NULL output with zero capacity
 * is a successful count query. Errors before copying leave items unchanged and
 * reset a supplied out_total to zero. These calls serialize with registry
 * writes; exclude all concurrent operations before registry release. */
UmiStatus UmiServiceRegistryCopySnapshots(const UmiServiceRegistry *registry,
    UmiServiceSnapshot *out_snapshots, size_t capacity, size_t *out_total);


/* Counts from one locked registry observation; no allocation or service calls. */
typedef struct UmiServiceRegistrySummary {
    size_t service_count;
    size_t singleton_service_count;
    size_t owned_service_count;
    size_t thread_safe_service_count;
    size_t replaceable_service_count;
} UmiServiceRegistrySummary;

UmiStatus UmiServiceRegistryCopySummary(const UmiServiceRegistry *registry,
    UmiServiceRegistrySummary *out_summary);

/* Empty prefix matches all registered providers. Count rows, not distinct
 * service names: two providers of one service contribute two matches.
 * On failure a supplied out_count is reset to zero. */
UmiStatus UmiServiceRegistryCountPrefix(const UmiServiceRegistry *registry,
    const char *prefix, size_t *out_count);

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

/* Checked owner release. Empty owners succeed; destructor reentry is refused
 * without changing the pointer. Exclude all concurrent operations for release;
 * read-only destructor callbacks run under that exclusive owner.
 * Destructors run outside the write mutex and must return normally. During a
 * destructor, registry mutation/release returns INVALID_STATE; read-only
 * discovery can observe the composition after the retired entry was detached. */
UmiStatus UmiServiceRegistryRelease(UmiServiceRegistry **inOutRegistry);

/* Observe the destructor interval while holding a valid owner reference. This
 * is a reentry guard for enclosing owners, not permission to race destruction.
 * NULL returns zero. */
int UmiServiceRegistryIsInCallback(const UmiServiceRegistry *registry);
/**
 * Add service registry only after its inputs and available capacity have been checked.
 */
/* Identifiers are copied, service pointers are borrowed unless OWNED. Replace
 * requires the existing entry to be REPLACEABLE. Replacing with the same object
 * changes metadata without destroying it; the new ownership/destructor applies
 * at later removal. Either provider declaring SINGLETON forbids a second
 * provider. Input may be a currently borrowed descriptor when serialized.
 * Reacquire find/at pointers after mutation; serialize reads against writes. */
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
