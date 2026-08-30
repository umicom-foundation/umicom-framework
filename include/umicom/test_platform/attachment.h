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

/* BEGINNER NOTE:
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

typedef struct UmiTestPlatformAttachmentRegistry UmiTestPlatformAttachmentRegistry;

UmiStatus umi_test_platform_attachment_registry_create(UmiTestPlatformAttachmentRegistry **out_registry);
void umi_test_platform_attachment_registry_destroy(UmiTestPlatformAttachmentRegistry *registry);
UmiStatus umi_test_platform_attachment_registry_upsert(UmiTestPlatformAttachmentRegistry *registry, const UmiTestPlatformAttachmentSnapshot *item);
UmiStatus umi_test_platform_attachment_registry_remove(UmiTestPlatformAttachmentRegistry *registry, const char *id);
UmiStatus umi_test_platform_attachment_registry_find(const UmiTestPlatformAttachmentRegistry *registry, const char *id, UmiTestPlatformAttachmentSnapshot *out_item);
UmiStatus umi_test_platform_attachment_registry_at(const UmiTestPlatformAttachmentRegistry *registry, size_t index, UmiTestPlatformAttachmentSnapshot *out_item);
size_t umi_test_platform_attachment_registry_count(const UmiTestPlatformAttachmentRegistry *registry);
uint64_t umi_test_platform_attachment_registry_revision(const UmiTestPlatformAttachmentRegistry *registry);
void umi_test_platform_attachment_registry_clear(UmiTestPlatformAttachmentRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
