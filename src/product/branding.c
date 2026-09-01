/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/product/branding.c
 *
 * PURPOSE:
 *   Keep product name, vendor and executable identity separate from shared Framework code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation keeps product composition separate from reusable Framework mechanisms. It lets independent applications share the same platform without copying infrastructure.
 */
#include "umicom/product/branding.h"
#include <stddef.h>
UmiStatus umi_product_branding_validate(const UmiProductBranding *b){if(b==NULL||b->display_name==NULL||b->display_name[0]=='\0'||b->vendor==NULL||b->executable_name==NULL)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
