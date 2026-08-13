/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/features/editor_context_keys.c
 *
 * PURPOSE:
 *   Define the canonical Editor Context Keys capability metadata used by every Umicom
 *   editor host, IDE, comparer, designer and text-centric application.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/features/editor_context_keys.h"

/*
 * The descriptor intentionally contains no toolkit object or application
 * callback. It is safe to enumerate in command-line tools, headless tests,
 * GTK4, Qt, Wt and future adapters. The stable identifier is a persistence and
 * extension boundary; changing it requires an explicit migration.
 */
static const UmiEditorFeatureDescriptor FEATURE = {
    .struct_size = (uint32_t)sizeof(UmiEditorFeatureDescriptor),
    .api_version = UMI_EDITOR_FEATURE_API_VERSION,
    .feature_id = "umicom.editor.editor_context_keys",
    .title = "Editor Context Keys",
    .category = UMI_EDITOR_FEATURE_CATEGORY_COLLABORATION,
    .maturity = UMI_EDITOR_FEATURE_MATURITY_STABLE,
    .summary = "Declarative editor state used for contextual enablement.",
    .flags = UMI_EDITOR_FEATURE_FLAG_CORE | UMI_EDITOR_FEATURE_FLAG_HEADLESS,
    .priority = 1120,
    .dependency_count = 0U,
    .revision = 1U
};

const UmiEditorFeatureDescriptor *umi_editor_feature_editor_context_keys(void)
{
    return &FEATURE;
}
