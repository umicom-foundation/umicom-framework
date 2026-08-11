/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_product_suite.c
 *
 * PURPOSE:
 *   Validate product suite behaviour for the major release hardening train.
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
int main(void){const UmiProductProfile *items[]={umi_reference_product_studio(),umi_reference_product_designer()};UmiProductSuite s={"umicom.development-suite",items,2U};assert(umi_product_suite_validate(&s)==UMI_STATUS_OK);assert(umi_product_suite_find(&s,"org.umicom.designer")!=NULL);return 0;}
