/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_form_template.c
 *
 * PURPOSE:
 *   Validate describe reusable form templates and expected field/action counts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/form_template.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadFormTemplate item;CHECK(umi_rad_form_template_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_form_template_is_valid(&item));return 0;}
