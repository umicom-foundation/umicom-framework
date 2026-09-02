/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/dpi_class.h
 *
 * PURPOSE:
 *   Classify physical display density for asset selection without hard-coding platform DPI APIs.
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
#ifndef UMICOM_UI_APPEARANCE_DPI_CLASS_H
#define UMICOM_UI_APPEARANCE_DPI_CLASS_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named appearance dpi class values accepted by this public contract.
 */
typedef enum UmiAppearanceDpiClass { UMI_APPEARANCE_DPI_STANDARD=1, UMI_APPEARANCE_DPI_HIGH=2, UMI_APPEARANCE_DPI_VERY_HIGH=3, UMI_APPEARANCE_DPI_ULTRA=4 } UmiAppearanceDpiClass;
/* Classify a positive display DPI into a stable appearance density class. */
UmiStatus umi_appearance_dpi_class_from_dpi(uint32_t dpi,UmiAppearanceDpiClass *out_class);

#ifdef __cplusplus
}
#endif
#endif
