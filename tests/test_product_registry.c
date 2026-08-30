/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_product_registry.c
 *
 * PURPOSE:
 *   Validate product registry behaviour for the major release hardening train.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This focused test protects one public contract so future Framework changes cannot silently break SDK or product consumers.
 */
#include <assert.h>
#include "umicom/product/product.h"
int main(void){UmiProductRegistry *r=NULL;assert(umi_product_registry_create(&r)==UMI_STATUS_OK);assert(umi_product_registry_register(r,umi_reference_product_studio())==UMI_STATUS_OK);assert(umi_product_registry_count(r)==1U);umi_product_registry_destroy(r);return 0;}
