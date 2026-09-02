/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/features/collaborative_cursor.c
 *
 * PURPOSE:
 *   Define the canonical Collaborative Cursor capability metadata used by every Umicom
 *   editor host, IDE, comparer, designer and text-centric application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/features/collaborative_cursor.h"

/*
 * The descriptor intentionally contains no toolkit object or application
 * callback. It is safe to enumerate in command-line tools, headless tests,
 * GTK4, Qt, Wt and future adapters. The stable identifier is a persistence and
 * extension boundary; changing it requires an explicit migration.
 */
static const UmiEditorFeatureDescriptor FEATURE = {
    .struct_size = (uint32_t)sizeof(UmiEditorFeatureDescriptor),
    .api_version = UMI_EDITOR_FEATURE_API_VERSION,
    .feature_id = "umicom.editor.collaborative_cursor",
    .title = "Collaborative Cursor",
    .category = UMI_EDITOR_FEATURE_CATEGORY_COLLABORATION,
    .maturity = UMI_EDITOR_FEATURE_MATURITY_EXPERIMENTAL,
    .summary = "Presence, remote selections and participant identity.",
    .flags = UMI_EDITOR_FEATURE_FLAG_CORE | UMI_EDITOR_FEATURE_FLAG_HEADLESS | UMI_EDITOR_FEATURE_FLAG_GUI,
    .priority = 1150,
    .dependency_count = 0U,
    .revision = 1U
};

/*
 * Provide the editor feature collaborative cursor operation used by this module and its
 * client applications.
 */
const UmiEditorFeatureDescriptor *umi_editor_feature_collaborative_cursor(void)
{
    return &FEATURE;
}
