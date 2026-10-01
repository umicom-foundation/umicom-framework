/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/resource_location.h
 *
 * PURPOSE:
 *   Define normalised local and remote resource locations and the per-user
 *   application directories used by Umicom products.  Applications can keep
 *   settings, recovery files, caches and other writable state away from their
 *   installation folder and away from whichever directory happened to launch
 *   the process.
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
#ifndef UMICOM_PLATFORM_RESOURCE_LOCATION_H
#define UMICOM_PLATFORM_RESOURCE_LOCATION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"
#include "umicom/platform/path.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PLATFORM_RESOURCE_LOCATION_CAPACITY 1024U
#define UMI_APPLICATION_PATHS_API_VERSION 1U

/**
 * Represent the resource location snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiResourceLocationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char uri[1024];
    char display_name[256];
    char scheme[64];
    char authority[256];
    char path[1024];
    int local;
    int writable;
    int available;
    uint64_t revision;
} UmiResourceLocationSnapshot;

/**
 * Represent the resource location registry data shared with callers of this public
 * contract.
 */
typedef struct UmiResourceLocationRegistry UmiResourceLocationRegistry;

/**
 * Initialise platform resource location registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_platform_resource_location_registry_create(UmiResourceLocationRegistry **out_registry);
/**
 * Release or reset state held by platform resource location registry so the same storage
 * can be reused safely.
 */
void umi_platform_resource_location_registry_destroy(UmiResourceLocationRegistry *registry);
/**
 * Provide the platform resource location registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_platform_resource_location_registry_upsert(UmiResourceLocationRegistry *registry, const UmiResourceLocationSnapshot *item);
/**
 * Remove platform resource location registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_platform_resource_location_registry_remove(UmiResourceLocationRegistry *registry, const char *id);
/**
 * Find platform resource location registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_platform_resource_location_registry_find(const UmiResourceLocationRegistry *registry, const char *id, UmiResourceLocationSnapshot *out_item);
/**
 * Find platform resource location registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_platform_resource_location_registry_at(const UmiResourceLocationRegistry *registry, size_t index, UmiResourceLocationSnapshot *out_item);
/**
 * Return the number of records represented by platform resource location registry without
 * changing their state.
 */
size_t umi_platform_resource_location_registry_count(const UmiResourceLocationRegistry *registry);
/**
 * Provide the platform resource location registry revision operation used by this module
 * and its client applications.
 */
uint64_t umi_platform_resource_location_registry_revision(const UmiResourceLocationRegistry *registry);

/*
 * Writable application paths are an additive Framework capability.  The
 * resource-location API above is intentionally left unchanged so existing
 * applications retain their established public contract.
 */
#define UMI_APPLICATION_PATHS_API_VERSION 1U

/**
 * Input used to resolve writable application directories.
 *
 * baseOverride is intended for isolated tests, portable developer runs and
 * controlled hosts.  When it is NULL, Framework uses the operating system's
 * normal per-user application-data locations.  A supplied override must be an
 * absolute path so the result can never depend on the process working
 * directory.
 */
typedef struct UmiApplicationPathsConfig {
    uint32_t structSize;
    uint32_t apiVersion;
    const char *organisationDirectory;
    const char *applicationDirectory;
    const char *baseOverride;
} UmiApplicationPathsConfig;

/**
 * Writable directories owned by one Umicom application.
 *
 * On Windows these directories live below the current user's LOCALAPPDATA
 * folder.  On Unix-like systems Framework follows the XDG configuration,
 * data, state and cache locations when they are available.  No member is
 * resolved from the current working directory.
 */
typedef struct UmiApplicationPaths {
    uint32_t structSize;
    uint32_t apiVersion;
    char root[UMI_PATH_CAPACITY];
    char config[UMI_PATH_CAPACITY];
    char state[UMI_PATH_CAPACITY];
    char cache[UMI_PATH_CAPACITY];
    char data[UMI_PATH_CAPACITY];
    char logs[UMI_PATH_CAPACITY];
    char recovery[UMI_PATH_CAPACITY];
} UmiApplicationPaths;

/**
 * Create a default application-path request for an Umicom product.
 *
 * applicationDirectory is a directory name such as "Studio" or "Trader".
 * The returned configuration uses "Umicom" as the organisation directory and
 * leaves baseOverride unset.
 */
