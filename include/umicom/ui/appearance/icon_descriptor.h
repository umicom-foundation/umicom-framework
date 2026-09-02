/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/icon_descriptor.h
 *
 * PURPOSE:
 *   Describe a semantic icon identity, directionality and scalable/symbolic capabilities.
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
#ifndef UMICOM_UI_APPEARANCE_ICON_DESCRIPTOR_H
#define UMICOM_UI_APPEARANCE_ICON_DESCRIPTOR_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance icon descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceIconDescriptor {
    char icon_id[UMI_APPEARANCE_ID_CAPACITY];
    char semantic_role[UMI_APPEARANCE_ID_CAPACITY];
    bool scalable;
    bool symbolic;
    bool direction_sensitive;
} UmiAppearanceIconDescriptor;

/* Initialise one icon descriptor record with deterministic defaults. */
UmiStatus umi_appearance_icon_descriptor_init(UmiAppearanceIconDescriptor *item);
/* Validate the required production invariants for this icon descriptor. */
int umi_appearance_icon_descriptor_is_valid(const UmiAppearanceIconDescriptor *item);

#ifdef __cplusplus
}
#endif
#endif
