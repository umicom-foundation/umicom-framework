/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/feature.c
 *
 * PURPOSE:
 *   Implement the owned editor capability catalogue, filtering, summaries and
 *   dependency validation shared by every Umicom application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/* Provide the text terminated operation used by this module and its client applications. */
static int text_terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

/* Provide the set message operation used by this module and its client applications. */
static void set_message(char *out_message,
                        size_t capacity,
                        const char *message)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_message == NULL || capacity == 0U) return;
    (void)snprintf(out_message, capacity, "%s", message != NULL ? message : "");
}

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiEditorFeatureCatalog *catalog,
                         const char *feature_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalog == NULL || feature_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalog->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalog->items[index].feature_id, feature_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

/*
 * Initialise editor feature catalog from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_feature_catalog_create(UmiEditorFeatureCatalog **out_catalog)
{
    UmiEditorFeatureCatalog *catalog;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_catalog == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_catalog = NULL;
    catalog = (UmiEditorFeatureCatalog *)calloc(1U, sizeof(*catalog));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalog == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    catalog->revision = 1U;
    *out_catalog = catalog;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor feature catalog so the same storage can be reused
 * safely.
 */
void umi_editor_feature_catalog_destroy(UmiEditorFeatureCatalog *catalog)
{
    free(catalog);
}

/*
 * Check that editor feature descriptor satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_editor_feature_descriptor_validate(
    const UmiEditorFeatureDescriptor *descriptor,
    char *out_message,
    size_t message_capacity)
{
    size_t dependency_index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL) {
        set_message(out_message, message_capacity, "descriptor is null");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (descriptor->struct_size != sizeof(*descriptor)) {
        set_message(out_message, message_capacity, "descriptor structure size is incompatible");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (descriptor->api_version != UMI_EDITOR_FEATURE_API_VERSION) {
        set_message(out_message, message_capacity, "descriptor API version is unsupported");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!text_terminated(descriptor->feature_id, sizeof(descriptor->feature_id)) ||
        descriptor->feature_id[0] == '\0') {
        set_message(out_message, message_capacity, "feature identifier is empty or unterminated");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!text_terminated(descriptor->title, sizeof(descriptor->title)) ||
        descriptor->title[0] == '\0') {
        set_message(out_message, message_capacity, "feature title is empty or unterminated");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!text_terminated(descriptor->summary, sizeof(descriptor->summary)) ||
        descriptor->summary[0] == '\0') {
        set_message(out_message, message_capacity, "feature summary is empty or unterminated");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (descriptor->category < UMI_EDITOR_FEATURE_CATEGORY_TEXT ||
        descriptor->category >= UMI_EDITOR_FEATURE_CATEGORY_COUNT) {
        set_message(out_message, message_capacity, "feature category is outside the public enumeration");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (descriptor->maturity < UMI_EDITOR_FEATURE_MATURITY_EXPERIMENTAL ||
        descriptor->maturity > UMI_EDITOR_FEATURE_MATURITY_DEPRECATED) {
        set_message(out_message, message_capacity, "feature maturity is outside the public enumeration");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (descriptor->dependency_count > UMI_EDITOR_FEATURE_DEPENDENCY_CAPACITY) {
        set_message(out_message, message_capacity, "feature dependency capacity exceeded");
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (dependency_index = 0U;
         dependency_index < descriptor->dependency_count;
         ++dependency_index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
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

/*
 * Provide the editor feature catalog upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_feature_catalog_upsert(
    UmiEditorFeatureCatalog *catalog,
    const UmiEditorFeatureDescriptor *descriptor)
{
    UmiEditorFeatureDescriptor copy;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalog == NULL || descriptor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_feature_descriptor_validate(descriptor, NULL, 0U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    index = find_index(catalog, descriptor->feature_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Remove editor feature catalog while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_feature_catalog_remove(
    UmiEditorFeatureCatalog *catalog,
    const char *feature_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalog == NULL || feature_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(catalog, feature_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < catalog->count) {
        (void)memmove(&catalog->items[index],
                      &catalog->items[index + 1U],
                      (catalog->count - index - 1U) * sizeof(catalog->items[0]));
    }
    catalog->count -= 1U;
    catalog->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find editor feature catalog while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_feature_catalog_find(
    const UmiEditorFeatureCatalog *catalog,
    const char *feature_id,
    UmiEditorFeatureDescriptor *out_descriptor)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalog == NULL || feature_id == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(catalog, feature_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = catalog->items[index];
    return UMI_STATUS_OK;
}

/*
 * Find editor feature catalog while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_feature_catalog_at(
    const UmiEditorFeatureCatalog *catalog,
    size_t index,
    UmiEditorFeatureDescriptor *out_descriptor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalog == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= catalog->count) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = catalog->items[index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor feature catalog without changing
 * their state.
 */
size_t umi_editor_feature_catalog_count(const UmiEditorFeatureCatalog *catalog)
{
    return catalog != NULL ? catalog->count : 0U;
}

