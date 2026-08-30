/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/features/line_operations.h
 *
 * PURPOSE:
 *   Publish the product-neutral descriptor for the Line Operations editor capability.
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
 *   Move, copy, duplicate, delete, join and transpose line commands.
 *
 * LIFETIME:
 *   The returned immutable descriptor has static storage duration. Callers
 *   must not free or modify it. A catalogue copies the descriptor by value.
 */
#ifndef UMICOM_EDITOR_FEATURES_LINE_OPERATIONS_H
#define UMICOM_EDITOR_FEATURES_LINE_OPERATIONS_H

#include "umicom/editor/feature.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Return the canonical Framework descriptor for Line Operations. */
const UmiEditorFeatureDescriptor *umi_editor_feature_line_operations(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_FEATURES_LINE_OPERATIONS_H */
