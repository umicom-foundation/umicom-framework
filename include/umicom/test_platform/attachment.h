/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/attachment.h
 *
 * PURPOSE:
 *   Define a reusable test-explorer and test-run record independent of any single test framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_TEST_PLATFORM_ATTACHMENT_H
#define UMICOM_TEST_PLATFORM_ATTACHMENT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_PLATFORM_ATTACHMENT_CAPACITY 4096U
#define UMI_TEST_PLATFORM_ATTACHMENT_API_VERSION 2U

/**
 * Represent the test platform attachment snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformAttachmentSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char result_id[128];
    char name[256];
    char kind[64];
    char producer[128];
    char uri[1024];
    char mime_type[128];
    char schema_uri[1024];
    char checksum[128];
    uint64_t size_bytes;
    uint64_t revision;
} UmiTestPlatformAttachmentSnapshot;

/**
 * Represent the test platform attachment registry data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformAttachmentRegistry UmiTestPlatformAttachmentRegistry;

/**
 * Initialise test platform attachment registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_test_platform_attachment_registry_create(UmiTestPlatformAttachmentRegistry **out_registry);
/**
 * Release or reset state held by test platform attachment registry so the same storage can
 * be reused safely.
 */
void umi_test_platform_attachment_registry_destroy(UmiTestPlatformAttachmentRegistry *registry);
/**
 * Provide the test platform attachment registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_platform_attachment_registry_upsert(UmiTestPlatformAttachmentRegistry *registry, const UmiTestPlatformAttachmentSnapshot *item);
/**
 * Remove test platform attachment registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_test_platform_attachment_registry_remove(UmiTestPlatformAttachmentRegistry *registry, const char *id);
/**
 * Find test platform attachment registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_test_platform_attachment_registry_find(const UmiTestPlatformAttachmentRegistry *registry, const char *id, UmiTestPlatformAttachmentSnapshot *out_item);
/**
 * Find test platform attachment registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_test_platform_attachment_registry_at(const UmiTestPlatformAttachmentRegistry *registry, size_t index, UmiTestPlatformAttachmentSnapshot *out_item);
/**
 * Return the number of records represented by test platform attachment registry without
 * changing their state.
 */
size_t umi_test_platform_attachment_registry_count(const UmiTestPlatformAttachmentRegistry *registry);
/**
 * Provide the test platform attachment registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_test_platform_attachment_registry_revision(const UmiTestPlatformAttachmentRegistry *registry);
/**
 * Release or reset state held by test platform attachment registry so the same storage can
 * be reused safely.
 */
void umi_test_platform_attachment_registry_clear(UmiTestPlatformAttachmentRegistry *registry);


/** Check id and all fixed text arrays before string lookup or publication.
 * id must be nonempty; optional text may be empty. Unterminated arrays return
 * INVALID_ARGUMENT with optional static field diagnostics. No allocation or
 * mutation occurs. This checks text bounds, not UTF-8 or domain semantics.
 * Single-record upsert uses the same preflight and rejects revision overflow. */
UmiStatus umi_test_platform_attachment_snapshot_validate(const UmiTestPlatformAttachmentSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Insert/replace up to UMI_TEST_PLATFORM_ATTACHMENT_CAPACITY distinct IDs as one in-memory operation.
 * Duplicate input IDs are rejected; stored IDs can be replaced. Valid records
 * retain ordinary upsert order, normalisation and revision increments. Any
 * failure leaves the live registry unchanged. Empty input is a successful no-op.
 * A full private registry is allocated temporarily. No files or callbacks are
 * used. Keep all access on the owner's thread; this is not thread locking.
 * Inputs are borrowed for this call, unchanged and never retained. outResult
 * is optional, written on every return, and must not overlap input or registry. */
UmiStatus umi_test_platform_attachment_registry_upsert_many(UmiTestPlatformAttachmentRegistry *registry,
    const UmiTestPlatformAttachmentSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_test_platform_attachment_registry_capture(const UmiTestPlatformAttachmentRegistry *registry,
    UmiTestPlatformAttachmentSnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

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
UmiStatus umi_test_platform_attachment_registry_replace_if_current(UmiTestPlatformAttachmentRegistry *registry,
    uint64_t expected_revision, const UmiTestPlatformAttachmentSnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
