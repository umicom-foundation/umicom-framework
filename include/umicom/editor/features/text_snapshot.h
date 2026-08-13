/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/features/text_snapshot.h
 *
 * PURPOSE:
 *   Publish the product-neutral descriptor for the Text Snapshot editor capability.
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
 *   Immutable document revision views for readers and background services.
 *
 * LIFETIME:
 *   The returned immutable descriptor has static storage duration. Callers
 *   must not free or modify it. A catalogue copies the descriptor by value.
 */
#ifndef UMICOM_EDITOR_FEATURES_TEXT_SNAPSHOT_H
#define UMICOM_EDITOR_FEATURES_TEXT_SNAPSHOT_H

#include "umicom/editor/feature.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Return the canonical Framework descriptor for Text Snapshot. */
const UmiEditorFeatureDescriptor *umi_editor_feature_text_snapshot(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_FEATURES_TEXT_SNAPSHOT_H */
