/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/reference_products/tms/main.c
 *
 * PURPOSE:
 *   Launch the headless Umicom Treasury Management System reference consumer and print its Framework composition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This small executable is a smoke test for independent Framework consumption. It does not implement the final product UI or business domain.
 */
#include <stdio.h>
#include "reference_product.h"
int main(void){UmiProductBootstrapResult r;const UmiProductProfile *p=umi_example_tms_profile();if(umi_product_bootstrap(p,&r)!=UMI_STATUS_OK)return 1;printf("Umicom Treasury Management System | id=%s | required=%zu | frontends=%zu\n",r.product_id,r.required_capabilities,r.enabled_frontends);return r.ready?0:2;}
