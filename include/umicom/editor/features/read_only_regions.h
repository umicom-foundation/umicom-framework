/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/features/read_only_regions.h
 *
 * PURPOSE:
 *   Publish the product-neutral descriptor for the Read Only Regions editor capability.
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
 *   Protected ranges for generated, virtual or policy-controlled text.
 *
 * LIFETIME:
 *   The returned immutable descriptor has static storage duration. Callers
 *   must not free or modify it. A catalogue copies the descriptor by value.
 */
#ifndef UMICOM_EDITOR_FEATURES_READ_ONLY_REGIONS_H
#define UMICOM_EDITOR_FEATURES_READ_ONLY_REGIONS_H

#include "umicom/editor/feature.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Return the canonical Framework descriptor for Read Only Regions. */
const UmiEditorFeatureDescriptor *umi_editor_feature_read_only_regions(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_FEATURES_READ_ONLY_REGIONS_H */
