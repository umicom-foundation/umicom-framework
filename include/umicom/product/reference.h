/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/product/reference.h
 *
 * PURPOSE:
 *   Publish canonical reference profiles for Studio, Designer, Trader, TMS and Media Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Product APIs describe composition and identity only. Reusable mechanisms stay in Framework while each Umicom application supplies its own domain behaviour.
 */
#ifndef UMICOM_PRODUCT_REFERENCE_H
#define UMICOM_PRODUCT_REFERENCE_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/product/profile.h"
#ifdef __cplusplus
extern "C" {
#endif
const UmiProductProfile *umi_reference_product_studio(void);
const UmiProductProfile *umi_reference_product_designer(void);
const UmiProductProfile *umi_reference_product_trader(void);
const UmiProductProfile *umi_reference_product_tms(void);
const UmiProductProfile *umi_reference_product_media_studio(void);
#ifdef __cplusplus
}
#endif
#endif
