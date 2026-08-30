/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/features/problems_navigation.c
 *
 * PURPOSE:
 *   Define the canonical Problems Navigation capability metadata used by every Umicom
 *   editor host, IDE, comparer, designer and text-centric application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/features/problems_navigation.h"

/*
 * The descriptor intentionally contains no toolkit object or application
 * callback. It is safe to enumerate in command-line tools, headless tests,
 * GTK4, Qt, Wt and future adapters. The stable identifier is a persistence and
 * extension boundary; changing it requires an explicit migration.
 */
static const UmiEditorFeatureDescriptor FEATURE = {
    .struct_size = (uint32_t)sizeof(UmiEditorFeatureDescriptor),
    .api_version = UMI_EDITOR_FEATURE_API_VERSION,
    .feature_id = "umicom.editor.problems_navigation",
    .title = "Problems Navigation",
    .category = UMI_EDITOR_FEATURE_CATEGORY_QUALITY,
    .maturity = UMI_EDITOR_FEATURE_MATURITY_STABLE,
    .summary = "Problem list filtering and next/previous navigation.",
    .flags = UMI_EDITOR_FEATURE_FLAG_CORE | UMI_EDITOR_FEATURE_FLAG_HEADLESS | UMI_EDITOR_FEATURE_FLAG_GUI,
    .priority = 1000,
    .dependency_count = 0U,
    .revision = 1U
};

const UmiEditorFeatureDescriptor *umi_editor_feature_problems_navigation(void)
{
    return &FEATURE;
}