/*
 * Provide the editor feature catalog revision operation used by this module and its client
 * applications.
 */
uint64_t umi_editor_feature_catalog_revision(const UmiEditorFeatureCatalog *catalog)
{
    return catalog != NULL ? catalog->revision : 0U;
}

/* Provide the query begin operation used by this module and its client applications. */
static UmiStatus query_begin(const UmiEditorFeatureCatalog *catalog,
                             UmiEditorFeatureQueryResult *out_result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalog == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_result, 0, sizeof(*out_result));
    out_result->catalog_revision = catalog->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor feature catalog query category operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_feature_catalog_query_category(
    const UmiEditorFeatureCatalog *catalog,
    UmiEditorFeatureCategory category,
    UmiEditorFeatureQueryResult *out_result)
{
    size_t index;
    UmiStatus status = query_begin(catalog, out_result);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (category < UMI_EDITOR_FEATURE_CATEGORY_TEXT ||
        category >= UMI_EDITOR_FEATURE_CATEGORY_COUNT) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalog->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (catalog->items[index].category == category) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (out_result->count >= UMI_EDITOR_FEATURE_QUERY_CAPACITY) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            out_result->indices[out_result->count++] = index;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the editor feature catalog query flags operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_feature_catalog_query_flags(
    const UmiEditorFeatureCatalog *catalog,
    uint32_t required_flags,
    UmiEditorFeatureQueryResult *out_result)
{
    size_t index;
    UmiStatus status = query_begin(catalog, out_result);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalog->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if ((catalog->items[index].flags & required_flags) == required_flags) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (out_result->count >= UMI_EDITOR_FEATURE_QUERY_CAPACITY) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            out_result->indices[out_result->count++] = index;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the editor feature catalog snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_feature_catalog_snapshot(
    const UmiEditorFeatureCatalog *catalog,
    UmiEditorFeatureCatalogSnapshot *out_snapshot)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalog == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_FEATURE_API_VERSION;
    out_snapshot->total_count = catalog->count;
    out_snapshot->revision = catalog->revision;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalog->count; ++index) {
        const UmiEditorFeatureDescriptor *item = &catalog->items[index];
        out_snapshot->category_counts[item->category] += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (item->maturity == UMI_EDITOR_FEATURE_MATURITY_EXPERIMENTAL) {
            out_snapshot->experimental_count += 1U;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (item->maturity == UMI_EDITOR_FEATURE_MATURITY_PREVIEW) {
            out_snapshot->preview_count += 1U;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (item->maturity == UMI_EDITOR_FEATURE_MATURITY_STABLE) {
            out_snapshot->stable_count += 1U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            out_snapshot->deprecated_count += 1U;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if ((item->flags & UMI_EDITOR_FEATURE_FLAG_HEADLESS) != 0U) {
            out_snapshot->headless_count += 1U;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if ((item->flags & UMI_EDITOR_FEATURE_FLAG_GUI) != 0U) {
            out_snapshot->gui_count += 1U;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the editor feature catalog validate dependencies operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_feature_catalog_validate_dependencies(
    const UmiEditorFeatureCatalog *catalog,
    char *out_message,
    size_t message_capacity)
{
    size_t feature_index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalog == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (feature_index = 0U; feature_index < catalog->count; ++feature_index) {
        const UmiEditorFeatureDescriptor *item = &catalog->items[feature_index];
        size_t dependency_index;
        /* Visit each bounded item once so every record receives the same rule. */
        for (dependency_index = 0U;
             dependency_index < item->dependency_count;
             ++dependency_index) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (find_index(catalog, item->dependency_ids[dependency_index]) == SIZE_MAX) {
                /*
                 * Protect caller-owned memory by checking that required state is available before it is
                 * used.
                 */
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

/*
 * Provide the editor feature category text operation used by this module and its client
 * applications.
 */
const char *umi_editor_feature_category_text(UmiEditorFeatureCategory category)
{
    static const char *const TEXT[] = {
        "Text", "Editing", "Navigation", "View", "Workspace",
        "Language", "Quality", "Collaboration"
    };
    /* Apply this branch only when its contract condition is satisfied. */
    if (category < UMI_EDITOR_FEATURE_CATEGORY_TEXT ||
        category >= UMI_EDITOR_FEATURE_CATEGORY_COUNT) return "Unknown";
    return TEXT[category];
}

/*
 * Provide the editor feature maturity text operation used by this module and its client
 * applications.
 */
const char *umi_editor_feature_maturity_text(UmiEditorFeatureMaturity maturity)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (maturity) {
        case UMI_EDITOR_FEATURE_MATURITY_EXPERIMENTAL: return "Experimental";
        case UMI_EDITOR_FEATURE_MATURITY_PREVIEW: return "Preview";
        case UMI_EDITOR_FEATURE_MATURITY_STABLE: return "Stable";
        case UMI_EDITOR_FEATURE_MATURITY_DEPRECATED: return "Deprecated";
        default: return "Unknown";
    }
}
