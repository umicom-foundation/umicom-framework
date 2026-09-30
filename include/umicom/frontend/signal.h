/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/signal.h
 *
 * PURPOSE:
 *   Define signal-to-command bindings for server-side and desktop frontend composition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This contract stores bounded snapshots by value. The registry owns those
 * copies; it does not take ownership of strings or external resources.
 * Coordinate cross-thread mutation at the product/service boundary.
 */
#ifndef UMICOM_FRONTEND_SIGNAL_H
#define UMICOM_FRONTEND_SIGNAL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FRONTEND_SIGNAL_CAPACITY 2048U

/**
 * Represent the frontend signal snapshot data shared with callers of this public contract.
 */
typedef struct UmiFrontendSignalSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char widget_id[128];
    char signal_name[128];
    char command_id[128];
    char argument[512];
    int enabled;
    int once;
    uint64_t revision;
} UmiFrontendSignalSnapshot;

/**
 * Represent the frontend signal registry data shared with callers of this public contract.
 */
typedef struct UmiFrontendSignalRegistry UmiFrontendSignalRegistry;

/**
 * Initialise frontend signal registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_frontend_signal_registry_create(UmiFrontendSignalRegistry **out_registry);
/**
 * Release or reset state held by frontend signal registry so the same storage can be
 * reused safely.
 */
void umi_frontend_signal_registry_destroy(UmiFrontendSignalRegistry *registry);
/**
 * Provide the frontend signal registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_signal_registry_upsert(UmiFrontendSignalRegistry *registry, const UmiFrontendSignalSnapshot *item);
/**
 * Remove frontend signal registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_frontend_signal_registry_remove(UmiFrontendSignalRegistry *registry, const char *id);
/**
 * Find frontend signal registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_frontend_signal_registry_find(const UmiFrontendSignalRegistry *registry, const char *id, UmiFrontendSignalSnapshot *out_item);
/**
 * Find frontend signal registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_frontend_signal_registry_at(const UmiFrontendSignalRegistry *registry, size_t index, UmiFrontendSignalSnapshot *out_item);
/**
 * Return the number of records represented by frontend signal registry without changing
 * their state.
 */
size_t umi_frontend_signal_registry_count(const UmiFrontendSignalRegistry *registry);
/**
 * Provide the frontend signal registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_frontend_signal_registry_revision(const UmiFrontendSignalRegistry *registry);


/** Check id and every fixed text array before lookup/copy. id must be nonempty;
 * other text may be empty. Output is optional and contains field diagnostics,
 * not input text. No state changes or allocations occur. This validates text
 * bounds, not domain semantics or UTF-8. Size/version normalisation is unchanged.
 * The existing registry_upsert operation applies the same check before mutation. */
UmiStatus umi_frontend_signal_snapshot_validate(const UmiFrontendSignalSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Atomically insert/replace up to UMI_FRONTEND_SIGNAL_CAPACITY distinct IDs in memory.
 * Entries retain normal upsert order and revision increments. Duplicate IDs
 * within the batch return ALREADY_EXISTS; IDs already stored may be replaced.
 * Failure publishes no changes. An empty batch succeeds without allocation.
 * Inputs remain caller-owned and unchanged. outResult is optional and must not
 * overlap inputs or registry storage. Keep them stable and serialize registry
 * access on the owning thread. This copies a full registry temporarily and
 * performs no I/O; it is not a filesystem or cross-thread transaction. */
UmiStatus umi_frontend_signal_registry_upsert_many(UmiFrontendSignalRegistry *registry,
    const UmiFrontendSignalSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);

#ifdef __cplusplus
}
#endif

#endif
