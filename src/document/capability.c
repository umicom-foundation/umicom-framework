/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capability.c
 *
 * PURPOSE:
 *   Implement the owned document capability catalogue, validation, queries and
 *   delivery snapshots used by every Umicom application and extension host.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capability.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiDocumentCapabilityCatalog {
    UmiDocumentCapabilityDescriptor items[UMI_DOCUMENT_CAPABILITY_CAPACITY];
    size_t count;
    uint64_t revision;
};

static void set_message(char *out_message, size_t capacity, const char *message)
{
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "%s", message != NULL ? message : "");
    }
}

static int text_valid(const char *text, size_t capacity)
{
    return text != NULL && text[0] != '\0' && strlen(text) < capacity;
}

static size_t find_index(const UmiDocumentCapabilityCatalog *catalog,
                         const char *capability_id)
{
    size_t index;
    if (catalog == NULL || capability_id == NULL) return SIZE_MAX;
    for (index = 0U; index < catalog->count; ++index) {
        if (strcmp(catalog->items[index].capability_id, capability_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

UmiStatus umi_document_capability_catalog_create(
    UmiDocumentCapabilityCatalog **out_catalog)
{
    UmiDocumentCapabilityCatalog *catalog;
    if (out_catalog == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_catalog = NULL;
    catalog = (UmiDocumentCapabilityCatalog *)calloc(1U, sizeof(*catalog));
    if (catalog == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    catalog->revision = 1U;
    *out_catalog = catalog;
    return UMI_STATUS_OK;
}

void umi_document_capability_catalog_destroy(
    UmiDocumentCapabilityCatalog *catalog)
{
    free(catalog);
}

UmiStatus umi_document_capability_descriptor_validate(
    const UmiDocumentCapabilityDescriptor *descriptor,
    char *out_message,
    size_t message_capacity)
{
    if (descriptor == NULL) {
        set_message(out_message, message_capacity, "descriptor is null");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (descriptor->struct_size != sizeof(*descriptor) ||
        descriptor->api_version != UMI_DOCUMENT_CAPABILITY_API_VERSION) {
        set_message(out_message, message_capacity, "descriptor ABI is incompatible");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!text_valid(descriptor->capability_id,
                    UMI_DOCUMENT_CAPABILITY_ID_CAPACITY) ||
        !text_valid(descriptor->title,
                    UMI_DOCUMENT_CAPABILITY_TITLE_CAPACITY) ||
        !text_valid(descriptor->summary,
                    UMI_DOCUMENT_CAPABILITY_SUMMARY_CAPACITY) ||
        !text_valid(descriptor->provider_role,
                    UMI_DOCUMENT_CAPABILITY_PROVIDER_CAPACITY)) {
        set_message(out_message, message_capacity,
                    "descriptor text is empty or exceeds its public capacity");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (descriptor->category < UMI_DOCUMENT_CAPABILITY_CATEGORY_LIFECYCLE ||
        descriptor->category >= UMI_DOCUMENT_CAPABILITY_CATEGORY_COUNT ||
        descriptor->maturity < UMI_DOCUMENT_CAPABILITY_MATURITY_PLANNED ||
        descriptor->maturity > UMI_DOCUMENT_CAPABILITY_MATURITY_DEPRECATED) {
        set_message(out_message, message_capacity, "descriptor enumeration is invalid");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    set_message(out_message, message_capacity, "valid");
    return UMI_STATUS_OK;
}

UmiStatus umi_document_capability_catalog_upsert(
    UmiDocumentCapabilityCatalog *catalog,
    const UmiDocumentCapabilityDescriptor *descriptor)
{
    size_t index;
    UmiStatus status;
    if (catalog == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_document_capability_descriptor_validate(descriptor, NULL, 0U);
    if (status != UMI_STATUS_OK) return status;
    index = find_index(catalog, descriptor->capability_id);
    if (index == SIZE_MAX) {
        if (catalog->count >= UMI_DOCUMENT_CAPABILITY_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = catalog->count++;
    }
    catalog->items[index] = *descriptor;
    catalog->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_document_capability_catalog_find(
    const UmiDocumentCapabilityCatalog *catalog,
    const char *capability_id,
    UmiDocumentCapabilityDescriptor *out_descriptor)
{
    size_t index;
    if (catalog == NULL || capability_id == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(catalog, capability_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = catalog->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_document_capability_catalog_at(
    const UmiDocumentCapabilityCatalog *catalog,
    size_t index,
    UmiDocumentCapabilityDescriptor *out_descriptor)
{
    if (catalog == NULL || out_descriptor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= catalog->count) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = catalog->items[index];
    return UMI_STATUS_OK;
}

size_t umi_document_capability_catalog_count(
    const UmiDocumentCapabilityCatalog *catalog)
{
    return catalog != NULL ? catalog->count : 0U;
}

static UmiStatus query_begin(const UmiDocumentCapabilityCatalog *catalog,
                             UmiDocumentCapabilityQueryResult *out_result)
{
    if (catalog == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_result, 0, sizeof(*out_result));
    out_result->catalog_revision = catalog->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_document_capability_catalog_query_category(
    const UmiDocumentCapabilityCatalog *catalog,
    UmiDocumentCapabilityCategory category,
    UmiDocumentCapabilityQueryResult *out_result)
{
    size_t index;
    UmiStatus status = query_begin(catalog, out_result);
    if (status != UMI_STATUS_OK) return status;
    if (category < UMI_DOCUMENT_CAPABILITY_CATEGORY_LIFECYCLE ||
        category >= UMI_DOCUMENT_CAPABILITY_CATEGORY_COUNT) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < catalog->count; ++index) {
        if (catalog->items[index].category == category) {
            out_result->indices[out_result->count++] = index;
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_document_capability_catalog_query_flags(
    const UmiDocumentCapabilityCatalog *catalog,
    uint32_t required_flags,
    UmiDocumentCapabilityQueryResult *out_result)
{
    size_t index;
    UmiStatus status = query_begin(catalog, out_result);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < catalog->count; ++index) {
        if ((catalog->items[index].flags & required_flags) == required_flags) {
            out_result->indices[out_result->count++] = index;
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_document_capability_catalog_snapshot(
    const UmiDocumentCapabilityCatalog *catalog,
    UmiDocumentCapabilityCatalogSnapshot *out_snapshot)
{
    size_t index;
    if (catalog == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->total_count = catalog->count;
    out_snapshot->revision = catalog->revision;
    for (index = 0U; index < catalog->count; ++index) {
        const UmiDocumentCapabilityDescriptor *item = &catalog->items[index];
        out_snapshot->category_counts[item->category] += 1U;
        if ((item->flags & UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED) != 0U) {
            out_snapshot->implemented_count += 1U;
        }
        if (item->maturity == UMI_DOCUMENT_CAPABILITY_MATURITY_PLANNED) {
            out_snapshot->planned_count += 1U;
        }
        if ((item->flags & UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS) != 0U) {
            out_snapshot->headless_count += 1U;
        }
        if ((item->flags & UMI_DOCUMENT_CAPABILITY_FLAG_GUI) != 0U) {
            out_snapshot->gui_count += 1U;
        }
        if ((item->flags & UMI_DOCUMENT_CAPABILITY_FLAG_PERSISTED) != 0U) {
            out_snapshot->persisted_count += 1U;
        }
        if ((item->flags & UMI_DOCUMENT_CAPABILITY_FLAG_SECURITY_SENSITIVE) != 0U) {
            out_snapshot->security_sensitive_count += 1U;
        }
    }
    return UMI_STATUS_OK;
}

const char *umi_document_capability_category_text(
    UmiDocumentCapabilityCategory category)
{
    static const char *const NAMES[] = {
        "lifecycle", "identity", "provider", "encoding", "persistence",
        "recovery", "conflict", "command", "navigation", "security",
        "performance", "integration"
    };
    return category >= UMI_DOCUMENT_CAPABILITY_CATEGORY_LIFECYCLE &&
           category < UMI_DOCUMENT_CAPABILITY_CATEGORY_COUNT
        ? NAMES[category] : "unknown";
}
