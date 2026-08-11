/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_product_bootstrap.c
 *
 * PURPOSE:
 *   Validate product bootstrap behaviour for the major release hardening train.
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
int main(void){UmiProductBootstrapResult r;assert(umi_product_bootstrap(umi_reference_product_tms(),&r)==UMI_STATUS_OK);assert(r.ready);return 0;}
