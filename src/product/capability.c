/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/product/capability.c
 *
 * PURPOSE:
 *   Describe a Framework capability required or optionally used by a product.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation keeps product composition separate from reusable Framework mechanisms. It lets independent applications share the same platform without copying infrastructure.
 */
#include "umicom/product/capability.h"
#include <stddef.h>
UmiStatus umi_product_capability_validate(const UmiProductCapability *c){if(c==NULL||c->capability_id==NULL||c->capability_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
