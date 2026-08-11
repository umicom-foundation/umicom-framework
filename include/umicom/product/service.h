/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/product/service.h
 *
 * PURPOSE:
 *   Describe a product-owned service that is composed on top of Framework services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Product APIs describe composition and identity only. Reusable mechanisms stay in Framework while each Umicom application supplies its own domain behaviour.
 */
#ifndef UMICOM_PRODUCT_SERVICE_H
#define UMICOM_PRODUCT_SERVICE_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProductService { const char *service_id; const char *provider; int required; } UmiProductService;
UmiStatus umi_product_service_validate(const UmiProductService *service);
#ifdef __cplusplus
}
#endif
#endif
