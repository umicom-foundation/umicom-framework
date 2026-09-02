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

#ifdef __cplusplus
}
#endif

#endif
