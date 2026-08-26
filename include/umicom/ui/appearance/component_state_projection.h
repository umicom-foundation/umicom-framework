/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/component_state_projection.h
 *
 * PURPOSE:
 *   Map semantic component state to resolved style and accessibility state identifiers.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_COMPONENT_STATE_PROJECTION_H
#define UMICOM_UI_APPEARANCE_COMPONENT_STATE_PROJECTION_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceComponentStateProjection {
    char component_id[UMI_APPEARANCE_ID_CAPACITY];
    char state_id[UMI_APPEARANCE_ID_CAPACITY];
    char resolved_style_id[UMI_APPEARANCE_ID_CAPACITY];
    bool focus_visible;
    bool disabled;
} UmiAppearanceComponentStateProjection;

/* Initialise one component state projection record with deterministic defaults. */
UmiStatus umi_appearance_component_state_projection_init(UmiAppearanceComponentStateProjection *item);
/* Validate the required production invariants for this component state projection. */
int umi_appearance_component_state_projection_is_valid(const UmiAppearanceComponentStateProjection *item);

#ifdef __cplusplus
}
#endif
#endif
