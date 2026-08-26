/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/component_catalogue.h
 *
 * PURPOSE:
 *   Maintain a searchable bounded catalogue of reusable semantic component descriptors.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_COMPONENT_CATALOGUE_H
#define INCLUDE_UMICOM_UI_DESIGN_COMPONENT_CATALOGUE_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/component_descriptor.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/design/component_descriptor.h"
typedef struct UmiDesignComponentCatalogue { UmiDesignComponentDescriptor items[UMI_DESIGN_MAX_ITEMS]; size_t count; uint64_t revision; } UmiDesignComponentCatalogue;
/* Register or replace one semantic component descriptor by identifier. */
UmiStatus umi_design_component_catalogue_upsert(UmiDesignComponentCatalogue *catalogue, const UmiDesignComponentDescriptor *descriptor);
/* Resolve one semantic component descriptor by identifier. */
UmiStatus umi_design_component_catalogue_find(const UmiDesignComponentCatalogue *catalogue, const char *id, UmiDesignComponentDescriptor *out_descriptor);

#ifdef __cplusplus
}
#endif

#endif
