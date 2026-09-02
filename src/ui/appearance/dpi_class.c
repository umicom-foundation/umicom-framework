/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/dpi_class.c
 *
 * PURPOSE:
 *   Classify physical display density for asset selection without hard-coding platform DPI APIs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/dpi_class.h"
/*
 * Provide the appearance dpi class from dpi operation used by this module and its client
 * applications.
 */
UmiStatus umi_appearance_dpi_class_from_dpi(uint32_t dpi,UmiAppearanceDpiClass *out_class){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_class==NULL||dpi==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(dpi<=120U)*out_class=UMI_APPEARANCE_DPI_STANDARD;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(dpi<=180U)*out_class=UMI_APPEARANCE_DPI_HIGH;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(dpi<=260U)*out_class=UMI_APPEARANCE_DPI_VERY_HIGH;/* Use this fallback path when the earlier condition does not apply. */ else *out_class=UMI_APPEARANCE_DPI_ULTRA;return UMI_STATUS_OK;}
