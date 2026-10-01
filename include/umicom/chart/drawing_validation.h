/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/drawing_validation.h
 * PURPOSE: Validate drawing geometry before persistence or atomic replacement.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CHART_DRAWING_VALIDATION_H
#define UMICOM_CHART_DRAWING_VALIDATION_H
#include "umicom/chart/drawing.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Supplement the established bounded-text validator with finite coordinates,
 * nonempty pane/tool identities and boolean selected/locked flags. Tool names
 * remain extensible and timestamps may precede the Unix epoch. No mutation. */
UmiStatus UmiChartDrawingValidateGeometry(const UmiChartDrawingSnapshot *drawing);
#ifdef __cplusplus
}
#endif
#endif
