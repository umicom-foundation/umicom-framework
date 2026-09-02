/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/font_family_descriptor.h
 *
 * PURPOSE:
 *   Describe one semantic font-family candidate and its broad typographic classification.
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
#ifndef UMICOM_UI_APPEARANCE_FONT_FAMILY_DESCRIPTOR_H
#define UMICOM_UI_APPEARANCE_FONT_FAMILY_DESCRIPTOR_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance font family descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceFontFamilyDescriptor {
    char family_id[UMI_APPEARANCE_ID_CAPACITY];
    char family_name[UMI_APPEARANCE_TEXT_CAPACITY];
    char classification[UMI_APPEARANCE_ID_CAPACITY];
    bool monospace;
    bool variable_font;
} UmiAppearanceFontFamilyDescriptor;

/* Initialise one font family descriptor record with deterministic defaults. */
UmiStatus umi_appearance_font_family_descriptor_init(UmiAppearanceFontFamilyDescriptor *item);
/* Validate the required production invariants for this font family descriptor. */
int umi_appearance_font_family_descriptor_is_valid(const UmiAppearanceFontFamilyDescriptor *item);

#ifdef __cplusplus
}
#endif
#endif
