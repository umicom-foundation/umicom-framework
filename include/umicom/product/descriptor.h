/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/product/descriptor.h
 *
 * PURPOSE:
 *   Describe one independently runnable Umicom product.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Product APIs describe composition and identity only. Reusable mechanisms stay in Framework while each Umicom application supplies its own domain behaviour.
 */
#ifndef UMICOM_PRODUCT_DESCRIPTOR_H
#define UMICOM_PRODUCT_DESCRIPTOR_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/base/version.h"
#include "umicom/product/branding.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiProductKind { UMI_PRODUCT_DEVELOPMENT=1, UMI_PRODUCT_DESIGN=2, UMI_PRODUCT_TRADING=3, UMI_PRODUCT_TREASURY=4, UMI_PRODUCT_MEDIA=5, UMI_PRODUCT_GENERAL=6 } UmiProductKind;
typedef struct UmiProductDescriptor { const char *product_id; UmiProductKind kind; UmiVersion version; UmiProductBranding branding; } UmiProductDescriptor;
UmiStatus umi_product_descriptor_validate(const UmiProductDescriptor *descriptor);
#ifdef __cplusplus
}
#endif
#endif
