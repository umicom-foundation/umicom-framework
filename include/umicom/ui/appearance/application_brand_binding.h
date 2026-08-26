/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/application_brand_binding.h
 *
 * PURPOSE:
 *   Bind a thin application identity to Framework-owned brand and theme-pack identifiers.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_APPLICATION_BRAND_BINDING_H
#define UMICOM_UI_APPEARANCE_APPLICATION_BRAND_BINDING_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceApplicationBrandBinding {
    char application_id[UMI_APPEARANCE_ID_CAPACITY];
    char brand_id[UMI_APPEARANCE_ID_CAPACITY];
    char theme_pack_id[UMI_APPEARANCE_ID_CAPACITY];
} UmiAppearanceApplicationBrandBinding;

/* Initialise one application brand binding record with deterministic defaults. */
UmiStatus umi_appearance_application_brand_binding_init(UmiAppearanceApplicationBrandBinding *item);
/* Validate the required production invariants for this application brand binding. */
int umi_appearance_application_brand_binding_is_valid(const UmiAppearanceApplicationBrandBinding *item);

#ifdef __cplusplus
}
#endif
#endif
