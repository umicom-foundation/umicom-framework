/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/surface_semantics.h
 *
 * PURPOSE:
 *   Describe semantic surface hierarchy and elevation intent independently of renderer primitives.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_SURFACE_SEMANTICS_H
#define UMICOM_UI_APPEARANCE_SURFACE_SEMANTICS_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance surface semantics data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceSurfaceSemantics {
    char surface_id[UMI_APPEARANCE_ID_CAPACITY];
    char background_role[UMI_APPEARANCE_ID_CAPACITY];
    char foreground_role[UMI_APPEARANCE_ID_CAPACITY];
    int32_t elevation_level;
} UmiAppearanceSurfaceSemantics;

/* Initialise one surface semantics record with deterministic defaults. */
UmiStatus umi_appearance_surface_semantics_init(UmiAppearanceSurfaceSemantics *item);
/* Validate the required production invariants for this surface semantics. */
int umi_appearance_surface_semantics_is_valid(const UmiAppearanceSurfaceSemantics *item);

#ifdef __cplusplus
}
#endif
#endif
