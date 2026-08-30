/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/typography.h
 *
 * PURPOSE:
 *   Define validated toolkit-neutral typography specifications for semantic text roles.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_TYPOGRAPHY_H
#define INCLUDE_UMICOM_UI_DESIGN_TYPOGRAPHY_H

#include "umicom/ui/design/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignTypography { char family[UMI_DESIGN_NAME_CAPACITY]; double size; uint16_t weight; double line_height; double letter_spacing; } UmiDesignTypography;

/* Initialise a typography specification with bounded family text and validated metrics. */
UmiStatus umi_design_typography_init(UmiDesignTypography *spec, const char *family, double size, uint16_t weight, double line_height);
/* Return one when the typography metrics can be rendered consistently. */
int umi_design_typography_valid(const UmiDesignTypography *spec);

#ifdef __cplusplus
}
#endif

#endif
