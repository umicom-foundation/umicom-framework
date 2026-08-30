/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/product/registry.h
 *
 * PURPOSE:
 *   Register independently runnable products in a bounded product catalogue.
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
#ifndef UMICOM_PRODUCT_REGISTRY_H
#define UMICOM_PRODUCT_REGISTRY_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/product/profile.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProductRegistry UmiProductRegistry;
UmiStatus umi_product_registry_create(UmiProductRegistry **out_registry);
void umi_product_registry_destroy(UmiProductRegistry *registry);
UmiStatus umi_product_registry_register(UmiProductRegistry *registry,const UmiProductProfile *profile);
const UmiProductProfile *umi_product_registry_find(const UmiProductRegistry *registry,const char *product_id);
size_t umi_product_registry_count(const UmiProductRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
