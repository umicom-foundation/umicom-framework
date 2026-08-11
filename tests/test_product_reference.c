/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_product_reference.c
 *
 * PURPOSE:
 *   Validate product reference behaviour for the major release hardening train.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This focused test protects one public contract so future Framework changes cannot silently break SDK or product consumers.
 */
#include <assert.h>
#include "umicom/product/product.h"
int main(void){assert(umi_reference_product_count()==5U);assert(umi_reference_product_find("org.umicom.trader")!=NULL);assert(umi_reference_product_find("org.umicom.media-studio")!=NULL);return 0;}
