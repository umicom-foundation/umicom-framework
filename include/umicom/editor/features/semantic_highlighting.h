/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/features/semantic_highlighting.h
 *
 * PURPOSE:
 *   Publish the product-neutral descriptor for the Semantic Highlighting editor capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * ARCHITECTURE NOTE:
 *   This header is a Framework contract, not a GTK4 or Studio implementation.
 *   Applications discover the capability through UmiEditorFeatureCatalog and
 *   bind a suitable service or adapter without redefining its identity.
 *
 * FEATURE CONTRACT:
 *   Token-type and modifier driven semantic styling.
 *
 * LIFETIME:
 *   The returned immutable descriptor has static storage duration. Callers
 *   must not free or modify it. A catalogue copies the descriptor by value.
 */
#ifndef UMICOM_EDITOR_FEATURES_SEMANTIC_HIGHLIGHTING_H
#define UMICOM_EDITOR_FEATURES_SEMANTIC_HIGHLIGHTING_H

#include "umicom/editor/feature.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Return the canonical Framework descriptor for Semantic Highlighting. */
const UmiEditorFeatureDescriptor *umi_editor_feature_semantic_highlighting(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_FEATURES_SEMANTIC_HIGHLIGHTING_H */
