/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/reference_products/designer/test_reference_product.c
 *
 * PURPOSE:
 *   Validate the Umicom Designer reference profile as an independent Framework consumer.
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
int main(void){UmiProductValidation v;UmiProductCompatibility c;const UmiProductProfile *p=umi_example_designer_profile();assert(p!=NULL);assert(strcmp(p->descriptor->product_id,"org.umicom.designer")==0);assert(umi_product_validation_run(p,&v)==UMI_STATUS_OK);assert(v.passed);assert(umi_product_compatibility_check(p,&c)==UMI_STATUS_OK);return 0;}