UmiApplicationPathsConfig UmiApplicationPathsConfigDefault(
    const char *applicationDirectory);

/**
 * Resolve the current user's writable locations without creating directories.
 */
UmiStatus UmiApplicationPathsResolve(
    const UmiApplicationPathsConfig *config,
    UmiApplicationPaths *outPaths);

/**
 * Create the resolved writable directories when they do not already exist.
 */
UmiStatus UmiApplicationPathsPrepare(
    const UmiApplicationPaths *paths);


/** Check id and every fixed text array before lookup/copy. id must be nonempty;
 * other text may be empty. Output is optional and contains field diagnostics,
 * not input text. No state changes or allocations occur. This validates text
 * bounds, not domain semantics or UTF-8. Size/version normalisation is unchanged.
 * The existing registry_upsert operation applies the same check before mutation. */
UmiStatus umi_platform_resource_location_snapshot_validate(const UmiResourceLocationSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Atomically insert/replace up to UMI_PLATFORM_RESOURCE_LOCATION_CAPACITY distinct IDs in memory.
 * Entries retain normal upsert order and revision increments. Duplicate IDs
 * within the batch return ALREADY_EXISTS; IDs already stored may be replaced.
 * Failure publishes no changes. An empty batch succeeds without allocation.
 * Inputs remain caller-owned and unchanged. outResult is optional and must not
 * overlap inputs or registry storage. Keep them stable and serialize registry
 * access on the owning thread. This copies a full registry temporarily and
 * performs no I/O; it is not a filesystem or cross-thread transaction. */
UmiStatus umi_platform_resource_location_registry_upsert_many(UmiResourceLocationRegistry *registry,
    const UmiResourceLocationSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_platform_resource_location_registry_capture(const UmiResourceLocationRegistry *registry,
    UmiResourceLocationSnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

/** Replace the complete collection only while expected_revision still matches
 * this registry. Use a revision from capture, not a different registry. A stale
 * proposal returns INVALID_STATE without changing records. IDs must be unique;
 * an empty proposal clears the collection, except an already-empty collection
 * needs no change. Successful publication advances the revision once and gives
 * every stored row that revision. Revision exhaustion returns CAPACITY_EXCEEDED.
 * Existing upsert normalisation remains in effect. Any validation/allocation
 * failure retains the complete previous collection. Inputs and optional result
 * must not overlap each other or registry storage; serialize owner access.
 * This is an in-memory publication, not a thread lock or a durable disk save. */
UmiStatus umi_platform_resource_location_registry_replace_if_current(UmiResourceLocationRegistry *registry,
    uint64_t expected_revision, const UmiResourceLocationSnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);


/** One reviewed change. For REMOVE, initialise item.id; its other fields are
 * ignored. For UPSERT, supply a complete record under the usual domain rules.
 * The edit remains caller-owned and is never changed by publication. */
typedef struct UmiResourceLocationEdit {
    UmiSnapshotEditKind kind;
    UmiResourceLocationSnapshot item;
} UmiResourceLocationEdit;

/** Apply this ordered list only if expected_revision still matches this owner.
 * A stale review returns INVALID_STATE, including for an empty list. Each ID
 * may appear once; duplicates return ALREADY_EXISTS. A missing removal returns
 * NOT_FOUND. Upserts retain normal field normalisation and capacity checks;
 * when full, remove an existing row before adding its replacement.
 * At most twice UMI_PLATFORM_RESOURCE_LOCATION_CAPACITY edits are accepted. Success publishes
 * all changes together, advances once, and stamps every remaining row with
 * that revision. An empty current list is a no-op. Every refusal preserves
 * the previous collection. out_result is optional and identifies a rejected
 * edit where possible; validation describes text and duplicate-ID errors.
 * Inputs/result must not overlap each other or the owner. Serialize access;
 * staging allocates one registry and performs no I/O or external callbacks. */
UmiStatus umi_platform_resource_location_registry_edit_if_current(UmiResourceLocationRegistry *registry,
    uint64_t expected_revision, const UmiResourceLocationEdit *edits, size_t count,
    UmiSnapshotBatchResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
