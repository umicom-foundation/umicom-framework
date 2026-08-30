/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/features/go_to_line.c
 *
 * PURPOSE:
 *   Define the canonical Go To Line capability metadata used by every Umicom
 *   editor host, IDE, comparer, designer and text-centric application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/features/go_to_line.h"

/*
 * The descriptor intentionally contains no toolkit object or application
 * callback. It is safe to enumerate in command-line tools, headless tests,
 * GTK4, Qt, Wt and future adapters. The stable identifier is a persistence and
 * extension boundary; changing it requires an explicit migration.
 */
static const UmiEditorFeatureDescriptor FEATURE = {
    .struct_size = (uint32_t)sizeof(UmiEditorFeatureDescriptor),
    .api_version = UMI_EDITOR_FEATURE_API_VERSION,
    .feature_id = "umicom.editor.go_to_line",
    .title = "Go To Line",
    .category = UMI_EDITOR_FEATURE_CATEGORY_NAVIGATION,
    .maturity = UMI_EDITOR_FEATURE_MATURITY_STABLE,
    .summary = "Validated navigation to line, column or byte offset.",
    .flags = UMI_EDITOR_FEATURE_FLAG_CORE | UMI_EDITOR_FEATURE_FLAG_HEADLESS,
    .priority = 400,
    .dependency_count = 0U,
    .revision = 1U
};

const UmiEditorFeatureDescriptor *umi_editor_feature_go_to_line(void)
{
    return &FEATURE;
}
