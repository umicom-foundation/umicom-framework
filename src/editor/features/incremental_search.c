/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/features/incremental_search.c
 *
 * PURPOSE:
 *   Define the canonical Incremental Search capability metadata used by every Umicom
 *   editor host, IDE, comparer, designer and text-centric application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/features/incremental_search.h"

/*
 * The descriptor intentionally contains no toolkit object or application
 * callback. It is safe to enumerate in command-line tools, headless tests,
 * GTK4, Qt, Wt and future adapters. The stable identifier is a persistence and
 * extension boundary; changing it requires an explicit migration.
 */
static const UmiEditorFeatureDescriptor FEATURE = {
    .struct_size = (uint32_t)sizeof(UmiEditorFeatureDescriptor),
    .api_version = UMI_EDITOR_FEATURE_API_VERSION,
    .feature_id = "umicom.editor.incremental_search",
    .title = "Incremental Search",
    .category = UMI_EDITOR_FEATURE_CATEGORY_NAVIGATION,
    .maturity = UMI_EDITOR_FEATURE_MATURITY_PREVIEW,
    .summary = "Search-as-you-type sessions preserving origin and wrap state.",
    .flags = UMI_EDITOR_FEATURE_FLAG_CORE | UMI_EDITOR_FEATURE_FLAG_HEADLESS,
    .priority = 390,
    .dependency_count = 0U,
    .revision = 1U
};

const UmiEditorFeatureDescriptor *umi_editor_feature_incremental_search(void)
{
    return &FEATURE;
}
