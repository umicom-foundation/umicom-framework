/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/features/format_on_type.h
 *
 * PURPOSE:
 *   Publish the product-neutral descriptor for the Format On Type editor capability.
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
 *   Debounced language formatting requests triggered by typed characters.
 *
 * LIFETIME:
 *   The returned immutable descriptor has static storage duration. Callers
 *   must not free or modify it. A catalogue copies the descriptor by value.
 */
#ifndef UMICOM_EDITOR_FEATURES_FORMAT_ON_TYPE_H
#define UMICOM_EDITOR_FEATURES_FORMAT_ON_TYPE_H

#include "umicom/editor/feature.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Return the canonical Framework descriptor for Format On Type. */
const UmiEditorFeatureDescriptor *umi_editor_feature_format_on_type(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_FEATURES_FORMAT_ON_TYPE_H */
