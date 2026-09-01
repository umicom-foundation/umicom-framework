/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/product/branding.h
 *
 * PURPOSE:
 *   Keep product name, vendor and executable identity separate from shared Framework code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Product APIs describe composition and identity only. Reusable mechanisms stay in Framework while each Umicom application supplies its own domain behaviour.
 */
#ifndef UMICOM_PRODUCT_BRANDING_H
#define UMICOM_PRODUCT_BRANDING_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProductBranding { const char *display_name; const char *vendor; const char *executable_name; } UmiProductBranding;
UmiStatus umi_product_branding_validate(const UmiProductBranding *branding);
#ifdef __cplusplus
}
#endif
#endif
