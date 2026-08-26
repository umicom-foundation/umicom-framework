/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/scale_factor.h
 *
 * PURPOSE:
 *   Represent a bounded effective UI scale factor with independent OS and user contributions.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_SCALE_FACTOR_H
#define UMICOM_UI_APPEARANCE_SCALE_FACTOR_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceScaleFactor {
    char scale_id[UMI_APPEARANCE_ID_CAPACITY];
    double os_factor;
    double user_factor;
    double effective_factor;
} UmiAppearanceScaleFactor;

/* Initialise one scale factor record with deterministic defaults. */
UmiStatus umi_appearance_scale_factor_init(UmiAppearanceScaleFactor *item);
/* Validate the required production invariants for this scale factor. */
int umi_appearance_scale_factor_is_valid(const UmiAppearanceScaleFactor *item);

#ifdef __cplusplus
}
#endif
#endif
