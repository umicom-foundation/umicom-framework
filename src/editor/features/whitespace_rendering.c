/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/features/whitespace_rendering.c
 *
 * PURPOSE:
 *   Define the canonical Whitespace Rendering capability metadata used by every Umicom
 *   editor host, IDE, comparer, designer and text-centric application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/features/whitespace_rendering.h"

/*
 * The descriptor intentionally contains no toolkit object or application
 * callback. It is safe to enumerate in command-line tools, headless tests,
 * GTK4, Qt, Wt and future adapters. The stable identifier is a persistence and
 * extension boundary; changing it requires an explicit migration.
 */
static const UmiEditorFeatureDescriptor FEATURE = {
    .struct_size = (uint32_t)sizeof(UmiEditorFeatureDescriptor),
    .api_version = UMI_EDITOR_FEATURE_API_VERSION,
    .feature_id = "umicom.editor.whitespace_rendering",
    .title = "Whitespace Rendering",
    .category = UMI_EDITOR_FEATURE_CATEGORY_VIEW,
    .maturity = UMI_EDITOR_FEATURE_MATURITY_STABLE,
    .summary = "Spaces, tabs, trailing whitespace and control character display.",
    .flags = UMI_EDITOR_FEATURE_FLAG_CORE | UMI_EDITOR_FEATURE_FLAG_HEADLESS | UMI_EDITOR_FEATURE_FLAG_GUI,
    .priority = 560,
    .dependency_count = 0U,
    .revision = 1U
};

/*
 * Provide the editor feature whitespace rendering operation used by this module and its
 * client applications.
 */
const UmiEditorFeatureDescriptor *umi_editor_feature_whitespace_rendering(void)
{
    return &FEATURE;
}
