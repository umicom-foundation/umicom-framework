/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/features/go_to_reference.h
 *
 * PURPOSE:
 *   Publish the product-neutral descriptor for the Go To Reference editor capability.
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
 *   Cross-document reference result sets and navigation.
 *
 * LIFETIME:
 *   The returned immutable descriptor has static storage duration. Callers
 *   must not free or modify it. A catalogue copies the descriptor by value.
 */
#ifndef UMICOM_EDITOR_FEATURES_GO_TO_REFERENCE_H
#define UMICOM_EDITOR_FEATURES_GO_TO_REFERENCE_H

#include "umicom/editor/feature.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Return the canonical Framework descriptor for Go To Reference. */
const UmiEditorFeatureDescriptor *umi_editor_feature_go_to_reference(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_FEATURES_GO_TO_REFERENCE_H */
