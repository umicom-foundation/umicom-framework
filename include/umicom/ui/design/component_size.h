/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/component_size.h
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_COMPONENT_SIZE_H
#define INCLUDE_UMICOM_UI_DESIGN_COMPONENT_SIZE_H

#include "umicom/ui/design/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignComponentSize { UmiUiSize minimum; UmiUiSize preferred; UmiUiSize maximum; } UmiDesignComponentSize;
/* Initialise ordered component size constraints. */
UmiStatus umi_design_component_size_init(UmiDesignComponentSize *size, UmiUiSize minimum, UmiUiSize preferred, UmiUiSize maximum);
/* Return one when minimum, preferred and maximum dimensions are consistently ordered. */
int umi_design_component_size_valid(const UmiDesignComponentSize *size);

#ifdef __cplusplus
}
#endif

#endif
