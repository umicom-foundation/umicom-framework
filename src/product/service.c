/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/product/service.c
 *
 * PURPOSE:
 *   Describe a product-owned service that is composed on top of Framework services.
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
#include "umicom/product/service.h"
#include <stddef.h>
UmiStatus umi_product_service_validate(const UmiProductService *s){if(s==NULL||s->service_id==NULL||s->service_id[0]=='\0'||s->provider==NULL)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
