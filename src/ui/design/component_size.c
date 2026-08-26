/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/component_size.c
 *
 * PURPOSE:
 *   Define minimum, preferred and maximum component geometry for adaptive frontend layout.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/component_size.h"

int umi_design_component_size_valid(const UmiDesignComponentSize *size){return size!=NULL&&size->minimum.width>=0&&size->minimum.height>=0&&size->preferred.width>=size->minimum.width&&size->preferred.height>=size->minimum.height&&size->maximum.width>=size->preferred.width&&size->maximum.height>=size->preferred.height?1:0;}
UmiStatus umi_design_component_size_init(UmiDesignComponentSize *size,UmiUiSize minimum,UmiUiSize preferred,UmiUiSize maximum){if(size==NULL)return UMI_STATUS_INVALID_ARGUMENT;size->minimum=minimum;size->preferred=preferred;size->maximum=maximum;return umi_design_component_size_valid(size)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;}
