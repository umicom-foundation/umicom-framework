/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/feature.c
 *
 * PURPOSE:
 *   Implement the owned editor capability catalogue, filtering, summaries and
 *   dependency validation shared by every Umicom application.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/feature.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiEditorFeatureCatalog {
    UmiEditorFeatureDescriptor items[UMI_EDITOR_FEATURE_CAPACITY];
    size_t count;
    uint64_t revision;
};

static int text_terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static void set_message(char *out_message,
                        size_t capacity,
                        const char *message)
{
    if (out_message == NULL || capacity == 0U) return;
    (void)snprintf(out_message, capacity, "%s", message != NULL ? message : "");
}

static size_t find_index(const UmiEditorFeatureCatalog *catalog,
                         const char *feature_id)
{
    size_t index;

    if (catalog == NULL || feature_id == NULL) return SIZE_MAX;
    for (index = 0U; index < catalog->count; ++index) {
        if (strcmp(catalog->items[index].feature_id, feature_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

UmiStatus umi_editor_feature_catalog_create(UmiEditorFeatureCatalog **out_catalog)
{
    UmiEditorFeatureCatalog *catalog;

    if (out_catalog == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_catalog = NULL;
    catalog = (UmiEditorFeatureCatalog *)calloc(1U, sizeof(*catalog));
    if (catalog == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    catalog->revision = 1U;
    *out_catalog = catalog;
    return UMI_STATUS_OK;
}

void umi_editor_feature_catalog_destroy(UmiEditorFeatureCatalog *catalog)
{
    free(catalog);
}

UmiStatus umi_editor_feature_descriptor_validate(
    const UmiEditorFeatureDescriptor *descriptor,
    char *out_message,
    size_t message_capacity)
{
    size_t dependency_index;

    if (descriptor == NULL) {
        set_message(out_message, message_capacity, "descriptor is null");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (descriptor->struct_size != sizeof(*descriptor)) {
        set_message(out_message, message_capacity, "descriptor structure size is incompatible");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (descriptor->api_version != UMI_EDITOR_FEATURE_API_VERSION) {
        set_message(out_message, message_capacity, "descriptor API version is unsupported");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!text_terminated(descriptor->feature_id, sizeof(descriptor->feature_id)) ||
        descriptor->feature_id[0] == '\0') {
        set_message(out_message, message_capacity, "feature identifier is empty or unterminated");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!text_terminated(descriptor->title, sizeof(descriptor->title)) ||
        descriptor->title[0] == '\0') {
        set_message(out_message, message_capacity, "feature title is empty or unterminated");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!text_terminated(descriptor->summary, sizeof(descriptor->summary)) ||
        descriptor->summary[0] == '\0') {
        set_message(out_message, message_capacity, "feature summary is empty or unterminated");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (descriptor->category < UMI_EDITOR_FEATURE_CATEGORY_TEXT ||
        descriptor->category >= UMI_EDITOR_FEATURE_CATEGORY_COUNT) {
        set_message(out_message, message_capacity, "feature category is outside the public enumeration");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (descriptor->maturity < UMI_EDITOR_FEATURE_MATURITY_EXPERIMENTAL ||
        descriptor->maturity > UMI_EDITOR_FEATURE_MATURITY_DEPRECATED) {
        set_message(out_message, message_capacity, "feature maturity is outside the public enumeration");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (descriptor->dependency_count > UMI_EDITOR_FEATURE_DEPENDENCY_CAPACITY) {
        set_message(out_message, message_capacity, "feature dependency capacity exceeded");
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (dependency_index = 0U;
         dependency_index < descriptor->dependency_count;
         ++dependency_index) {
        if (!text_terminated(descriptor->dependency_ids[dependency_index],
                             UMI_EDITOR_FEATURE_ID_CAPACITY) ||
            descriptor->dependency_ids[dependency_index][0] == '\0') {
            set_message(out_message, message_capacity,
                        "feature dependency identifier is empty or unterminated");
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    set_message(out_message, message_capacity, "valid");
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_feature_catalog_upsert(
    UmiEditorFeatureCatalog *catalog,
    const UmiEditorFeatureDescriptor *descriptor)
{
    UmiEditorFeatureDescriptor copy;
    size_t index;
    UmiStatus status;

    if (catalog == NULL || descriptor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_feature_descriptor_validate(descriptor, NULL, 0U);
    if (status != UMI_STATUS_OK) return status;

    index = find_index(catalog, descriptor->feature_id);
    if (index == SIZE_MAX) {
        if (catalog->count >= UMI_EDITOR_FEATURE_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = catalog->count;
        catalog->count += 1U;
    }

    copy = *descriptor;
    copy.revision = catalog->revision + 1U;
    catalog->items[index] = copy;
    catalog->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_feature_catalog_remove(
    UmiEditorFeatureCatalog *catalog,
    const char *feature_id)
{
    size_t index;

    if (catalog == NULL || feature_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(catalog, feature_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < catalog->count) {
        (void)memmove(&catalog->items[index],
                      &catalog->items[index + 1U],
                      (catalog->count - index - 1U) * sizeof(catalog->items[0]));
    }
    catalog->count -= 1U;
    catalog->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_feature_catalog_find(
    const UmiEditorFeatureCatalog *catalog,
    const char *feature_id,
    UmiEditorFeatureDescriptor *out_descriptor)
{
    size_t index;

    if (catalog == NULL || feature_id == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(catalog, feature_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = catalog->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_feature_catalog_at(
    const UmiEditorFeatureCatalog *catalog,
    size_t index,
    UmiEditorFeatureDescriptor *out_descriptor)
{
    if (catalog == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= catalog->count) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = catalog->items[index];
    return UMI_STATUS_OK;
}

size_t umi_editor_feature_catalog_count(const UmiEditorFeatureCatalog *catalog)
{
    return catalog != NULL ? catalog->count : 0U;
}

uint64_t umi_editor_feature_catalog_revision(const UmiEditorFeatureCatalog *catalog)
{
    return catalog != NULL ? catalog->revision : 0U;
}

static UmiStatus query_begin(const UmiEditorFeatureCatalog *catalog,
                             UmiEditorFeatureQueryResult *out_result)
{
    if (catalog == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_result, 0, sizeof(*out_result));
    out_result->catalog_revision = catalog->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_feature_catalog_query_category(
    const UmiEditorFeatureCatalog *catalog,
    UmiEditorFeatureCategory category,
    UmiEditorFeatureQueryResult *out_result)
{
    size_t index;
    UmiStatus status = query_begin(catalog, out_result);

    if (status != UMI_STATUS_OK) return status;
    if (category < UMI_EDITOR_FEATURE_CATEGORY_TEXT ||
        category >= UMI_EDITOR_FEATURE_CATEGORY_COUNT) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < catalog->count; ++index) {
        if (catalog->items[index].category == category) {
            if (out_result->count >= UMI_EDITOR_FEATURE_QUERY_CAPACITY) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            out_result->indices[out_result->count++] = index;
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_feature_catalog_query_flags(
    const UmiEditorFeatureCatalog *catalog,
    uint32_t required_flags,
    UmiEditorFeatureQueryResult *out_result)
{
    size_t index;
    UmiStatus status = query_begin(catalog, out_result);

    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < catalog->count; ++index) {
        if ((catalog->items[index].flags & required_flags) == required_flags) {
            if (out_result->count >= UMI_EDITOR_FEATURE_QUERY_CAPACITY) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            out_result->indices[out_result->count++] = index;
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_feature_catalog_snapshot(
    const UmiEditorFeatureCatalog *catalog,
    UmiEditorFeatureCatalogSnapshot *out_snapshot)
{
    size_t index;

    if (catalog == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_FEATURE_API_VERSION;
    out_snapshot->total_count = catalog->count;
    out_snapshot->revision = catalog->revision;

    for (index = 0U; index < catalog->count; ++index) {
        const UmiEditorFeatureDescriptor *item = &catalog->items[index];
        out_snapshot->category_counts[item->category] += 1U;
        if (item->maturity == UMI_EDITOR_FEATURE_MATURITY_EXPERIMENTAL) {
            out_snapshot->experimental_count += 1U;
        } else if (item->maturity == UMI_EDITOR_FEATURE_MATURITY_PREVIEW) {
            out_snapshot->preview_count += 1U;
        } else if (item->maturity == UMI_EDITOR_FEATURE_MATURITY_STABLE) {
            out_snapshot->stable_count += 1U;
        } else {
            out_snapshot->deprecated_count += 1U;
        }
        if ((item->flags & UMI_EDITOR_FEATURE_FLAG_HEADLESS) != 0U) {
            out_snapshot->headless_count += 1U;
        }
        if ((item->flags & UMI_EDITOR_FEATURE_FLAG_GUI) != 0U) {
            out_snapshot->gui_count += 1U;
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_feature_catalog_validate_dependencies(
    const UmiEditorFeatureCatalog *catalog,
    char *out_message,
    size_t message_capacity)
{
    size_t feature_index;

    if (catalog == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (feature_index = 0U; feature_index < catalog->count; ++feature_index) {
        const UmiEditorFeatureDescriptor *item = &catalog->items[feature_index];
        size_t dependency_index;
        for (dependency_index = 0U;
             dependency_index < item->dependency_count;
             ++dependency_index) {
            if (find_index(catalog, item->dependency_ids[dependency_index]) == SIZE_MAX) {
                if (out_message != NULL && message_capacity > 0U) {
                    (void)snprintf(out_message, message_capacity,
                                   "%s requires missing feature %s",
                                   item->feature_id,
                                   item->dependency_ids[dependency_index]);
                }
                return UMI_STATUS_NOT_FOUND;
            }
        }
    }
    set_message(out_message, message_capacity, "all feature dependencies are satisfied");
    return UMI_STATUS_OK;
}

const char *umi_editor_feature_category_text(UmiEditorFeatureCategory category)
{
    static const char *const TEXT[] = {
        "Text", "Editing", "Navigation", "View", "Workspace",
        "Language", "Quality", "Collaboration"
    };
    if (category < UMI_EDITOR_FEATURE_CATEGORY_TEXT ||
        category >= UMI_EDITOR_FEATURE_CATEGORY_COUNT) return "Unknown";
    return TEXT[category];
}

const char *umi_editor_feature_maturity_text(UmiEditorFeatureMaturity maturity)
{
    switch (maturity) {
        case UMI_EDITOR_FEATURE_MATURITY_EXPERIMENTAL: return "Experimental";
        case UMI_EDITOR_FEATURE_MATURITY_PREVIEW: return "Preview";
        case UMI_EDITOR_FEATURE_MATURITY_STABLE: return "Stable";
        case UMI_EDITOR_FEATURE_MATURITY_DEPRECATED: return "Deprecated";
        default: return "Unknown";
    }
}
