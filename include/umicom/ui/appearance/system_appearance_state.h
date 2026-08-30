/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/system_appearance_state.h
 *
 * PURPOSE:
 *   Represent operating-system appearance signals without coupling Framework logic to platform APIs.
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
#ifndef UMICOM_UI_APPEARANCE_SYSTEM_APPEARANCE_STATE_H
#define UMICOM_UI_APPEARANCE_SYSTEM_APPEARANCE_STATE_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceSystemAppearanceState {
    char system_id[UMI_APPEARANCE_ID_CAPACITY];
    bool dark_mode;
    bool high_contrast;
    bool reduced_motion;
    uint32_t dpi;
    double scale;
} UmiAppearanceSystemAppearanceState;

/* Initialise one system appearance state record with deterministic defaults. */
UmiStatus umi_appearance_system_appearance_state_init(UmiAppearanceSystemAppearanceState *item);
/* Validate the required production invariants for this system appearance state. */
int umi_appearance_system_appearance_state_is_valid(const UmiAppearanceSystemAppearanceState *item);

#ifdef __cplusplus
}
#endif
#endif
