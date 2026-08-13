/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/features/editor_gutter.h
 *
 * PURPOSE:
 *   Publish the product-neutral descriptor for the Editor Gutter editor capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/*
 * ARCHITECTURE NOTE:
 *   This header is a Framework contract, not a GTK4 or Studio implementation.
 *   Applications discover the capability through UmiEditorFeatureCatalog and
 *   bind a suitable service or adapter without redefining its identity.
 *
 * FEATURE CONTRACT:
 *   Composable gutter lanes for line numbers, folds and markers.
 *
 * LIFETIME:
 *   The returned immutable descriptor has static storage duration. Callers
 *   must not free or modify it. A catalogue copies the descriptor by value.
 */
#ifndef UMICOM_EDITOR_FEATURES_EDITOR_GUTTER_H
#define UMICOM_EDITOR_FEATURES_EDITOR_GUTTER_H

#include "umicom/editor/feature.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Return the canonical Framework descriptor for Editor Gutter. */
const UmiEditorFeatureDescriptor *umi_editor_feature_editor_gutter(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_FEATURES_EDITOR_GUTTER_H */
