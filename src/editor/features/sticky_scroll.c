/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/features/sticky_scroll.c
 *
 * PURPOSE:
 *   Define the canonical Sticky Scroll capability metadata used by every Umicom
 *   editor host, IDE, comparer, designer and text-centric application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/features/sticky_scroll.h"

/*
 * The descriptor intentionally contains no toolkit object or application
 * callback. It is safe to enumerate in command-line tools, headless tests,
 * GTK4, Qt, Wt and future adapters. The stable identifier is a persistence and
 * extension boundary; changing it requires an explicit migration.
 */
static const UmiEditorFeatureDescriptor FEATURE = {
    .struct_size = (uint32_t)sizeof(UmiEditorFeatureDescriptor),
    .api_version = UMI_EDITOR_FEATURE_API_VERSION,
    .feature_id = "umicom.editor.sticky_scroll",
    .title = "Sticky Scroll",
    .category = UMI_EDITOR_FEATURE_CATEGORY_VIEW,
    .maturity = UMI_EDITOR_FEATURE_MATURITY_PREVIEW,
    .summary = "Pinned enclosing symbols at the top of the viewport.",
    .flags = UMI_EDITOR_FEATURE_FLAG_CORE | UMI_EDITOR_FEATURE_FLAG_HEADLESS | UMI_EDITOR_FEATURE_FLAG_GUI,
    .priority = 610,
    .dependency_count = 0U,
    .revision = 1U
};

/*
 * Provide the editor feature sticky scroll operation used by this module and its client
 * applications.
 */
const UmiEditorFeatureDescriptor *umi_editor_feature_sticky_scroll(void)
{
    return &FEATURE;
}
