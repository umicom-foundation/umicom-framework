/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/features/working_copy.c
 *
 * PURPOSE:
 *   Define the canonical Working Copy capability metadata used by every Umicom
 *   editor host, IDE, comparer, designer and text-centric application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/features/working_copy.h"

/*
 * The descriptor intentionally contains no toolkit object or application
 * callback. It is safe to enumerate in command-line tools, headless tests,
 * GTK4, Qt, Wt and future adapters. The stable identifier is a persistence and
 * extension boundary; changing it requires an explicit migration.
 */
static const UmiEditorFeatureDescriptor FEATURE = {
    .struct_size = (uint32_t)sizeof(UmiEditorFeatureDescriptor),
    .api_version = UMI_EDITOR_FEATURE_API_VERSION,
    .feature_id = "umicom.editor.working_copy",
    .title = "Working Copy",
    .category = UMI_EDITOR_FEATURE_CATEGORY_WORKSPACE,
    .maturity = UMI_EDITOR_FEATURE_MATURITY_STABLE,
    .summary = "Dirty in-memory resource tracked independently of file I/O.",
    .flags = UMI_EDITOR_FEATURE_FLAG_CORE | UMI_EDITOR_FEATURE_FLAG_HEADLESS,
    .priority = 820,
    .dependency_count = 0U,
    .revision = 1U
};

/*
 * Copy editor feature working into module-owned storage so callers keep ownership of their
 * input values.
 */
const UmiEditorFeatureDescriptor *umi_editor_feature_working_copy(void)
{
    return &FEATURE;
}
