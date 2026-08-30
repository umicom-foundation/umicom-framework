/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/product/manifest.h
 *
 * PURPOSE:
 *   Create a compact human-readable product manifest for generated repositories.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Product APIs describe composition and identity only. Reusable mechanisms stay in Framework while each Umicom application supplies its own domain behaviour.
 */
#ifndef UMICOM_PRODUCT_MANIFEST_H
#define UMICOM_PRODUCT_MANIFEST_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/product/profile.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_product_manifest_format(const UmiProductProfile *profile,char *buffer,size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
