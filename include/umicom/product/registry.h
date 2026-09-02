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

/*
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
/**
 * Represent the product registry data shared with callers of this public contract.
 */
typedef struct UmiProductRegistry UmiProductRegistry;
/**
 * Initialise product registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_product_registry_create(UmiProductRegistry **out_registry);
/**
 * Release or reset state held by product registry so the same storage can be reused
 * safely.
 */
void umi_product_registry_destroy(UmiProductRegistry *registry);
/**
 * Add product registry only after its inputs and available capacity have been checked.
 */
UmiStatus umi_product_registry_register(UmiProductRegistry *registry,const UmiProductProfile *profile);
/**
 * Find product registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiProductProfile *umi_product_registry_find(const UmiProductRegistry *registry,const char *product_id);
/**
 * Return the number of records represented by product registry without changing their
 * state.
 */
size_t umi_product_registry_count(const UmiProductRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
