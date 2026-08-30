/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/feature.h
 *
 * PURPOSE:
 *   Define the authoritative, toolkit-neutral catalogue for reusable editor
 *   capabilities supplied by Umicom Framework and optional extensions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_FEATURE_H
#define UMICOM_EDITOR_FEATURE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Public capacities are part of the C ABI and avoid ownership ambiguity. */
#define UMI_EDITOR_FEATURE_API_VERSION 2U
#define UMI_EDITOR_FEATURE_CAPACITY 256U
#define UMI_EDITOR_FEATURE_ID_CAPACITY 96U
#define UMI_EDITOR_FEATURE_TITLE_CAPACITY 96U
#define UMI_EDITOR_FEATURE_SUMMARY_CAPACITY 320U
#define UMI_EDITOR_FEATURE_DEPENDENCY_CAPACITY 8U
#define UMI_EDITOR_FEATURE_QUERY_CAPACITY 256U
#define UMI_EDITOR_FEATURE_MESSAGE_CAPACITY 512U

/** Broad capability families used by settings, UI grouping and conformance. */
typedef enum UmiEditorFeatureCategory {
    UMI_EDITOR_FEATURE_CATEGORY_TEXT = 0,
    UMI_EDITOR_FEATURE_CATEGORY_EDITING = 1,
    UMI_EDITOR_FEATURE_CATEGORY_NAVIGATION = 2,
    UMI_EDITOR_FEATURE_CATEGORY_VIEW = 3,
    UMI_EDITOR_FEATURE_CATEGORY_WORKSPACE = 4,
    UMI_EDITOR_FEATURE_CATEGORY_LANGUAGE = 5,
    UMI_EDITOR_FEATURE_CATEGORY_QUALITY = 6,
    UMI_EDITOR_FEATURE_CATEGORY_COLLABORATION = 7,
    UMI_EDITOR_FEATURE_CATEGORY_COUNT = 8
} UmiEditorFeatureCategory;

/** Delivery confidence rather than marketing availability. */
typedef enum UmiEditorFeatureMaturity {
    UMI_EDITOR_FEATURE_MATURITY_EXPERIMENTAL = 0,
    UMI_EDITOR_FEATURE_MATURITY_PREVIEW = 1,
    UMI_EDITOR_FEATURE_MATURITY_STABLE = 2,
    UMI_EDITOR_FEATURE_MATURITY_DEPRECATED = 3
} UmiEditorFeatureMaturity;

/** Adapter and deployment qualities advertised by a feature definition. */
typedef enum UmiEditorFeatureFlag {
    UMI_EDITOR_FEATURE_FLAG_NONE = 0U,
    UMI_EDITOR_FEATURE_FLAG_CORE = 1U << 0,
    UMI_EDITOR_FEATURE_FLAG_HEADLESS = 1U << 1,
    UMI_EDITOR_FEATURE_FLAG_GUI = 1U << 2,
    UMI_EDITOR_FEATURE_FLAG_OPTIONAL = 1U << 3,
    UMI_EDITOR_FEATURE_FLAG_SECURITY_SENSITIVE = 1U << 4,
    UMI_EDITOR_FEATURE_FLAG_PERSISTED = 1U << 5
} UmiEditorFeatureFlag;

/**
 * Immutable-by-convention feature metadata.
 *
 * The catalogue copies the entire record, including dependency identifiers,
 * so callers may pass stack values safely. Every record begins with structure
 * size and API version for forward-compatible validation across shared-library
 * and plug-in boundaries.
 */
typedef struct UmiEditorFeatureDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    char feature_id[UMI_EDITOR_FEATURE_ID_CAPACITY];
    char title[UMI_EDITOR_FEATURE_TITLE_CAPACITY];
    UmiEditorFeatureCategory category;
    UmiEditorFeatureMaturity maturity;
    char summary[UMI_EDITOR_FEATURE_SUMMARY_CAPACITY];
    uint32_t flags;
    int32_t priority;
    size_t dependency_count;
    char dependency_ids[UMI_EDITOR_FEATURE_DEPENDENCY_CAPACITY]
                       [UMI_EDITOR_FEATURE_ID_CAPACITY];
    uint64_t revision;
} UmiEditorFeatureDescriptor;

/** Stable summary used by About, diagnostics and conformance surfaces. */
typedef struct UmiEditorFeatureCatalogSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t total_count;
    size_t category_counts[UMI_EDITOR_FEATURE_CATEGORY_COUNT];
    size_t experimental_count;
    size_t preview_count;
    size_t stable_count;
    size_t deprecated_count;
    size_t headless_count;
    size_t gui_count;
    uint64_t revision;
} UmiEditorFeatureCatalogSnapshot;

/** Bounded query result containing catalogue indices, not borrowed pointers. */
typedef struct UmiEditorFeatureQueryResult {
    size_t count;
    size_t indices[UMI_EDITOR_FEATURE_QUERY_CAPACITY];
    uint64_t catalog_revision;
} UmiEditorFeatureQueryResult;

typedef struct UmiEditorFeatureCatalog UmiEditorFeatureCatalog;

UmiStatus umi_editor_feature_catalog_create(UmiEditorFeatureCatalog **out_catalog);
void umi_editor_feature_catalog_destroy(UmiEditorFeatureCatalog *catalog);

UmiStatus umi_editor_feature_descriptor_validate(
    const UmiEditorFeatureDescriptor *descriptor,
    char *out_message,
    size_t message_capacity);

UmiStatus umi_editor_feature_catalog_upsert(
    UmiEditorFeatureCatalog *catalog,
    const UmiEditorFeatureDescriptor *descriptor);
UmiStatus umi_editor_feature_catalog_remove(
    UmiEditorFeatureCatalog *catalog,
    const char *feature_id);
UmiStatus umi_editor_feature_catalog_find(
    const UmiEditorFeatureCatalog *catalog,
    const char *feature_id,
    UmiEditorFeatureDescriptor *out_descriptor);
UmiStatus umi_editor_feature_catalog_at(
    const UmiEditorFeatureCatalog *catalog,
    size_t index,
    UmiEditorFeatureDescriptor *out_descriptor);
size_t umi_editor_feature_catalog_count(const UmiEditorFeatureCatalog *catalog);
uint64_t umi_editor_feature_catalog_revision(const UmiEditorFeatureCatalog *catalog);

UmiStatus umi_editor_feature_catalog_query_category(
    const UmiEditorFeatureCatalog *catalog,
    UmiEditorFeatureCategory category,
    UmiEditorFeatureQueryResult *out_result);
UmiStatus umi_editor_feature_catalog_query_flags(
    const UmiEditorFeatureCatalog *catalog,
    uint32_t required_flags,
    UmiEditorFeatureQueryResult *out_result);
UmiStatus umi_editor_feature_catalog_snapshot(
    const UmiEditorFeatureCatalog *catalog,
    UmiEditorFeatureCatalogSnapshot *out_snapshot);

/** Verify that every declared dependency is present in the same catalogue. */
UmiStatus umi_editor_feature_catalog_validate_dependencies(
    const UmiEditorFeatureCatalog *catalog,
    char *out_message,
    size_t message_capacity);

/** Register the Framework-owned inventory declared by features.h. */
UmiStatus umi_editor_feature_catalog_register_builtins(
    UmiEditorFeatureCatalog *catalog);

const char *umi_editor_feature_category_text(UmiEditorFeatureCategory category);
const char *umi_editor_feature_maturity_text(UmiEditorFeatureMaturity maturity);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_FEATURE_H */
