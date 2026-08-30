/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/features/extension_points.c
 *
 * PURPOSE:
 *   Define the canonical Extension Points capability metadata used by every Umicom
 *   editor host, IDE, comparer, designer and text-centric application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/features/extension_points.h"

/*
 * The descriptor intentionally contains no toolkit object or application
 * callback. It is safe to enumerate in command-line tools, headless tests,
 * GTK4, Qt, Wt and future adapters. The stable identifier is a persistence and
 * extension boundary; changing it requires an explicit migration.
 */
static const UmiEditorFeatureDescriptor FEATURE = {
    .struct_size = (uint32_t)sizeof(UmiEditorFeatureDescriptor),
    .api_version = UMI_EDITOR_FEATURE_API_VERSION,
    .feature_id = "umicom.editor.extension_points",
    .title = "Extension Points",
    .category = UMI_EDITOR_FEATURE_CATEGORY_COLLABORATION,
    .maturity = UMI_EDITOR_FEATURE_MATURITY_STABLE,
    .summary = "Registered editor contribution points and ownership metadata.",
    .flags = UMI_EDITOR_FEATURE_FLAG_CORE | UMI_EDITOR_FEATURE_FLAG_HEADLESS,
    .priority = 1100,
    .dependency_count = 0U,
    .revision = 1U
};

const UmiEditorFeatureDescriptor *umi_editor_feature_extension_points(void)
{
    return &FEATURE;
}
