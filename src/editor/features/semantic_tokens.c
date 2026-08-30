/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/features/semantic_tokens.c
 *
 * PURPOSE:
 *   Define the canonical Semantic Tokens capability metadata used by every Umicom
 *   editor host, IDE, comparer, designer and text-centric application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/features/semantic_tokens.h"

/*
 * The descriptor intentionally contains no toolkit object or application
 * callback. It is safe to enumerate in command-line tools, headless tests,
 * GTK4, Qt, Wt and future adapters. The stable identifier is a persistence and
 * extension boundary; changing it requires an explicit migration.
 */
static const UmiEditorFeatureDescriptor FEATURE = {
    .struct_size = (uint32_t)sizeof(UmiEditorFeatureDescriptor),
    .api_version = UMI_EDITOR_FEATURE_API_VERSION,
    .feature_id = "umicom.editor.semantic_tokens",
    .title = "Semantic Tokens",
    .category = UMI_EDITOR_FEATURE_CATEGORY_LANGUAGE,
    .maturity = UMI_EDITOR_FEATURE_MATURITY_PREVIEW,
    .summary = "Full and delta semantic token transfer model.",
    .flags = UMI_EDITOR_FEATURE_FLAG_CORE | UMI_EDITOR_FEATURE_FLAG_HEADLESS,
    .priority = 940,
    .dependency_count = 0U,
    .revision = 1U
};

const UmiEditorFeatureDescriptor *umi_editor_feature_semantic_tokens(void)
{
    return &FEATURE;
}
