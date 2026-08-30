/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/features/virtual_document.c
 *
 * PURPOSE:
 *   Define the canonical Virtual Document capability metadata used by every Umicom
 *   editor host, IDE, comparer, designer and text-centric application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/features/virtual_document.h"

/*
 * The descriptor intentionally contains no toolkit object or application
 * callback. It is safe to enumerate in command-line tools, headless tests,
 * GTK4, Qt, Wt and future adapters. The stable identifier is a persistence and
 * extension boundary; changing it requires an explicit migration.
 */
static const UmiEditorFeatureDescriptor FEATURE = {
    .struct_size = (uint32_t)sizeof(UmiEditorFeatureDescriptor),
    .api_version = UMI_EDITOR_FEATURE_API_VERSION,
    .feature_id = "umicom.editor.virtual_document",
    .title = "Virtual Document",
    .category = UMI_EDITOR_FEATURE_CATEGORY_TEXT,
    .maturity = UMI_EDITOR_FEATURE_MATURITY_PREVIEW,
    .summary = "Read-only content supplied by debuggers, diffs and remote providers.",
    .flags = UMI_EDITOR_FEATURE_FLAG_CORE | UMI_EDITOR_FEATURE_FLAG_HEADLESS,
    .priority = 170,
    .dependency_count = 0U,
    .revision = 1U
};

const UmiEditorFeatureDescriptor *umi_editor_feature_virtual_document(void)
{
    return &FEATURE;
}
