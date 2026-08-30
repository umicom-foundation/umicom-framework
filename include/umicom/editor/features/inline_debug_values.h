/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/features/inline_debug_values.h
 *
 * PURPOSE:
 *   Publish the product-neutral descriptor for the Inline Debug Values editor capability.
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
 *   Paused-frame values rendered beside source expressions.
 *
 * LIFETIME:
 *   The returned immutable descriptor has static storage duration. Callers
 *   must not free or modify it. A catalogue copies the descriptor by value.
 */
#ifndef UMICOM_EDITOR_FEATURES_INLINE_DEBUG_VALUES_H
#define UMICOM_EDITOR_FEATURES_INLINE_DEBUG_VALUES_H

#include "umicom/editor/feature.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Return the canonical Framework descriptor for Inline Debug Values. */
const UmiEditorFeatureDescriptor *umi_editor_feature_inline_debug_values(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_FEATURES_INLINE_DEBUG_VALUES_H */
