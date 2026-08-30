/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/features/document_format.h
 *
 * PURPOSE:
 *   Publish the product-neutral descriptor for the Document Format editor capability.
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
 *   Whole-document and selected-range formatting requests.
 *
 * LIFETIME:
 *   The returned immutable descriptor has static storage duration. Callers
 *   must not free or modify it. A catalogue copies the descriptor by value.
 */
#ifndef UMICOM_EDITOR_FEATURES_DOCUMENT_FORMAT_H
#define UMICOM_EDITOR_FEATURES_DOCUMENT_FORMAT_H

#include "umicom/editor/feature.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Return the canonical Framework descriptor for Document Format. */
const UmiEditorFeatureDescriptor *umi_editor_feature_document_format(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_FEATURES_DOCUMENT_FORMAT_H */
