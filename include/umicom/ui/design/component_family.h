/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/component_family.h
 *
 * PURPOSE:
 *   Group semantic component identifiers into discoverable families such as input, navigation and analytics.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_COMPONENT_FAMILY_H
#define INCLUDE_UMICOM_UI_DESIGN_COMPONENT_FAMILY_H

#include "umicom/ui/design/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignComponentFamily { char id[UMI_DESIGN_ID_CAPACITY]; char component_ids[32][UMI_DESIGN_ID_CAPACITY]; size_t count; } UmiDesignComponentFamily;
/* Initialise a named semantic component family. */
UmiStatus umi_design_component_family_init(UmiDesignComponentFamily *family, const char *id);
/* Add a unique component identifier to the family. */
UmiStatus umi_design_component_family_add(UmiDesignComponentFamily *family, const char *component_id);
/* Return one when the family contains a component identifier. */
int umi_design_component_family_contains(const UmiDesignComponentFamily *family, const char *component_id);

#ifdef __cplusplus
}
#endif

#endif
