/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capability.h
 *
 * PURPOSE:
 *   Define the authoritative inventory and conformance contract for reusable
 *   document I/O, working-copy, command and frontend integration capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITY_H
#define UMICOM_DOCUMENT_CAPABILITY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_API_VERSION 1U
#define UMI_DOCUMENT_CAPABILITY_CAPACITY 192U
#define UMI_DOCUMENT_CAPABILITY_ID_CAPACITY 112U
#define UMI_DOCUMENT_CAPABILITY_TITLE_CAPACITY 112U
#define UMI_DOCUMENT_CAPABILITY_SUMMARY_CAPACITY 384U
#define UMI_DOCUMENT_CAPABILITY_PROVIDER_CAPACITY 96U
#define UMI_DOCUMENT_CAPABILITY_QUERY_CAPACITY 192U
#define UMI_DOCUMENT_CAPABILITY_MESSAGE_CAPACITY 512U

typedef enum UmiDocumentCapabilityCategory {
    UMI_DOCUMENT_CAPABILITY_CATEGORY_LIFECYCLE = 0,
    UMI_DOCUMENT_CAPABILITY_CATEGORY_IDENTITY = 1,
    UMI_DOCUMENT_CAPABILITY_CATEGORY_PROVIDER = 2,
    UMI_DOCUMENT_CAPABILITY_CATEGORY_ENCODING = 3,
    UMI_DOCUMENT_CAPABILITY_CATEGORY_PERSISTENCE = 4,
    UMI_DOCUMENT_CAPABILITY_CATEGORY_RECOVERY = 5,
    UMI_DOCUMENT_CAPABILITY_CATEGORY_CONFLICT = 6,
    UMI_DOCUMENT_CAPABILITY_CATEGORY_COMMAND = 7,
    UMI_DOCUMENT_CAPABILITY_CATEGORY_NAVIGATION = 8,
    UMI_DOCUMENT_CAPABILITY_CATEGORY_SECURITY = 9,
    UMI_DOCUMENT_CAPABILITY_CATEGORY_PERFORMANCE = 10,
    UMI_DOCUMENT_CAPABILITY_CATEGORY_INTEGRATION = 11,
    UMI_DOCUMENT_CAPABILITY_CATEGORY_COUNT = 12
} UmiDocumentCapabilityCategory;

typedef enum UmiDocumentCapabilityMaturity {
    UMI_DOCUMENT_CAPABILITY_MATURITY_PLANNED = 0,
    UMI_DOCUMENT_CAPABILITY_MATURITY_PREVIEW = 1,
    UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE = 2,
    UMI_DOCUMENT_CAPABILITY_MATURITY_DEPRECATED = 3
} UmiDocumentCapabilityMaturity;

typedef enum UmiDocumentCapabilityFlag {
    UMI_DOCUMENT_CAPABILITY_FLAG_NONE = 0U,
    UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED = 1U << 0,
    UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS = 1U << 1,
    UMI_DOCUMENT_CAPABILITY_FLAG_GUI = 1U << 2,
    UMI_DOCUMENT_CAPABILITY_FLAG_PERSISTED = 1U << 3,
    UMI_DOCUMENT_CAPABILITY_FLAG_SECURITY_SENSITIVE = 1U << 4,
    UMI_DOCUMENT_CAPABILITY_FLAG_EXTENSION_POINT = 1U << 5
} UmiDocumentCapabilityFlag;

typedef struct UmiDocumentCapabilityDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    const char *capability_id;
    const char *title;
    UmiDocumentCapabilityCategory category;
    UmiDocumentCapabilityMaturity maturity;
    const char *summary;
    const char *provider_role;
    uint32_t flags;
    int32_t priority;
} UmiDocumentCapabilityDescriptor;

typedef struct UmiDocumentCapabilityCatalogSnapshot {
    size_t total_count;
    size_t category_counts[UMI_DOCUMENT_CAPABILITY_CATEGORY_COUNT];
    size_t implemented_count;
    size_t planned_count;
    size_t headless_count;
    size_t gui_count;
    size_t persisted_count;
    size_t security_sensitive_count;
    uint64_t revision;
} UmiDocumentCapabilityCatalogSnapshot;

typedef struct UmiDocumentCapabilityQueryResult {
    size_t count;
    size_t indices[UMI_DOCUMENT_CAPABILITY_QUERY_CAPACITY];
    uint64_t catalog_revision;
} UmiDocumentCapabilityQueryResult;

typedef struct UmiDocumentCapabilityCatalog UmiDocumentCapabilityCatalog;

UmiStatus umi_document_capability_catalog_create(
    UmiDocumentCapabilityCatalog **out_catalog);
void umi_document_capability_catalog_destroy(
    UmiDocumentCapabilityCatalog *catalog);
UmiStatus umi_document_capability_descriptor_validate(
    const UmiDocumentCapabilityDescriptor *descriptor,
    char *out_message,
    size_t message_capacity);
UmiStatus umi_document_capability_catalog_upsert(
    UmiDocumentCapabilityCatalog *catalog,
    const UmiDocumentCapabilityDescriptor *descriptor);
UmiStatus umi_document_capability_catalog_find(
    const UmiDocumentCapabilityCatalog *catalog,
    const char *capability_id,
    UmiDocumentCapabilityDescriptor *out_descriptor);
UmiStatus umi_document_capability_catalog_at(
    const UmiDocumentCapabilityCatalog *catalog,
    size_t index,
    UmiDocumentCapabilityDescriptor *out_descriptor);
size_t umi_document_capability_catalog_count(
    const UmiDocumentCapabilityCatalog *catalog);
UmiStatus umi_document_capability_catalog_query_category(
    const UmiDocumentCapabilityCatalog *catalog,
    UmiDocumentCapabilityCategory category,
    UmiDocumentCapabilityQueryResult *out_result);
UmiStatus umi_document_capability_catalog_query_flags(
    const UmiDocumentCapabilityCatalog *catalog,
    uint32_t required_flags,
    UmiDocumentCapabilityQueryResult *out_result);
UmiStatus umi_document_capability_catalog_snapshot(
    const UmiDocumentCapabilityCatalog *catalog,
    UmiDocumentCapabilityCatalogSnapshot *out_snapshot);
UmiStatus umi_document_capability_catalog_register_builtins(
    UmiDocumentCapabilityCatalog *catalog);
const char *umi_document_capability_category_text(
    UmiDocumentCapabilityCategory category);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITY_H */
