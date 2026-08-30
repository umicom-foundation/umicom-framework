/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/product/descriptor.c
 *
 * PURPOSE:
 *   Describe one independently runnable Umicom product.
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
#include "umicom/product/descriptor.h"
#include <stddef.h>
UmiStatus umi_product_descriptor_validate(const UmiProductDescriptor *d){if(d==NULL||d->product_id==NULL||d->product_id[0]=='\0'||d->kind<UMI_PRODUCT_DEVELOPMENT||d->kind>UMI_PRODUCT_GENERAL)return UMI_STATUS_INVALID_ARGUMENT;return umi_product_branding_validate(&d->branding);}
