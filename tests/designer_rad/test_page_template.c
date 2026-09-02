/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_page_template.c
 *
 * PURPOSE:
 *   Validate describe reusable page templates without embedding application-specific logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/page_template.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadPageTemplate item;CHECK(umi_rad_page_template_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_page_template_is_valid(&item));return 0;}
