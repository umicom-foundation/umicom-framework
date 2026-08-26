/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/palette_filter.h
 *
 * PURPOSE:
 *   Filter the component palette by text, category and capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_PALETTE_FILTER_H
#define UMICOM_DESIGNER_RAD_PALETTE_FILTER_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadPaletteFilter {
    char query[UMI_RAD_TEXT_CAPACITY];
    char category[UMI_RAD_ID_CAPACITY];
    char capability[UMI_RAD_ID_CAPACITY];
    bool favourites_only;
} UmiRadPaletteFilter;
UmiStatus umi_rad_palette_filter_init(UmiRadPaletteFilter *item);
int umi_rad_palette_filter_is_valid(const UmiRadPaletteFilter *item);
#ifdef __cplusplus
}
#endif
#endif
