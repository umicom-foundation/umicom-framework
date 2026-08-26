/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/dpi_class.c
 *
 * PURPOSE:
 *   Classify physical display density for asset selection without hard-coding platform DPI APIs.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/dpi_class.h"
UmiStatus umi_appearance_dpi_class_from_dpi(uint32_t dpi,UmiAppearanceDpiClass *out_class){if(out_class==NULL||dpi==0U)return UMI_STATUS_INVALID_ARGUMENT;if(dpi<=120U)*out_class=UMI_APPEARANCE_DPI_STANDARD;else if(dpi<=180U)*out_class=UMI_APPEARANCE_DPI_HIGH;else if(dpi<=260U)*out_class=UMI_APPEARANCE_DPI_VERY_HIGH;else *out_class=UMI_APPEARANCE_DPI_ULTRA;return UMI_STATUS_OK;}
