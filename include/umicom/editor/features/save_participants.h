/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/features/save_participants.h
 *
 * PURPOSE:
 *   Publish the product-neutral descriptor for the Save Participants editor capability.
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
 *   Ordered format, fix, trim and code-action save hooks.
 *
 * LIFETIME:
 *   The returned immutable descriptor has static storage duration. Callers
 *   must not free or modify it. A catalogue copies the descriptor by value.
 */
#ifndef UMICOM_EDITOR_FEATURES_SAVE_PARTICIPANTS_H
#define UMICOM_EDITOR_FEATURES_SAVE_PARTICIPANTS_H

#include "umicom/editor/feature.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Return the canonical Framework descriptor for Save Participants. */
const UmiEditorFeatureDescriptor *umi_editor_feature_save_participants(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_FEATURES_SAVE_PARTICIPANTS_H */
