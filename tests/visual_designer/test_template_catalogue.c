/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_template_catalogue.c
 *
 * PURPOSE:
 *   Validate maintain a bounded searchable catalogue of page and form templates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/template_catalogue.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadTemplateCatalogue c;UmiRadPageTemplate p;UmiRadFormTemplate f;CHECK(umi_rad_template_catalogue_init(&c)==UMI_STATUS_OK);CHECK(umi_rad_page_template_init(&p)==UMI_STATUS_OK);CHECK(umi_rad_form_template_init(&f)==UMI_STATUS_OK);CHECK(umi_rad_template_catalogue_add_page(&c,&p)==UMI_STATUS_OK);CHECK(umi_rad_template_catalogue_add_form(&c,&f)==UMI_STATUS_OK);return 0;}
