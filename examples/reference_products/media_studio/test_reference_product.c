/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/reference_products/media_studio/test_reference_product.c
 *
 * PURPOSE:
 *   Validate the Umicom Media Studio reference profile as an independent Framework consumer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The test checks product identity, profile validation and compatibility with the Framework capability catalogue.
 */
#include <assert.h>
#include <string.h>
#include "reference_product.h"
int main(void){UmiProductValidation v;UmiProductCompatibility c;const UmiProductProfile *p=umi_example_media_studio_profile();assert(p!=NULL);assert(strcmp(p->descriptor->product_id,"org.umicom.media-studio")==0);assert(umi_product_validation_run(p,&v)==UMI_STATUS_OK);assert(v.passed);assert(umi_product_compatibility_check(p,&c)==UMI_STATUS_OK);return 0;}
