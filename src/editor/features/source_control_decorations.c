/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/features/source_control_decorations.c
 *
 * PURPOSE:
 *   Define the canonical Source Control Decorations capability metadata used by every Umicom
 *   editor host, IDE, comparer, designer and text-centric application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/features/source_control_decorations.h"

/*
 * The descriptor intentionally contains no toolkit object or application
 * callback. It is safe to enumerate in command-line tools, headless tests,
 * GTK4, Qt, Wt and future adapters. The stable identifier is a persistence and
 * extension boundary; changing it requires an explicit migration.
 */
static const UmiEditorFeatureDescriptor FEATURE = {
    .struct_size = (uint32_t)sizeof(UmiEditorFeatureDescriptor),
    .api_version = UMI_EDITOR_FEATURE_API_VERSION,
    .feature_id = "umicom.editor.source_control_decorations",
    .title = "Source Control Decorations",
    .category = UMI_EDITOR_FEATURE_CATEGORY_QUALITY,
    .maturity = UMI_EDITOR_FEATURE_MATURITY_PREVIEW,
    .summary = "Added, modified, deleted and conflict editor markers.",
    .flags = UMI_EDITOR_FEATURE_FLAG_CORE | UMI_EDITOR_FEATURE_FLAG_HEADLESS | UMI_EDITOR_FEATURE_FLAG_GUI,
    .priority = 1010,
    .dependency_count = 0U,
    .revision = 1U
};

/*
 * Provide the editor feature source control decorations operation used by this module and
 * its client applications.
 */
const UmiEditorFeatureDescriptor *umi_editor_feature_source_control_decorations(void)
{
    return &FEATURE;
}
