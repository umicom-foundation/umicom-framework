/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/features/recent_editors.c
 *
 * PURPOSE:
 *   Define the canonical Recent Editors capability metadata used by every Umicom
 *   editor host, IDE, comparer, designer and text-centric application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/features/recent_editors.h"

/*
 * The descriptor intentionally contains no toolkit object or application
 * callback. It is safe to enumerate in command-line tools, headless tests,
 * GTK4, Qt, Wt and future adapters. The stable identifier is a persistence and
 * extension boundary; changing it requires an explicit migration.
 */
static const UmiEditorFeatureDescriptor FEATURE = {
    .struct_size = (uint32_t)sizeof(UmiEditorFeatureDescriptor),
    .api_version = UMI_EDITOR_FEATURE_API_VERSION,
    .feature_id = "umicom.editor.recent_editors",
    .title = "Recent Editors",
    .category = UMI_EDITOR_FEATURE_CATEGORY_WORKSPACE,
    .maturity = UMI_EDITOR_FEATURE_MATURITY_STABLE,
    .summary = "Most recently used editor traversal and reopening.",
    .flags = UMI_EDITOR_FEATURE_FLAG_CORE | UMI_EDITOR_FEATURE_FLAG_HEADLESS | UMI_EDITOR_FEATURE_FLAG_GUI,
    .priority = 730,
    .dependency_count = 0U,
    .revision = 1U
};

/*
 * Provide the editor feature recent editors operation used by this module and its client
 * applications.
 */
const UmiEditorFeatureDescriptor *umi_editor_feature_recent_editors(void)
{
    return &FEATURE;
}
