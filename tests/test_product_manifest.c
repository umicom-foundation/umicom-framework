/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_product_manifest.c
 *
 * PURPOSE:
 *   Validate product manifest behaviour for the major release hardening train.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This focused test protects one public contract so future Framework changes cannot silently break SDK or product consumers.
 */
#include <assert.h>
#include <string.h>
#include "umicom/product/product.h"
int main(void){char b[512];assert(umi_product_manifest_format(umi_reference_product_trader(),b,sizeof(b))==UMI_STATUS_OK);assert(strstr(b,"org.umicom.trader")!=NULL);return 0;}
